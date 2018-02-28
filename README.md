# ePerceptorSetup
Initial setup of the ePerceptor prototype. Every file the is required is present (Tobii library files).
This framework/prototype could be used in **Unity3D** or in a **C++ application**. All you need is a **Tobii EyeX device**.
Below is an overview (with direct links) to the interesting files.

## Interesting files
- [ET Host](NativeETLib/ETHost.cpp#L149): the core of the prototype. It it used in applications to initialize and to retrieve information from the device.
- [ET DataProvider](NativeETLib/ETDataProvider.cpp#L42): the class capturing all data and storing it in a buffer.
- [ET Calibrator](NativeETLib/ETCalibrator.cpp#L59): the class that is responsable for calibrating the device using device specific event flags.The calibration process (type of grid, etc.) is defined by the user in the application itself.
- [NativeETDll](NativeETDll/NativeETDll.cpp): public functions calling the internal framework, which is also getting controlled by the dynamic library. The managed library uses this dynamic library.
- [ETWrapper](ManagedETDll/ETWrapper.cs): a C# wrapper that uses the dynamic library. This wrapper is responsable for calling the library functions as well as Marshalling all the data based on it's type.
