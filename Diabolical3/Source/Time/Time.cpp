#include "Time.h"
#include <chrono>

std::chrono::steady_clock::time_point AppStart = std::chrono::steady_clock::now();

float DTime::GetSecondsSinceStart()
{
	std::chrono::steady_clock::time_point Now = std::chrono::steady_clock::now();
	return ((float)std::chrono::duration_cast<std::chrono::nanoseconds>(Now - AppStart).count() / 1000000000);
}