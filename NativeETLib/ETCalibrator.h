#pragma once

#include "ETTypes.h"
#include "ETCalibrationPoint.h"
#include "include/tobiigaze.h"
#include "include/tobiigaze_callback_types.h"
#include "include/tobiigaze_data_types.h"
#include "include/tobiigaze_calibration.h"

#include <vector>
using namespace std;

enum CalibrationState
{
	INACTIVE = 0,
	INTIALIZE = 1,
	POSITIONING = 2,
	FETCH_POINT = 3,
	MOVE_POINT = 4,
	FOCUS_POINT = 5,
	UNFOCUS_POINT = 6,
	CALIBRATE_POINT = 7,
	COMPUTE_CALIBRATION = 8,
	FINISHED = 9
};

/*
* ETCalibrator: contains the calibration points and controls the calibration process
*/
class ETCalibrator final
{
public:
	//Constructor & Destructor
	ETCalibrator(tobiigaze_eye_tracker* pTrackingDevice);
	~ETCalibrator();

	//Functions
	void StartCalibration();
	void Update();

	//Getters & Setters
	ETCalibrationPoint* ReturnActiveCalibrationPoint(){ return m_pCurrentCalibrationPoint; }
	bool IsCalibrated(){ return m_bIsCalibrated; }

private:
	//Datamembers
	std::vector<ETCalibrationPoint*> m_vpCalibrationPoints;
	ETCalibrationPoint* m_pCurrentCalibrationPoint;
	int m_iCurrentTargetPointIndex = 0;
	bool m_bPositioningRequired;
	bool m_bIsCalibrated;
	bool m_bCalibratingPoint;
	bool m_bComputingCalibration;

	//Static Datamembers
	static tobiigaze_eye_tracker* g_pAttachedTrackingDevice;
	static CalibrationState g_eCurrentState;

	//Callback Functions
	static void TOBIIGAZE_CALL StartCompleted(tobiigaze_error_code error_code, void *pUserData);
	static void TOBIIGAZE_CALL StopCompleted(tobiigaze_error_code error_code, void *pUserData);
	static void TOBIIGAZE_CALL CalibrationPoint(tobiigaze_error_code error_code, ETCalibrationPoint* pPoint);
	static void TOBIIGAZE_CALL CalibrationPointCompleted(tobiigaze_error_code error_code, void *pUserData);
	static void TOBIIGAZE_CALL ComputeCalibration(tobiigaze_error_code error_code, void *pUserData);
	static void TOBIIGAZE_CALL ComputeCalibrationCompleted(tobiigaze_error_code error_code, void *pUserData);
	static void TOBIIGAZE_CALL StopCalibration(tobiigaze_error_code error_code, void *pUserData);
	static void TOBIIGAZE_CALL Dummy(tobiigaze_error_code error_code, void *pUserData);
};