/*
Implementation of the ETCalibrator functionality
*/
#pragma once

#include "stdafx.h"
#include "ETCalibrator.h"
#include "ETHost.h"

//Declare statics
tobiigaze_eye_tracker* ETCalibrator::g_pAttachedTrackingDevice;
CalibrationState ETCalibrator::g_eCurrentState;

//Constructor
ETCalibrator::ETCalibrator(tobiigaze_eye_tracker* trackingDevice):
m_iCurrentTargetPointIndex(0),
m_pCurrentCalibrationPoint(nullptr),
m_bPositioningRequired(false),
m_bIsCalibrated(false),
m_bCalibratingPoint(false),
m_bComputingCalibration(false)
{
	//Store attached tracking device
	ETCalibrator::g_pAttachedTrackingDevice = trackingDevice;
	ETCalibrator::g_eCurrentState = CalibrationState::INACTIVE;

	//Fill our point array, TODO: add points from external file 
	m_vCalibrationPoints.push_back(new ETCalibrationPoint(0.5,0.5));
	m_vCalibrationPoints.push_back(new ETCalibrationPoint(0.9, 0.5));
	m_vCalibrationPoints.push_back(new ETCalibrationPoint(0.9, 0.1));
	m_vCalibrationPoints.push_back(new ETCalibrationPoint(0.5, 0.1));
	m_vCalibrationPoints.push_back(new ETCalibrationPoint(0.1, 0.1));
	m_vCalibrationPoints.push_back(new ETCalibrationPoint(0.1, 0.5));
	m_vCalibrationPoints.push_back(new ETCalibrationPoint(0.1, 0.9));
	m_vCalibrationPoints.push_back(new ETCalibrationPoint(0.5, 0.9));
	m_vCalibrationPoints.push_back(new ETCalibrationPoint(0.9, 0.9));
}

//Destructor
ETCalibrator::~ETCalibrator()
{
	//Delete all calibration points in vector
	for each (auto cp in m_vCalibrationPoints)
		SAFEDELETE(cp);
	m_vCalibrationPoints.clear();

	SAFEDELETE(m_pCurrentCalibrationPoint);
}

//Public Functions
void ETCalibrator::StartCalibration()
{
	//Reset variables and start the calibration procedure
	m_iCurrentTargetPointIndex = 0;
	ETCalibrator::g_eCurrentState = CalibrationState::INTIALIZE;
}


void ETCalibrator::Update()
{
	switch (ETCalibrator::g_eCurrentState)
	{
		bool finished;
	case CalibrationState::INTIALIZE:
		//Check if data
		if (m_vCalibrationPoints.size() <= 0)
		{
			ETCalibrator::g_eCurrentState = CalibrationState::FINISHED;
			break;
		}
		//Create our dumy calibration point and copy data from the first calibration point in our list
		m_pCurrentCalibrationPoint = new ETCalibrationPoint(*m_vCalibrationPoints.at(m_iCurrentTargetPointIndex));
		//Set variables
		m_bIsCalibrated = false;
		//Start positioning, if not required, start focusing for this first point
		if (m_bPositioningRequired)
			ETCalibrator::g_eCurrentState = CalibrationState::POSITIONING;
		else
			//Tell Tobii to start the calibration process
			tobiigaze_calibration_start_async(ETCalibrator::g_pAttachedTrackingDevice, StartCompleted, nullptr);
		break;
	case CalibrationState::POSITIONING:
		//ADD POSITIONING CODE HERE
		//Go to the focus for the first point
		ETCalibrator::g_eCurrentState = CalibrationState::FOCUS_POINT;
		break;
	case CalibrationState::FETCH_POINT:
		//Reset variables
		m_bCalibratingPoint = false;
		//Get the next point in the list (if any)
		if (m_iCurrentTargetPointIndex + 1 >= m_vCalibrationPoints.size())
		{
			ETCalibrator::g_eCurrentState = CalibrationState::COMPUTE_CALIBRATION;
			break;
		}
		//Get index of next target
		++m_iCurrentTargetPointIndex;
		ETCalibrator::g_eCurrentState = CalibrationState::MOVE_POINT;
		break;
	case CalibrationState::MOVE_POINT:
		//Move to target
		finished = m_pCurrentCalibrationPoint->Move(ETHOST->GetDeltaTime(), m_vCalibrationPoints.at(m_iCurrentTargetPointIndex)->GetPoint2D());
		if (finished)
			ETCalibrator::g_eCurrentState = CalibrationState::FOCUS_POINT;
		break;
	case CalibrationState::FOCUS_POINT:
		//Start shrinking the point and check if ready
		finished = m_pCurrentCalibrationPoint->Shrink(ETHOST->GetDeltaTime(), 1.0);
		if (finished)
			ETCalibrator::g_eCurrentState = CalibrationState::CALIBRATE_POINT;
		break;
	case CalibrationState::UNFOCUS_POINT:
		//Start growing the point and check if ready
		finished = m_pCurrentCalibrationPoint->Grow(ETHOST->GetDeltaTime(), 1.0);
		if (finished)
			ETCalibrator::g_eCurrentState = CalibrationState::FETCH_POINT;
		break;
	case CalibrationState::CALIBRATE_POINT:
		//Calibrate the point
		if (m_bCalibratingPoint == false)
		{
			printf("Calibrate State \n");
			tobiigaze_error_code error_code = TOBIIGAZE_ERROR_SUCCESS;
			CalibrationPoint(error_code, m_pCurrentCalibrationPoint);
			m_bCalibratingPoint = true;
		}
		break;
	case CalibrationState::COMPUTE_CALIBRATION:
		if (m_bComputingCalibration == false)
		{
			printf("Compute \n");
			tobiigaze_error_code error_code = TOBIIGAZE_ERROR_SUCCESS;
			ComputeCalibration(error_code, nullptr);
			m_bComputingCalibration = true;
		}
		break;
	case CalibrationState::FINISHED:
		m_iCurrentTargetPointIndex = 0;
		m_bComputingCalibration = false;
		m_bCalibratingPoint = false;
		m_bIsCalibrated = true;
		ETHOST->Flush(); //Flush and Reset after calibration (don't need the data)
		ETCalibrator::g_eCurrentState = CalibrationState::INACTIVE;
		break;
	}
}

