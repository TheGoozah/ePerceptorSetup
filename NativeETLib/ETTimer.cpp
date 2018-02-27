/*
Implementation of the ETTimer functionality
*/
#pragma once

#include "stdafx.h"
#include "ETTimer.h"
#include "ETHost.h"
#include <math.h>

//http://www.songho.ca/misc/timer/timer.html
//http://www.braynzarsoft.net/index.php?p=D3D11HRTIMER

//Constructor
ETTimer::ETTimer():
m_elapsedTime(0.0),
m_deltaTime(0.0)
{
	m_startTime = Clock::now();
	m_lastEndTime = Clock::now();
}

//Destructor
ETTimer::~ETTimer()
{}

//Public Functions
void ETTimer::Update()
{
	//Get the current time
	auto timeNow = Clock::now();

	//Calculate delta
#pragma warning disable 4244
	m_deltaTime = chrono::duration_cast<chrono::milliseconds>(timeNow - m_lastEndTime).count();
#pragma warning restore 4244
	m_deltaTime /= 1000.0;
	m_elapsedTime += m_deltaTime;

	//Store this time for next cycle
	m_lastEndTime = timeNow;
}

double ETTimer::GetDeltaTime()
{ 
	return m_deltaTime;
};

double ETTimer::GetElapsedTimeInSeconds() 
{ 
	return m_elapsedTime;
};