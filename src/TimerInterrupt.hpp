#include <map>
#include <list>
#include <MsTimer2.h>

class TimerInterrupt
{
public:
	typedef void (*Func)(void);

private:
	static std::list<TimerInterrupt *> timer_list;
	static void unit_interrupt()
	{
		for (auto timer : timer_list)
			timer->execute();
	}

	unsigned int count = 0;
	unsigned int times = 1;
	Func func;

public:
	static void initialize(unsigned long interval)
	{
		MsTimer2::set(interval, unit_interrupt);
		MsTimer2::start();
	}

	TimerInterrupt(unsigned int times, Func func) : times(times), func(func)
	{
		timer_list.push_back(this);
	}

	void execute()
	{
		this->count = this->count + 1;
		if (count >= times)
		{
			func();
			count = 0;
		}
	}
};

std::list<TimerInterrupt *>
	TimerInterrupt::timer_list = std::list<TimerInterrupt *>();