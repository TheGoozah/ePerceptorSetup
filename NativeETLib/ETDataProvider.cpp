/*
Implementation of the ETDataProvider functionality
*/
#pragma once

#include "stdafx.h"
#include "ETDataProvider.h"
#include "ETDebug.h"
#include <iostream>

//Declare statics
vector<ET_GazeData> ETDataProvider::m_vTotalBuffer;
vector<ET_GazeData> ETDataProvider::m_vStream;
bool ETDataProvider::m_bTransmittedStream;

//Constructor
ETDataProvider::ETDataProvider()
{}

//Destructor
ETDataProvider::~ETDataProvider()
{}

//Public Functions
void ETDataProvider::ConnectToHost(DataReceiver& dr)
{
	if (dr == nullptr)
		dr = this->OnGazeData;
	else
		printf("Cannot connect Provider to Host because there is allready an active Provider at: %#x \n", dr);
}

void ETDataProvider::DisconnectFromHost(DataReceiver& dr)
{
	if (dr != nullptr)
		dr = nullptr;
	else
		printf("Cannot disconnect Provider from Host because there is no active Provider \n");
}

//Private Functions
void ETDataProvider::OnGazeData(const struct tobiigaze_gaze_data* gazedata, const struct tobiigaze_gaze_data_extensions* extensions, void *user_data)
{
	//Check if one or both eyes is being tracked, and if so store the data
	if (gazedata->tracking_status == TOBIIGAZE_TRACKING_STATUS_BOTH_EYES_TRACKED ||
		gazedata->tracking_status == TOBIIGAZE_TRACKING_STATUS_ONLY_LEFT_EYE_TRACKED ||
		gazedata->tracking_status == TOBIIGAZE_TRACKING_STATUS_ONLY_RIGHT_EYE_TRACKED ||
		gazedata->tracking_status == TOBIIGAZE_TRACKING_STATUS_ONE_EYE_TRACKED_PROBABLY_LEFT ||
		gazedata->tracking_status == TOBIIGAZE_TRACKING_STATUS_ONE_EYE_TRACKED_PROBABLY_RIGHT)
	{
		//Store the data
		ET_GazeData gd;
		gd.timestamp = (long long)gazedata->timestamp;
		gd.tracking_status = (ET_Tracking_Status)gazedata->tracking_status;

		//Left
		ET_Point_3D p1;
		p1.X = gazedata->left.eye_position_from_eye_tracker_mm.x;
		p1.Y = gazedata->left.eye_position_from_eye_tracker_mm.y;
		p1.Z = gazedata->left.eye_position_from_eye_tracker_mm.z;
		ET_Point_3D p2;
		p2.X = gazedata->left.eye_position_in_track_box_normalized.x;
		p2.Y = gazedata->left.eye_position_in_track_box_normalized.y;
		p2.Z = gazedata->left.eye_position_in_track_box_normalized.z;
		ET_Point_3D p3;
		p3.X = gazedata->left.gaze_point_from_eye_tracker_mm.x;
		p3.Y = gazedata->left.gaze_point_from_eye_tracker_mm.y;
		p3.Z = gazedata->left.gaze_point_from_eye_tracker_mm.z;
		ET_Point_2D p4;
		p4.X = gazedata->left.gaze_point_on_display_normalized.x;
		p4.Y = gazedata->left.gaze_point_on_display_normalized.y;
		ET_GazeDataEye left;
		left.eye_position_from_eye_tracker_mm = p1;
		left.eye_position_in_track_box_normalized = p2;
		left.gaze_point_from_eye_tracker_mm = p3;
		left.gaze_point_on_display_normalized = p4;
		gd.left = left;

		//Right
		ET_Point_3D p5;
		p5.X = gazedata->right.eye_position_from_eye_tracker_mm.x;
		p5.Y = gazedata->right.eye_position_from_eye_tracker_mm.y;
		p5.Z = gazedata->right.eye_position_from_eye_tracker_mm.z;
		ET_Point_3D p6;
		p6.X = gazedata->right.eye_position_in_track_box_normalized.x;
		p6.Y = gazedata->right.eye_position_in_track_box_normalized.y;
		p6.Z = gazedata->right.eye_position_in_track_box_normalized.z;
		ET_Point_3D p7;
		p7.X = gazedata->right.gaze_point_from_eye_tracker_mm.x;
		p7.Y = gazedata->right.gaze_point_from_eye_tracker_mm.y;
		p7.Z = gazedata->right.gaze_point_from_eye_tracker_mm.z;
		ET_Point_2D p8;
		p8.X = gazedata->right.gaze_point_on_display_normalized.x;
		p8.Y = gazedata->right.gaze_point_on_display_normalized.y;
		ET_GazeDataEye right;
		right.eye_position_from_eye_tracker_mm = p5;
		right.eye_position_in_track_box_normalized = p6;
		right.gaze_point_from_eye_tracker_mm = p7;
		right.gaze_point_on_display_normalized = p8;
		gd.right = right;

		//If flagged as retreived, clear stream
		if (m_bTransmittedStream)
		{
			m_vStream.clear();
			m_bTransmittedStream = false;
		}

		//Store in buffers
		//TODO: make sure the total buffer cannot overflow
		m_vTotalBuffer.push_back(gd);
		m_vStream.push_back(gd);

		//printf("C++ Time: %f, x: %f, y: %f \n", gd.timestamp, gd.right.gaze_point_on_display_normalized.X, gd.right.gaze_point_on_display_normalized.Y);
	}
}

void* ETDataProvider::GetAllData(int* size)
{
	//Pass the size
	*size = m_vTotalBuffer.size();

	//Check if data
	if (*size <= 0)
		return nullptr;

	//Fill the container with recorded data
	return &m_vTotalBuffer[0];
}

void* ETDataProvider::GetDataStream(int* size)
{
	//Pass the size
	*size = m_vStream.size();

	//Check if data
	if (*size <= 0)
		return nullptr;

	//Flag transmittion
	m_bTransmittedStream = true;

	//Fill the container with recorded data
	return &m_vStream[0];
}

void ETDataProvider::Flush()
{
	m_vTotalBuffer.clear();
	m_vStream.clear();
}