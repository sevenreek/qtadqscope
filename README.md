# qtadqscope
A Qt5 interface for controlling an ADQ14 high speed digitizer. Can operate as a simple osciloscope and/or save the acquired data to files. Enables various acquisition modes and user logic control.

# Dependencies
Included with in the source code:
*nlohmann_json 
*qcustomplot
*spdlog-1.8.2
To use the application ADQAPI must be installed in the system. For testing purposes MOCK_ADQAPI can be set to ON to avoid this requirement.
# Build
The project relies on CMake 3.5+. A build script for building on a Linux machine with the real ADQAPI installed in the tools directory. Run bld.sh to execute cmake build.
