cmake_minimum_required(VERSION 3.5)

project(qtadqscope LANGUAGES CXX)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_BUILD_TYPE Debug)


option(MOCK_ADQAPI "Use a mock ADQ library" ON)
option(LINUX_BUILD "Build on Linux" OFF)
option(DEBUG_DMA_DELAY "Add delay to DMA transfers for overflow investigation" OFF)

if(USE_ASAN)
    set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=address -fno-omit-frame-pointer")
    set (CMAKE_LINKER_FLAGS_DEBUG "${CMAKE_LINKER_FLAGS_DEBUG} -fsanitize=address -fno-omit-frame-pointer")
    message(STATUS "AddressSanitizer enabled")
endif()

if(LINUX_BUILD)
    add_compile_definitions(LINUX)
    message(STATUS "Linux build")
endif()

if(DEBUG_DMA_DELAY)
    add_compile_definitions(DEBUG_DMA_DELAY=${DEBUG_DMA_DELAY})
    message(WARNING "Using delayed DMA transfers. This is only for forcing overflows!")
endif()
find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets REQUIRED)

if(${QT_VERSION_MAJOR} GREATER 4)
    find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets REQUIRED)
    find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets PrintSupport REQUIRED)
else()
    find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets REQUIRED)
endif()

add_subdirectory(lib/qcustomplot)
add_subdirectory(lib/spdlog-1.8.2)


set(PROJECT_SOURCES
        main.cpp
        BuffersDialog.cpp BuffersDialog.h BuffersDialog.ui
        RegisterDialog.cpp RegisterDialog.h RegisterDialog.ui
        FullCalibrationDialog.cpp FullCalibrationDialog.h FullCalibrationDialog.ui
        SpectrumDialog.h SpectrumDialog.cpp SpectrumDialog.ui
        AcquisitionChannelSettingsTab.cpp AcquisitionChannelSettingsTab.h AcquisitionChannelSettingsTab.ui
        AcquisitionSettings.cpp AcquisitionSettings.h AcquisitionSettings.ui
        PrimaryWindow.cpp PrimaryWindow.h PrimaryWindow.ui
        PrimaryControls.cpp PrimaryControls.h PrimaryControls.ui
        RecordProcessorsPanel.cpp RecordProcessorsPanel.h RecordProcessorsPanel.ui
        Semaphore.h
        RecordProcessor.h
        ScopeUpdater.cpp ScopeUpdater.h
        ApplicationContext.cpp ApplicationContext.h
        BinaryFileWriter.cpp BinaryFileWriter.h
        SignalParameterComputer.cpp SignalParameterComputer.h
        MockADQAPI.cpp MockADQAPI.h
        CalibrationTable.h CalibrationTable.cpp
        MinifiedRecordHeader.h
        ADQInterfaceWrappers.cpp ADQInterfaceWrappers.h
        Digitizer.cpp Digitizer.h
        DigitizerConstants.h
        Acquisition.cpp Acquisition.h
        util.h util.cpp
        GUIApplication.cpp GUIApplication.h
        DigitizerGUIComponent.h
        GUILogger.h
        MockADQAPIGen3Constants.h
        BufferProcessorThread.h BufferProcessorThread.cpp
        ADQAPIIncluder.h
        RegisterConstants.h
        SpectrumPlotter.h SpectrumPlotter.cpp
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


set(PROJECT_LIBRARIES
    Qt${QT_VERSION_MAJOR}::Widgets
    qcustomplot
    spdlog
)
if(${QT_VERSION_MAJOR} GREATER 4)
    list(APPEND PROJECT_LIBRARIES Qt5::PrintSupport)
endif()
if(MOCK_ADQAPI)
    message(WARNING "Using MockADQ API")
    add_compile_definitions(MOCK_ADQAPI)
else()
    message(STATUS "Using real libADQ API")
    list(APPEND PROJECT_LIBRARIES adq)
endif()
target_link_libraries(qtadqscope PUBLIC ${PROJECT_LIBRARIES})
