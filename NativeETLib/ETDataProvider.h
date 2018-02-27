#pragma once

#include "ETTypes.h"
#include "include/tobiigaze.h"
#include "include/tobiigaze_callback_types.h"
#include "include/tobiigaze_data_types.h"

#include <vector>
using namespace std;

/*
* ETDataProvider: container that recieves the data from the Tobii EyeX and stores it
*/
class ETDataProvider final
{
public:
	//Constructor & Destructor
	ETDataProvider();
	virtual ~ETDataProvider();

	//Functions
	void ConnectToHost(DataReceiver& dr); //Connects the provider to the host, that connects it to the device
	void DisconnectFromHost(DataReceiver& dr); //Disconnect the provider from the host, that disconnects it from the device
	void* GetAllData(int* pSize); //Return the recorded data (using pointer to avoided copies)
	void* GetDataStream(int* pSize); //Returns the stream of recorded data since last call
	void Flush();

private:
	//Datamembers
	static vector<ET_GazeData> m_vTotalBuffer; //Holding all the recorded data in memory
	static vector<ET_GazeData> m_vStream; //Holding the stream of recorded data since last call
	static bool m_bTransmittedStream; //Holds if our temporary stream was transfered or not

	//Callback Functions
	static void TOBIIGAZE_CALL OnGazeData(const struct tobiigaze_gaze_data* pGazedata, const struct tobiigaze_gaze_data_extensions* pExtensions, void *pUserData); //Function that is linked with the device
};