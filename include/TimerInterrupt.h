#ifndef TIMERINTERRUPT_HPP
#define TIMERINTERRUPT_HPP

#include <map>
#include <list>
#include <MsTimer2.h>

class TimerInterrupt
{
public:
	typedef void (*Func)(void);

private:
	static std::list<TimerInterrupt *> timer_list;
	static void unit_interrupt();
	unsigned int count = 0;
	unsigned int times = 1;
	Func func;

public:
	static void initialize(unsigned long interval);
	TimerInterrupt(unsigned int times, Func func);

	void execute();
};

const int interrupt_period = 10;

#endif