/*
	Implementation of the ETHost functionality
*/
#pragma once

#include "stdafx.h"
#include "ETHost.h"

//Declare statics
ETHost* ETHost::g_pETHostInstance = nullptr;
std::mutex ETHost::g_p_mutex;
ETTimer* ETHost::g_pETTimer = nullptr;

//Constructor
ETHost::ETHost():
m_pEyeTracker(nullptr),
m_pDebugger(nullptr),
m_pDataProvider(nullptr),
m_pCalibrator(nullptr),
m_bIsTracking(false),
m_fpActiveDataReceiver(nullptr)
{}

//Destructor
ETHost::~ETHost()
{
	//TODO: put rollback in function
	StopTracking();
	m_pDataProvider->DisconnectFromHost(this->m_fpActiveDataReceiver); //Disconnect our dataprovider
	DisconnectTobiiEyeX();
	DestroyTobiiEyeXDevice();

	//Safe delete the acquired resources
	SAFEDELETE(m_pCalibrator);
	SAFEDELETE(m_pDataProvider);
	SAFEDELETE(m_pDebugger);

	//Finally set the Singleton pointer to nullptr to "free" the address for second allocation. The deletion is called from the outside (see NativeETDLL)
	ETHost::g_pETHostInstance = nullptr;
}

//Public Functions
ETHost* ETHost::GetInstance()
{
	//First check if we have an instance, else we need to create one while locking for thread safety 
	if (g_pETHostInstance == nullptr)
	{
		//Lock using a mutex, mutex will be released when lock goes out of scope
		std::lock_guard<std::mutex> lock(g_p_mutex);
		if (g_pETHostInstance == nullptr)
		{
			g_pETHostInstance = new ETHost();
			g_pETTimer = new ETTimer();
			printf("Created host and timer");
		}
	}

	//printf("Time elapsed: %d, Delta time: %f \n", g_pETTimer->GetElapsedTimeInSeconds(), g_pETTimer->GetDeltaTime());
	return g_pETHostInstance;
}

//Core Functions
void ETHost::BootETHost()
{
	/*If we are in Debug mode create a console window*/
#ifdef _DEBUG
	m_pDebugger = new ETDebug();
#endif

	//Search and connect tobii eyex (if there is a device connected via USB)
	CreateTobiiEyeXDevice();
	ConnectTobiiEyeX();

	//Create the DataProvider and link it as our active provider
	m_pDataProvider = new ETDataProvider();
	m_pDataProvider->ConnectToHost(this->m_fpActiveDataReceiver);

	//Create the calibrator so user can calibrate eye tracker
	m_pCalibrator = new ETCalibrator(this->m_pEyeTracker);

	//Start tracking (TODO: for now, this should become a seperate call)
	StartTracking();
}

void ETHost::UpdateETHost()
{
	//Update the timer before returning the instance of the host
	g_pETTimer->Update();

	if (m_pCalibrator != nullptr)
		m_pCalibrator->Update();
}

//Data Streaming Functions
void* ETHost::GetAllData(int* size)
{
	if (m_pDataProvider != nullptr)
		return m_pDataProvider->GetAllData(size);
	return nullptr;
}

void* ETHost::GetDataStream(int* size)
{
	if (m_pDataProvider != nullptr)
		return m_pDataProvider->GetDataStream(size);
	return nullptr;
}

void ETHost::Flush()
{
	if (m_pDataProvider != nullptr)
		return m_pDataProvider->Flush();
}

//Calibration Functions
void ETHost::InvokeCalibration()
{
	if (m_pCalibrator != nullptr)
		m_pCalibrator->StartCalibration();
}

bool ETHost::GetCalibrationPointData(double* X, double* Y, double* Scale)
{
	if (this->m_pCalibrator != nullptr && this->m_pCalibrator->ReturnActiveCalibrationPoint() != nullptr)
	{
		*X = this->m_pCalibrator->ReturnActiveCalibrationPoint()->GetPoint2D().X;
		*Y = this->m_pCalibrator->ReturnActiveCalibrationPoint()->GetPoint2D().Y;
		*Scale = this->m_pCalibrator->ReturnActiveCalibrationPoint()->GetScale();
		return true;
	}
	else
	{
		*X = 0;
		*Y = 0;
		*Scale = 0;
		return false;
	}
}

bool ETHost::IsCalibrated()
{
	if (m_pCalibrator == nullptr)
		return false;

	return m_pCalibrator->IsCalibrated();
}

//Private Functions
void ETHost::CreateTobiiEyeXDevice()
{
	char url[256];
	tobiigaze_error_code error_code;
	tobiigaze_get_connected_eye_tracker(url, 256, &error_code);

	if (error_code != TOBIIGAZE_ERROR_SUCCESS)
	{
		m_pDebugger->ReportError(error_code, "tobiigaze_usb_info");
		return;
	}

	//Create eye-tracker
	m_pEyeTracker = tobiigaze_create(url, &error_code);
	if (error_code != TOBIIGAZE_ERROR_SUCCESS)
	{
		m_pDebugger->ReportError(error_code, "tobiigaze_create");
		return;
	}
	else
		printf("Eye Tracker created. \n");
}

void ETHost::DestroyTobiiEyeXDevice()
{
	if (m_pEyeTracker == nullptr)
		return;

	tobiigaze_destroy(m_pEyeTracker);
	m_pEyeTracker = nullptr;
}

void ETHost::ConnectTobiiEyeX()
{
	if (m_pEyeTracker == nullptr)
		return;

	//Start loop (MUST be done before connecting)
	tobiigaze_run_event_loop_on_internal_thread(m_pEyeTracker, 0, 0);

	//Connect to the tracker
	tobiigaze_error_code error_code;
	tobiigaze_connect(m_pEyeTracker, &error_code);
	if (error_code != TOBIIGAZE_ERROR_SUCCESS)
	{
		m_pDebugger->ReportError(error_code, "tobiigaze_connect");
		return;
	}
	else
		printf("Eye Tracker connected. \n");
}

void ETHost::DisconnectTobiiEyeX()
{
	if(m_pEyeTracker == nullptr)
		return;

	//Disconnect and break the event loop
	tobiigaze_disconnect(m_pEyeTracker);
	tobiigaze_break_event_loop(m_pEyeTracker);
}

void ETHost::StartTracking()
{
	if (m_fpActiveDataReceiver == nullptr)
		printf("No active dataReceiver \n");

	if (m_pEyeTracker == nullptr || m_fpActiveDataReceiver == nullptr)
		return;

	tobiigaze_error_code error_code;
	tobiigaze_start_tracking(m_pEyeTracker, this->m_fpActiveDataReceiver, &error_code, 0);
	if (error_code != TOBIIGAZE_ERROR_SUCCESS)
	{
		m_pDebugger->ReportError(error_code, "tobiigaze_tracking");
		return;
	}
	else
		printf("Eye Tracker started tracking. \n");
	m_bIsTracking = true;
}

void ETHost::StopTracking()
{
	if (m_pEyeTracker == nullptr)
		return;

	tobiigaze_error_code error_code;
	tobiigaze_stop_tracking(m_pEyeTracker, &error_code);
	if (error_code != TOBIIGAZE_ERROR_SUCCESS)
	{
		m_pDebugger->ReportError(error_code, "tobiigaze_tracking");
		return;
	}
	else
		printf("Eye Tracker stopped tracking. \n");
	m_bIsTracking = false;
}