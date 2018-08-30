cmake_minimum_required (VERSION 3.0)
set(CMAKE_CXX_STANDARD 11)

project (CGRAMapper
         VERSION 1.0.0)

# include custom modules
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/.cmake/")
# some command C++ flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wfatal-errors")

# default debug build
# this will set debug flags properly
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif(NOT CMAKE_BUILD_TYPE)

# find core_ir
find_package(CoreIR REQUIRED)

# keep the output to old bin folder, instead of cmake build
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)

# fPIC
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# include directories
# we only have src
add_subdirectory(src)
