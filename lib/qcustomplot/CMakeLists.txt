cmake_minimum_required(VERSION 3.5)
project(qcustomplot)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets REQUIRED)

set(QCUSTOMPLOT_LIBRARIES
    Qt${QT_VERSION_MAJOR}::Widgets
)
add_library(qcustomplot STATIC)
target_sources(qcustomplot PRIVATE qcustomplot.cpp)
target_include_directories(qcustomplot PUBLIC ${PROJECT_SOURCE_DIR})
target_link_libraries(qcustomplot PUBLIC ${QCUSTOMPLOT_LIBRARIES})
