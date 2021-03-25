cmake_minimum_required(VERSION 3.10)

# set the project name and version
project(adq_fast_save VERSION 0.1)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_compile_definitions(LINUX)

add_subdirectory(include/spdlog-1.8.2)
find_package(spdlog)
find_package(adq)
find_package(HDF5)
set(_hdf5_libs hdf5 hdf5_cpp)

list(APPEND EXTRA_LIBS spdlog)
list(APPEND EXTRA_LIBS adq)

# add the executable
add_executable(adq_fast_save main.cpp application.cpp streaming.cpp application_settings.cpp util.cpp hdf5_write.cpp streaming_headers.cpp buffer_processor.cpp)

target_link_libraries(adq_fast_save PUBLIC ${EXTRA_LIBS})
# add the binary tree to the search path for include files
# so that we will find TutorialConfig.h
target_include_directories(adq_fast_save PUBLIC
                           "${PROJECT_BINARY_DIR}"
                           ${EXTRA_INCLUDES}
                           ${HDF5_INCLUDE_DIR}
                           )


                           