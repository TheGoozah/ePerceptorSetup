using System;
using System.IO;
using System.Runtime.InteropServices;
using System.Collections.Generic;
using System.Runtime.Serialization.Formatters.Binary;

//References
//Dispose pattern: https://msdn.microsoft.com/en-us/library/b1yfkh5e(v=vs.110).aspx

namespace ManagedETDll
{
    sealed public class ManagedTestObject : IDisposable
    {
        #region UnmanagedDll imports
        //CORE FUNCTIONS
        [DllImport("NativeETDll.dll", EntryPoint = "BootETHost", CallingConvention=CallingConvention.Cdecl)]
        protected static extern void _BootETHost();

        [DllImport("NativeETDll.dll", EntryPoint = "UpdateETHost", CallingConvention = CallingConvention.Cdecl)]
        protected static extern void _UpdateETHost();

        [DllImport("NativeETDll.dll", EntryPoint = "DeleteSingleton", CallingConvention = CallingConvention.Cdecl)]
        protected static extern void _DeleteSingleton();

        //DATA STREAMING FUNCTIONS
        [DllImport("NativeETDll.dll", EntryPoint = "GetAllData", CallingConvention = CallingConvention.Cdecl)]
        protected static extern IntPtr _GetAllData(out int arraySize);

        [DllImport("NativeETDll.dll", EntryPoint = "GetDataStream", CallingConvention = CallingConvention.Cdecl)]
        protected static extern IntPtr _GetDataStream(out int arraySize);

        [DllImport("NativeETDll.dll", EntryPoint = "Flush", CallingConvention = CallingConvention.Cdecl)]
        protected static extern void _Flush();

        //CALIBRATION FUNCTIONS
        [DllImport("NativeETDll.dll", EntryPoint = "InvokeCalibration", CallingConvention = CallingConvention.Cdecl)]
        protected static extern void _InvokeCalibration();

        [DllImport("NativeETDll.dll", EntryPoint = "GetCalibrationPointData", CallingConvention = CallingConvention.Cdecl)]
        protected static extern bool _GetCalibrationPointData(out double x, out double y, out double scale);

        [DllImport("NativeETDll.dll", EntryPoint = "IsCalibrated", CallingConvention = CallingConvention.Cdecl)]
        protected static extern bool _IsCalibrated();
        #endregion

        //FIELDS
        private ET_DataStream _dataStream;
        private bool _bDisposed = false;

        //CONSTRUCTOR
        public ManagedTestObject() //Constructor
        {
            _BootETHost();
        }

        //DESTRUCTOR
        ~ManagedTestObject() //Finalize
        {
            Dispose(false);
        }

        //DISPOSERS
        public void Dispose()
        {
            Dispose(true);
            GC.SuppressFinalize(this);
        }

        public void Dispose(bool disposing) //Destroy
        {
            //Execute if resource have not already been disposed
            if (!_bDisposed)
            {
                //If the call is from Dispose, free managed resources
                if (disposing)
                {
                    //NO MANAGED RESOURCES
                    DetachStream();
                }
                //Free unmanaged resources
                _DeleteSingleton();
            }
            _bDisposed = true;
        }

        //CORE METHODS
        public void Update()
        {
            _UpdateETHost();
        }

        //DATA STREAMING METHODS
        public void AttachStream(ref ET_DataStream stream)
        {
            _dataStream = stream;
        }

        public void DetachStream()
        {
            _dataStream = null;
        }

        public void FlushStream()
        {
            if (_dataStream == null)
                return;

            _dataStream._dataStream.Clear();
            _Flush();
        }

        public void PullToStream()
        {
            int arraySize;
            IntPtr arrayPointer = _GetDataStream(out arraySize);

            //TODO: temporary flush untill the HD streaming is implemented (performance)
            if((_dataStream._dataStream.Count + arraySize) > _dataStream._storageLimit)
                FlushStream();

            if (arrayPointer != IntPtr.Zero && arraySize > 0)
            {
                for (int i = 0; i < arraySize; ++i)
                {
                    object data = new ET_GazeData();
                    IntPtr ptr = new IntPtr(arrayPointer.ToInt64() + (i * Marshal.SizeOf(data)));
                    if (data != null)
                        Marshal.PtrToStructure(ptr, data);
                    _dataStream._dataStream.Add((ET_GazeData)data);
                }
            }
        }

        public List<ET_GazeData> GetAllData()
        {
            List<ET_GazeData> totaldataList = new List<ET_GazeData>();

            int arraySize;
            IntPtr arrayPointer = _GetAllData(out arraySize);

            if (arrayPointer != IntPtr.Zero && arraySize > 0)
            {
                for (int i = 0; i < arraySize; ++i)
                {
                    object data = new ET_GazeData();
                    IntPtr ptr = new IntPtr(arrayPointer.ToInt64() + (i*Marshal.SizeOf(data)));
                    if (data != null)
                        Marshal.PtrToStructure(ptr, data);
                    totaldataList.Add((ET_GazeData)data);
                }
            }

            return totaldataList;
        }

        //CALIBRATION METHODS
        public void InvokeCalibration()
        {
            _InvokeCalibration();
        }

        public bool GetCalibrationPointData(out double x, out double y, out double scale)
        {
            return _GetCalibrationPointData(out x, out y, out scale);
        }

        public bool IsCalibrated()
        {
            return _IsCalibrated();
        }
    }
}
