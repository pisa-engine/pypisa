cmake_minimum_required(VERSION 3.0)
project(pysa)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


EXECUTE_PROCESS(COMMAND git submodule update --init
                WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/..
                OUTPUT_QUIET
        )

# Add pybind11
add_subdirectory(pybind11)

# Add PISA
set(PISA_BUILD_TOOLS OFF CACHE BOOL "skip building")
set(PISA_ENABLE_TESTING OFF CACHE BOOL "skip testing")
set(PISA_ENABLE_BENCHMARKING OFF CACHE BOOL "skip benchmarking")
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/pisa EXCLUDE_FROM_ALL)

add_library(pysa MODULE src/binder.cpp)
target_link_libraries(pysa PRIVATE pybind11::module pisa)
set_target_properties(pysa PROPERTIES PREFIX "${PYTHON_MODULE_PREFIX}"
                                         SUFFIX "${PYTHON_MODULE_EXTENSION}")



