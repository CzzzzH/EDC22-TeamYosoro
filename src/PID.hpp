#include <Arduino.h>
#include <map>
#include <set>

class PID
{
public:
	typedef void (*Control)(double input);

private:
	static int count;
	static std::map<int, PID *> pid_dict;

	int id = 0;
	double P = 0;
	double I = 0;
	double D = 0;
	Control control;
	double *monitor_value;

	double last_error = 0;
	double error_sum = 0;

public:
	double target = 0;

	PID(double P, double I, double D, Control control, double *monitor_value) : id(count), P(P), I(I), D(D), control(control), monitor_value(monitor_value)
	{
		pid_dict[id] = this;
		count++;
	}

	~PID()
	{
		pid_dict.erase(id);
	}

	static void run()
	{
		for (auto pair : pid_dict)
		{
			PID *pid = pair.second;
			double error = pid->target - *(pid->monitor_value);
			Serial.print("pid target : ");
			Serial.print(pid->target);
			Serial.print("    ");

			Serial.print("pid monitor value : ");
			Serial.print(*(pid->monitor_value));
			Serial.print("    ");

			pid->error_sum += error;
			pid->error_sum = std::min(100000.0, pid->error_sum);

			Serial.print("pid error sum : ");
			Serial.print(pid->error_sum);
			Serial.print("    ");

			Serial.print("pid input : ");
			Serial.print(pid->P * error + pid->I * pid->error_sum + pid->D * (error - pid->last_error));
			Serial.print("    ");

			pid->control(pid->P * error + pid->I * pid->error_sum + pid->D * (error - pid->last_error));
			Serial.println("");
			pid->last_error = error;
		}
	}
};

int PID::count = 0;
std::map<int, PID *> PID::pid_dict = std::map<int, PID *>();