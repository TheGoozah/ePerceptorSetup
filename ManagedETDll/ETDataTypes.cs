using System;
using System.Collections.Generic;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using System.Text;

namespace ManagedETDll
{
    //References
    //http://www.developerfusion.com/article/84519/mastering-structs-in-c/
    //https://msdn.microsoft.com/en-us/library/system.runtime.interopservices.structlayoutattribute%28v=vs.71%29.aspx

    //Make sure to pack to variables from big to small and set the Pack attribute to the biggest value in the struct (eg: double -> Pack = 8) 
    //A bool has to be marshalled as I1
    [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Unicode, Pack = 8)]
    public class ET_Point_3D
    {
        [MarshalAs(UnmanagedType.R8)] public double X;
        [MarshalAs(UnmanagedType.R8)] public double Y;
        [MarshalAs(UnmanagedType.R8)] public double Z;
    };

    [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Unicode, Pack = 4)]
    public class ET_Point_3D_F
    {
        [MarshalAs(UnmanagedType.R4)] public float X;
        [MarshalAs(UnmanagedType.R4)] public float Y;
        [MarshalAs(UnmanagedType.R4)] public float Z;
    };

    [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Unicode, Pack = 8)]
    public class ET_Point_2D
    {
        [MarshalAs(UnmanagedType.R8)] public double X;
        [MarshalAs(UnmanagedType.R8)] public double Y;
    };

    [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Unicode, Pack = 4)]
    public class ET_Point_2D_F
    {
        [MarshalAs(UnmanagedType.R4)] public float X;
        [MarshalAs(UnmanagedType.R4)] public float Y;
    };

    [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Unicode, Pack = 8)]
    public class ET_GazeDataEye
    {
        public ET_Point_3D eye_position_from_eye_tracker_mm;
        public ET_Point_3D eye_position_in_track_box_normalized;
        public ET_Point_3D gaze_point_from_eye_tracker_mm;
        public ET_Point_2D gaze_point_on_display_normalized;
    };

    [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Unicode, Pack = 8)]
    public class ET_GazeData
    {
        public ET_GazeDataEye left;
        public ET_GazeDataEye right;
        [MarshalAs(UnmanagedType.U8)] public UInt64 timestamp;
        [MarshalAs(UnmanagedType.I4)] public ET_Tracking_Status tracking_status;
    };

    public enum ET_Tracking_Status
    {
        ETGAZE_TRACKING_STATUS_NO_EYES_TRACKED = 0,
        ETGAZE_TRACKING_STATUS_BOTH_EYES_TRACKED = 1,
        ETGAZE_TRACKING_STATUS_ONLY_LEFT_EYE_TRACKED = 2,
        ETGAZE_TRACKING_STATUS_ONE_EYE_TRACKED_PROBABLY_LEFT = 3,
        ETGAZE_TRACKING_STATUS_ONE_LEFT_TRACKED_UNKNOWN_WHICH = 4,
        ETGAZE_TRACKING_STATUS_ONE_EYE_TRACKED_PROBABLY_RIGHT = 5,
        ETGAZE_TRACKING_STATUS_ONLY_RIGHT_EYE_TRACKED = 6
    }

    [assembly: InternalsVisibleTo("ETDataProvider")]
    public class ET_DataStream
    {
        //FIELDS
        internal List<ET_GazeData> _dataStream;
        internal int _storageLimit = 500;

        //PROPERTIES
        public List<ET_GazeData> DataStream 
        {
            get { return _dataStream; }
        }

        //CONSTRUCTOR
        public ET_DataStream()
        {
            _dataStream = new List<ET_GazeData>(_storageLimit);
        }
    }
}
