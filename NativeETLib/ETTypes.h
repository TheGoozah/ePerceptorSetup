#pragma once

//Define types
typedef void(*DataReceiver)(const struct tobiigaze_gaze_data*, const struct tobiigaze_gaze_data_extensions*, void*);

struct ET_Point_3D
{
	double X;
	double Y;
	double Z;
};

struct ET_Point_2D
{
	double X;
	double Y;
};

struct ET_GazeDataEye
{
	ET_Point_3D eye_position_from_eye_tracker_mm;
	ET_Point_3D eye_position_in_track_box_normalized;
	ET_Point_3D gaze_point_from_eye_tracker_mm;
	ET_Point_2D gaze_point_on_display_normalized;
};

enum ET_Tracking_Status
{
	ETGAZE_TRACKING_STATUS_NO_EYES_TRACKED = 0,
	ETGAZE_TRACKING_STATUS_BOTH_EYES_TRACKED = 1,
	ETGAZE_TRACKING_STATUS_ONLY_LEFT_EYE_TRACKED = 2,
	ETGAZE_TRACKING_STATUS_ONE_EYE_TRACKED_PROBABLY_LEFT = 3,
	ETGAZE_TRACKING_STATUS_ONE_LEFT_TRACKED_UNKNOWN_WHICH = 4,
	ETGAZE_TRACKING_STATUS_ONE_EYE_TRACKED_PROBABLY_RIGHT = 5,
	ETGAZE_TRACKING_STATUS_ONLY_RIGHT_EYE_TRACKED = 6
};

struct ET_GazeData
{
	ET_GazeDataEye left;
	ET_GazeDataEye right;
	long long timestamp;
	ET_Tracking_Status tracking_status;
};
