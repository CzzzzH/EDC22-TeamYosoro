#ifndef JY61_HPP
#define JY61_HPP

class JY61
{
public:
	static double Acc[3];
	static double Gyro[3];
	static double Angle[3];

	static bool isDebug;

	static void print();

	static void read();
};

#endif