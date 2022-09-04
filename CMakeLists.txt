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

option(IGNORE_UNSAFE_FUNCTIONS "Ignore warning about unsafe functions like scanf" ON)

if(IGNORE_UNSAFE_FUNCTIONS)
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
endif()


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
        ScrollbarScalingQScrollArea.h ScrollbarScalingQScrollArea.cpp
        BuffersDialog.cpp BuffersDialog.h BuffersDialog.ui
        SpectroscopeTab.h SpectroscopeTab.cpp SpectroscopeTab.ui
        AcquisitionChannelSettingsTab.cpp AcquisitionChannelSettingsTab.h AcquisitionChannelSettingsTab.ui
        AcquisitionSettingsSidePanel.cpp AcquisitionSettingsSidePanel.h AcquisitionSettingsSidePanel.ui
        PrimaryWindow.cpp PrimaryWindow.h PrimaryWindow.ui
        PrimaryControls.cpp PrimaryControls.h PrimaryControls.ui
        FileSaveSettingsPanel.h FileSaveSettingsPanel.cpp FileSaveSettingsPanel.ui
        ScopeUpdateSettingsPanel.h ScopeUpdateSettingsPanel.cpp ScopeUpdateSettingsPanel.ui
        Semaphore.h
        RecordProcessor.h
        ScopeUpdater.cpp ScopeUpdater.h
        ApplicationConfiguration.h ApplicationConfiguration.cpp
        ApplicationContext.cpp ApplicationContext.h
        BinaryFileWriter.cpp BinaryFileWriter.h
        MockADQAPI.cpp MockADQAPI.h
        MinifiedRecordHeader.h
        Digitizer.cpp Digitizer.h
        DigitizerConstants.h
        AcquisitionConfiguration.cpp AcquisitionConfiguration.h
        SpectroscopeController.h SpectroscopeController.cpp
        util.h util.cpp
        Application.cpp Application.h
        DigitizerGUIComponent.h
        GUILogger.h
        MockADQAPIGen3Constants.h
        BufferProcessor.h BufferProcessor.cpp
        ADQAPIIncluder.h
        RegisterConstants.h
        SpectrumPlotter.h SpectrumPlotter.cpp
        ScopeTab.h ScopeTab.cpp ScopeTab.ui
        ConfigurationController.h ConfigurationController.cpp
        QAcquisitionHandler.h QAcquisitionHandler.cpp
        NullRecordProcessor.h NullRecordProcessor.cpp
        version.h
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
