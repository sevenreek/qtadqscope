# qtadqscope

<!-- ABOUT THE PROJECT -->
## About The Project

This repository houses the Qt5 Graphical User Interface for controlling an ADQ14 high speed digitizer. The application can operate as a simple osciloscope and/or save the acquired data to files. It enables various acquisition modes and user logic control.

The application is developed together with the firmware and often provides experimental features.

<!-- GETTING STARTED -->
## Prerequisites

To build the application the following software must be installed:
+ [cmake3](https://cmake.org/download/) in version at least 3.5
+ a C++ compiler supporting C++14
+ the [Qt5 framework](https://www.qt.io/download-qt-installer) (v5.9.9 recommended)
+ ADQAPI library and ADQ14 drivers in version r68178

The table below lists the configurations which have successfully been used to build the project from the source code. Other setups consisting of versions between those listed in the table will probably work, however they might produce more warnings during the building phase.
| OS                   | Compiler        | Qt     | CMake  |
|----------------------|-----------------|--------|--------|
| RHEL 7.4             | GCC 4.8.5       | 5.6.2  | 3.17.5 |
| Windows 10           | MSVC2017 64 bit | 5.9.9  | 3.21.3 |
| Ubuntu 20.04.3 (WSL) | g++ 9.3.0       | 5.12.8 | 3.16.3 |

### External dependencies
The code also uses the following external dependencies: 
* [qcustomplot 2.0.1](https://www.qcustomplot.com/)
* [spdlog 1.8.2](https://github.com/gabime/spdlog)

For ease of use, these libraries are included with the source code and do not have to be downloaded separately.

### Installing ADQAPI
For detailed instructions for specific operating systems refer to the `README` file located in the `ADQ_SDK_linux_r68178` directory. 

For Windows systems the drivers and library can be installed using the provided `TSPD-SDK-installer.exe`. 

For Linux systems a manual installation of an appropriate package from `ADQ_SDK_linux_r687178/packages` must be performed. For example, for RHEL 7 the four `.rpm` packages must be installed in the following order:

1. `rpm -U dkms-2.2.0.3-28.git.7c3e7c5.el7.noarch`
1. `rpm -U spd-adq-pci-dkms-1.21-1.noarch`
1. `rpm -U libadq0-0.68178-9.1.x86_64`
1. `rpm -U adqtools-0.68181-4.1.x86_64`

The DKMS is likely to be already installed on the operating system. No issues have been found with newer versions, primarily with `dkms-2.8.6`.

## Building the project

1. **Create a `build` directory in the root dir**

   ```sh
   mkdir build
   ```

2. **`cd` into the directory**

   ```sh
   cd build
   ```

3. **Configure CMake**

   ```sh
   cmake3 -DMOCK_ADQAPI=OFF -DLINUX_BUILD=ON ..
   ```
   The program can be used in debug mode on systems without the ADQAPI or the board plugged in. To indicate a production build intended to be used on a real setup unset `MOCK_ADQAPI`. Due to small differences between Linux and Windows systems the `LINUX_BUILD` setting must also be chosen appropriately. Additionaly, for Windows and possibly other systems the cmake command is going to be just `cmake` instead of `cmake3`.

4. **Build the project**

   ```sh
   cmake3 --build .
   ```

The last two commands can be executed using the `bld.sh` script found in the `tools` directory.

## Running the project
The last step of the build process should produce a correct executable `qtadqscope` (or `qtadqscope.exe`). Run it to start the application in the GUI mode.
### Settings
By default the application tries to load its settings using from a file called `default_config.json` on start. If such file does not exist it can be created by simply saving any configuration from the GUI app with that name. Any configuration file can also be loaded using the appropriate GUI context menu option or alternatively provided to the application as the first command line argument:
   ```sh
   ./qtadqscope some_custom_config.json
   ```
### Command line mode
For automatic scripts that might want to use the custom functionality provided by the software package an experimental headless command line mode is available. To load a configuration and immediately start an acquisition based on the provided file provide the `-c` option when starting the app together with a configuration file:
   ```sh
   ./qtadqscope -c some_custom_config.json
   ```
This will cause the app to run solely in the terminal. As soon as the configuration is done an acquisition starts and the application exits automatically after it is finished. The feature is experimental and should be used with caution. For example, if the acquisiton duration is not specified in the config file the acquisition will not terminate and thus the application will not return.

## Logging
The application generates a total of three different logs. Two log files are generated by the ADQAPI in the current working directory:
- `spd_adqcontrolunit_trace.log` - with initial data before control is given to the specific board
- `spd_device_t714_pcie_#######_trace.log` - with information more specific to the board model

Another log stream is generated by the custom functionality and the `spdlog` library. This log is displayed in the terminal and the messages window visible in the main GUI view. This log corresponds to the results of the various ADQAPI operations and is probably the most useful one, however the detailed, direct cause of some errors can often be found only in the file logs.




