#include <map>
#include <list>
#include <MsTimer2.h>
#include "TimerInterrupt.h"
#include <Arduino.h>

void TimerInterrupt::unit_interrupt()
{
	for (auto timer : timer_list)
		timer->execute();
}

void TimerInterrupt::initialize(unsigned long interval)
{
	MsTimer2::set(interval, unit_interrupt);
	MsTimer2::start();
}

TimerInterrupt::TimerInterrupt(unsigned int period, Func func) : times((double)period / (double)interrupt_period), func(func)
{
	Serial.println("times : " + String(times));
	timer_list.push_back(this);
}

void TimerInterrupt::execute()
{
	this->count = this->count + 1;
	if (count >= times)
	{
		func();
		count = 0;
	}
}
