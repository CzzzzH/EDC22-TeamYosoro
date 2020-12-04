#ifndef TIMERINTERRUPT_HPP
#define TIMERINTERRUPT_HPP

#include <map>
#include <list>
#include <MsTimer2.h>

#define INTERRUPT_INTERVAL 50
class TimerInterrupt
{
public:
	typedef void (*Func)(void);
    static std::list<TimerInterrupt *> timer_list;

private:
	static void unit_interrupt();
	unsigned int count = 0;
	unsigned int times = 1;
	Func func;

public:
	static void initialize(unsigned long interval);
	TimerInterrupt(unsigned int period , Func func);

	void execute();
};

const int interrupt_period = 10;

#endif