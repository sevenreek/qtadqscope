cmake_minimum_required(VERSION 3.5)

project(qtadqscope LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_BUILD_TYPE Debug)




option(MOCK_ADQAPI "Use a mock ADQ library" ON)
option(LINUX_BUILD "Build on Linux" OFF)


if(LINUX_BUILD)
    add_compile_definitions(LINUX)
    message(STATUS "Linux build")
endif()

# QtCreator supports the following variables for Android, which are identical to qmake Android variables.
# Check https://doc.qt.io/qt/deployment-android.html for more information.
# They need to be set before the find_package( ...) calls below.

#if(ANDROID)
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
#    if (ANDROID_ABI STREQUAL "armeabi-v7a")
#        set(ANDROID_EXTRA_LIBS
#            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libcrypto.so
#            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libssl.so)
#    endif()
#endif()

find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets REQUIRED)

if(${QT_VERSION_MAJOR} GREATER 4)
    find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets REQUIRED)
    find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets PrintSupport REQUIRED)
else()
    find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets REQUIRED)
endif()

add_subdirectory(include/spdlog-1.8.2)
include_directories(include/nlohmann_json)
include_directories(include/qcustomplot)

set(JSON_BuildTests OFF CACHE INTERNAL "")

set(PROJECT_SOURCES
        main.cpp
        ADQDeviceConfiguration.cpp
        ADQDeviceConfiguration.h
        MainWindow.cpp
        MainWindow.h
        MainWindow.ui
        include/qcustomplot/qcustomplot.cpp
        include/qcustomplot/qcustomplot.h
        BufferProcessor.cpp
        BufferProcessor.h
        Semaphore.h
        StreamingBuffers.cpp
        StreamingBuffers.h
        StreamingHeader.h
        RecordProcessor.h
        Application.cpp
        Application.h
        ScopeUpdater.cpp
        ScopeUpdater.h
        ApplicationConfiguration.cpp
        ApplicationConfiguration.h
        AcquisitionThreads.cpp
        AcquisitionThreads.h
        BinaryFileWriter.cpp
        BinaryFileWriter.h
        BuffersDialog.cpp
        BuffersDialog.h
        BuffersDialog.ui
        RegisterDialog.cpp
        RegisterDialog.h
        RegisterDialog.ui
        SignalParameterComputer.cpp
        SignalParameterComputer.h
        MockADQAPI.cpp
        MockADQAPI.h
        FullCalibrationDialog.cpp
        FullCalibrationDialog.h
        FullCalibrationDialog.ui
        CalibrationTable.h
        CalibrationTable.cpp
        MinifiedRecordHeader.h
        QADQWrapper.cpp
        QADQWrapper.h
        Digitizer.cpp
        Digitizer.h
        DigitizerConstants.h
        DigitizerConfiguration.cpp
        DigitizerConfiguration.h
        util.h
        )

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(qtadqscope
        ${PROJECT_SOURCES}
    )
else()
    add_executable(qtadqscope
        ${PROJECT_SOURCES}
    )
endif()

if(MOCK_ADQAPI)
    message(WARNING "Using MockADQ API")
    add_compile_definitions(MOCK_ADQAPI)
    if(${QT_VERSION_MAJOR} GREATER 4)
        set(PROJECT_LIBRARIES
            Qt${QT_VERSION_MAJOR}::Widgets
            Qt5::PrintSupport
            spdlog
        )
    else()
        set(PROJECT_LIBRARIES
            Qt${QT_VERSION_MAJOR}::Widgets
            spdlog
        )
    endif()
else()
    message(STATUS "Using real libADQ API")
    if(${QT_VERSION_MAJOR} GREATER 4)
        set(PROJECT_LIBRARIES
            Qt${QT_VERSION_MAJOR}::Widgets
            Qt5::PrintSupport
            spdlog
            adq
        )
    else()
        set(PROJECT_LIBRARIES
            Qt${QT_VERSION_MAJOR}::Widgets
            spdlog
            adq
        )
    endif()
endif()
target_link_libraries(qtadqscope PRIVATE ${PROJECT_LIBRARIES})
