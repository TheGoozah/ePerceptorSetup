#pragma once

#ifdef WIN32
#include <Windows.h>
#endif

#include <chrono>
using namespace std;

/*
* ETTimer: holds and handles the Time Information (used for animation or debugging eg)
*/
class ETTimer final
{
public:
	//Constructor & Destructor
	ETTimer();
	virtual ~ETTimer();

	//Functions
	void Update();

	//Getters
	double GetDeltaTime();
	double GetElapsedTimeInSeconds();

private:
	//Datamembers
	typedef chrono::high_resolution_clock Clock;
	Clock::time_point m_startTime;
	Clock::time_point m_lastEndTime;
	typedef chrono::duration<long, std::ratio<1, 60>> Sixtieth_Of_A_Second;
	double m_elapsedTime;
	double m_deltaTime;
};