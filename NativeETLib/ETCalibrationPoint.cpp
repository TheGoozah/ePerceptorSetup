/*
Implementation of the ETCalibrationPoint functionality
*/
#pragma once

#include "stdafx.h"
#include "ETCalibrationPoint.h"

#ifdef _DEBUG
#include "ETDebug.h"
#include <iostream>
#endif

#define M_PI 3.14159265358979323846

//Constructor
ETCalibrationPoint::ETCalibrationPoint(const double X, const double Y, const double Scale, const double MinScale, const double MaxScale) :
m_dScale(Scale),
m_dStartScale(Scale),
m_dMinScale(MinScale),
m_dMaxScale(MaxScale),
m_dInternalTimer(0.0),
m_dMoveDistance(0.0),
m_bInFocus(false)
{
	m_pointData.X = X;
	m_pointData.Y = Y;

	m_dStartScale = m_dScale;
	m_startPosition.X = X;
	m_startPosition.Y = Y;
}

//Destructor
ETCalibrationPoint::~ETCalibrationPoint()
{}

//Copy Constructor
ETCalibrationPoint::ETCalibrationPoint(const ETCalibrationPoint& source):
m_dInternalTimer(0.0),
m_dMoveDistance(0.0),
m_bInFocus(false)
{
	this->m_pointData.X = source.m_pointData.X;
	this->m_pointData.Y = source.m_pointData.Y;

	this->m_dScale = source.m_dScale;
	this->m_dMinScale = source.m_dMinScale;
	this->m_dMaxScale = source.m_dMaxScale;

	m_dStartScale = m_dScale;
	m_startPosition.X = source.m_pointData.X;
	m_startPosition.Y = source.m_pointData.Y;
}

//Public Functions
bool ETCalibrationPoint::Shrink(const double deltaTime, const double timeSpan)
{
	//Scale our point based on deltTime to MinScale in given timeSpan
	//if our point reached it's end, return true and reset timer, else return false
	//Start by holding the start scale

	//Lerp
	m_dInternalTimer += deltaTime;
	this->m_dScale = m_dStartScale + (m_dInternalTimer / timeSpan) * (m_dMinScale - m_dStartScale);

	//Check if finished
	if (m_dScale > m_dMinScale)
		return false;
	else
	{
		m_dInternalTimer = 0.0;
		m_dStartScale = m_dScale;
		//printf("End shrink with Scale: %f \n", m_dScale);
		return true;
	}
}

bool ETCalibrationPoint::Grow(const double deltaTime, const double timeSpan)
{
	//Scale our point based on deltTime to MinScale in given timeSpan
	//if our point reached it's end, return true and reset timer, else return false
	//Start by holding the start scale

	//Lerp
	m_dInternalTimer += deltaTime;
	this->m_dScale = m_dStartScale + (m_dInternalTimer / timeSpan) * (m_dMaxScale - m_dStartScale);

	//Check if finished
	if (m_dScale < m_dMaxScale)
		return false;
	else
	{
		m_dInternalTimer = 0.0;
		m_dStartScale = m_dScale;
		//printf("End grow with Scale: %f \n", m_dScale);
		return true;
	}
}

bool ETCalibrationPoint::Move(const double deltaTime, ET_Point_2D target)
{
	//Lerp
	m_dInternalTimer += deltaTime;
	this->m_pointData.X = m_startPosition.X + (m_dInternalTimer / 1.0) * (target.X - m_startPosition.X);
	this->m_pointData.Y = m_startPosition.Y + (m_dInternalTimer / 1.0) * (target.Y - m_startPosition.Y);

	//Check if finished
	double distance = sqrt(((target.X - this->m_pointData.X) * (target.X - this->m_pointData.X)) + ((target.Y - this->m_pointData.Y) * (target.Y - this->m_pointData.Y)));
	//printf("Distance: %f \n", distance);
	if (distance > 0.01)
		return false;
	else
	{
		m_dInternalTimer = 0.0;
		m_startPosition.X = this->m_pointData.X;
		m_startPosition.Y = this->m_pointData.Y;
		return true;
	}
}

void ETCalibrationPoint::Update()
{
	//
}

//Private Functions