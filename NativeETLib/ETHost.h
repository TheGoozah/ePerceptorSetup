#pragma once

#include <thread>
#include <mutex>

#include "ETDebug.h"
#include "ETTypes.h"
#include "ETDataProvider.h"
#include "ETCalibrator.h"
#include "ETTimer.h"
#include "include/tobiigaze.h"
#include "include/tobiigaze_discovery.h"
#include "include/tobiigaze_callback_types.h"

//Define macro
#define ETHOST ETHost::GetInstance()
#define SAFEDELETE(pointer) if((pointer)) { delete (pointer); pointer = NULL; }

/*
* ETHost: singleton class that is the beating heart of the project
*/
class ETHost
{
public:
	//Destructor
	virtual ~ETHost();

	//Getters
	static ETHost* GetInstance();

	//Core Functions
	void BootETHost();
	void UpdateETHost();

	ETDebug* GetDebugger(){ return m_pDebugger; }
	double GetDeltaTime(){ if (g_pETTimer != nullptr) return g_pETTimer->GetDeltaTime(); else return 0.0; };

	//Data Streaming Functions
	void* GetAllData(int* size);
	void* GetDataStream(int* size);
	void Flush();

	//Calibration Functions
	void InvokeCalibration();
	bool GetCalibrationPointData(double* X, double* Y, double* Scale);
	bool IsCalibrated();

private:
	//Datamembers
	static ETHost* g_pETHostInstance; //Holds the one and only instance of the object ETHost
	static std::mutex g_p_mutex; //Mutex used to lock the object during initial creation
	static ETTimer* g_pETTimer; //Timer object that updates everytime the host is being called

	ETDebug* m_pDebugger; //Console window holder for debug logging
	ETDataProvider* m_pDataProvider; //Object responsable for retrieving and storing data from the eye tracker
	ETCalibrator* m_pCalibrator; //Object that is responsable for calibration process
	tobiigaze_eye_tracker* m_pEyeTracker; //Pointer to the tracker device

	DataReceiver m_fpActiveDataReceiver; //Function pointer containing the function that will trigger our active data provider

	bool m_bIsTracking; //Flag holding the tracking status
	
	//Constructor (private)
	ETHost();

	//Functions
	void CreateTobiiEyeXDevice(); //Creates the Tobii Device interface
	void DestroyTobiiEyeXDevice(); //Destroyes the Tobii Device interface
	void ConnectTobiiEyeX(); //Connects the hardware device to the Tobii Device interface
	void DisconnectTobiiEyeX(); //Disconnects the hardware device from the Tobii Device interface
	void StartTracking(); //Starts the device and passes the data to the connected provider
	void StopTracking(); //Stops the device
};