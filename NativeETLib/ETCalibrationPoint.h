#pragma once

#include "ETTypes.h"
#include "include/tobiigaze.h"

/*
* ETCalibrationPoint: contains the data of the calibration point and handles the movement procedure
*/
class ETCalibrationPoint final
{
public:
	//Constructor & Destructor
	ETCalibrationPoint(const double X, const double Y, const double Scale = 1.0, const double MinScale = 0.1, const double MaxScale = 1.0);
	virtual ~ETCalibrationPoint();

	//Copy Constructor
	ETCalibrationPoint(const ETCalibrationPoint& source);

	//Functions
	bool Shrink(const double deltaTime, const double timeSpan);
	bool Grow(const double deltaTime, const double timeSpan);
	bool Move(const double deltaTime, ET_Point_2D target);
	void Update();

	//Getters & Setters
	ET_Point_2D GetPoint2D(){ return m_pointData; };
	double GetScale(){ return m_dScale; };
	double GetMinScale(){ return m_dMinScale; };
	double GetMaxScale(){ return m_dMaxScale; };
	bool IsInFocus(){ return m_bInFocus; };

	void SetPoint2D(const ET_Point_2D point){ m_pointData.X = point.X; m_pointData.Y = point.Y; };
	void SetScale(const double value){ m_dScale = value; m_dStartScale = value; };
	void SetInFocus(){ m_bInFocus = true; };
	void SetUnFocus(){ m_bInFocus = false; };

private:
	//Datamembers
	ET_Point_2D m_pointData;
	ET_Point_2D m_startPosition;
	double m_dScale;
	double m_dStartScale;
	double m_dMinScale;
	double m_dMaxScale;
	double m_dInternalTimer;
	double m_dMoveDistance;
	double m_dMoveDirectionX;
	double m_dMoveDirectionY;
	bool m_bInFocus;

	//Functions
};