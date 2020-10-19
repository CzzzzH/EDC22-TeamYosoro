// #include "PID.h"

// class PID
// {
// public:
// 	typedef void (*Control)(double input);

// private:
// 	static int count;
// 	static std::map<int, PID *> pid_dict;

// 	int id = 0;
// 	double P = 0;
// 	double I = 0;
// 	double D = 0;
// 	Control control;
// 	double *monitor_value;

// public:
// 	double target = 0;

// 	PID(double P, double I, double D, Control control, double *monitor_value) : id(count), P(P), I(I), D(D), control(control), monitor_value(monitor_value)
// 	{
// 		pid_dict[id] = this;
// 		count++;
// 	}

// 	~PID()
// 	{
// 		pid_dict.erase(id);
// 	}

// 	static void run()
// 	{
// 		for (auto pid_pair : pid_dict)
// 		{
// 			pid_pair.second->control(pid_pair.second->P * (pid_pair.second->target - *(pid_pair.second->monitor_value)));
// 		}
// 	}
// };