//Private Functions
void ETCalibrator::StartCompleted(tobiigaze_error_code error_code, void *user_data)
{
	printf("Started calibration pocess \n");
	ETCalibrator::g_eCurrentState = CalibrationState::FOCUS_POINT;
}

void ETCalibrator::StopCompleted(tobiigaze_error_code error_code, void *user_data)
{
	if (error_code != TOBIIGAZE_ERROR_SUCCESS)
	{
		ETHOST->GetDebugger()->ReportError(error_code, "Could not stop the calibration process. Maybe calibration failed. Retry! \n");
		tobiigaze_calibration_stop_async(ETCalibrator::g_pAttachedTrackingDevice, StopCalibration, nullptr);
		return;
	}

	printf("Finished calibration pocess \n");
	ETCalibrator::g_eCurrentState = CalibrationState::FINISHED;
}

void ETCalibrator::CalibrationPoint(tobiigaze_error_code error_code, ETCalibrationPoint* point)
{
	if (error_code != TOBIIGAZE_ERROR_SUCCESS || point == nullptr)
	{
		ETHOST->GetDebugger()->ReportError(error_code, "Error Add Calibration Point \n");
		tobiigaze_calibration_stop_async(ETCalibrator::g_pAttachedTrackingDevice, StopCalibration, nullptr);
		return;
	}

	//Calibrate
	tobiigaze_point_2d p;
	p.x = point->GetPoint2D().X;
	p.y = point->GetPoint2D().Y;
	tobiigaze_calibration_add_point_async(g_pAttachedTrackingDevice, &p, CalibrationPointCompleted, nullptr);

	printf("Calibrating point... \n");
}

void ETCalibrator::CalibrationPointCompleted(tobiigaze_error_code error_code, void *user_data)
{
	if (error_code != TOBIIGAZE_ERROR_SUCCESS)
	{
		ETHOST->GetDebugger()->ReportError(error_code, "Could Not Complete Calibration Of Point. Retry! \n");
		tobiigaze_calibration_stop_async(ETCalibrator::g_pAttachedTrackingDevice, StopCalibration, nullptr);
		return;
	}

	//Switch state
	printf("Completed calibrating point \n");
	ETCalibrator::g_eCurrentState = CalibrationState::UNFOCUS_POINT;
}

void ETCalibrator::ComputeCalibration(tobiigaze_error_code error_code, void *user_data)
{
	if (error_code != TOBIIGAZE_ERROR_SUCCESS)
	{
		ETHOST->GetDebugger()->ReportError(error_code, "Error Computing Calibration \n");
		tobiigaze_calibration_stop_async(ETCalibrator::g_pAttachedTrackingDevice, StopCalibration, nullptr);
		return;
	}
	
	tobiigaze_calibration_compute_and_set_async(ETCalibrator::g_pAttachedTrackingDevice, ComputeCalibrationCompleted, nullptr);

	printf("Computing calibration... \n");
}

void ETCalibrator::ComputeCalibrationCompleted(tobiigaze_error_code error_code, void *user_data)
{
	if (error_code != TOBIIGAZE_ERROR_SUCCESS)
	{
		ETHOST->GetDebugger()->ReportError(error_code, "Computing Could Not Be Completed. Retry! \n");
		tobiigaze_calibration_stop_async(ETCalibrator::g_pAttachedTrackingDevice, StopCalibration, nullptr);
		return;
	}

	//Switch state
	printf("Completed computing calibration \n");
	//Stop the calibration proccess if succeeded
	tobiigaze_calibration_stop_async(ETCalibrator::g_pAttachedTrackingDevice, StopCalibration, nullptr);
}

void ETCalibrator::StopCalibration(tobiigaze_error_code error_code, void *user_data)
{
	//Switch state
	printf("Stopped calibration \n");
	ETCalibrator::g_eCurrentState = CalibrationState::FINISHED;
}

void ETCalibrator::Dummy(tobiigaze_error_code error_code, void *user_data)
{}