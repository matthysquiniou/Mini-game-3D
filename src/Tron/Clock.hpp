#pragma once
#include <chrono>

class Clock {
public:
	Clock();
	float getDtAsSeconds();
private:
	std::chrono::time_point<std::chrono::high_resolution_clock> m_previous_time;
};