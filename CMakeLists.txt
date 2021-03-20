cmake_minimum_required(VERSION 3.5)

project(qtadqscope LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

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

include_directories(include/qcustomplot)

set(PROJECT_SOURCES
        main.cpp
        MainWindow.cpp
        MainWindow.h
        MainWindow.ui
        include/qcustomplot/qcustomplot.cpp
        include/qcustomplot/qcustomplot.h
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(qtadqscope
        ${PROJECT_SOURCES}
    )
else()
    if(ANDROID)
        add_library(qtadqscope SHARED
            ${PROJECT_SOURCES}
        )
    else()
        add_executable(qtadqscope
            ${PROJECT_SOURCES}
        )
    endif()
endif()

if(${QT_VERSION_MAJOR} GREATER 4)
    set(PROJECT_LIBRARIES
        Qt${QT_VERSION_MAJOR}::Widgets
        Qt5::PrintSupport
    )
else()
    set(PROJECT_LIBRARIES
        Qt${QT_VERSION_MAJOR}::Widgets
    )
endif()

target_link_libraries(qtadqscope PRIVATE ${PROJECT_LIBRARIES})
