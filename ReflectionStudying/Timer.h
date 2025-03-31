#pragma once
#include <chrono>

class Timer
{
public:
	Timer();
	~Timer();

	void Start();
	void Stop();
	double ElapsedMilliseconds() const;
	double ElpasedMicroseconds() const;
	double ElapsedNanoseconds() const;

private:
	std::chrono::high_resolution_clock::time_point startTime;
	std::chrono::high_resolution_clock::time_point endTime;
};
