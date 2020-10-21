#include <list>

typedef void (*Func)(void);

std::list<Func> exec_10ms = std::list<Func>();
std::list<Func> exec_100ms = std::list<Func>();

void Interrupt_100ms()
{
	for (auto func : exec_100ms)
		func();
}

void Interrupt_10ms()
{
	static int count = 0;

	for (auto func : exec_10ms)
		func();

	if (count == 10)
	{
		Interrupt_100ms();
		count = 0;
	}
	count++;
}

void add_10ms(Func func)
{
	exec_10ms.push_back(func);
}

void add_100ms(Func func)
{
	exec_100ms.push_back(func);
}


