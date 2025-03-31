#include "Timer.h"
#include <iostream>

Timer::Timer() :startTime(std::chrono::high_resolution_clock::now()), endTime(startTime)
{
}

Timer::~Timer()
{
    Stop();
    std::cout << "Elapsed Time: " << ElapsedMilliseconds() << " ms\n";
}

void Timer::Start()
{
    startTime = std::chrono::high_resolution_clock::now();
}

void Timer::Stop()
{
    endTime = std::chrono::high_resolution_clock::now();
}

double Timer::ElapsedMilliseconds() const
{
    return std::chrono::duration<double, std::milli>(endTime - startTime).count();
}

double Timer::ElpasedMicroseconds() const
{
	return std::chrono::duration<double, std::micro>(endTime - startTime).count();
}

double Timer::ElapsedNanoseconds() const
{
	return std::chrono::duration<double, std::nano>(endTime - startTime).count();
}
