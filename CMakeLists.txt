cmake_minimum_required(VERSION 3.0)
project(pypisa)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native")

add_subdirectory(external)

add_library(pypisa MODULE src/binder.cpp
    src/query_engine.cpp
    src/simdbp_query_engine.cpp)
target_include_directories(pypisa
    PUBLIC $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>)
target_link_libraries(pypisa PRIVATE pybind11::module pisa)
set_target_properties(pypisa PROPERTIES PREFIX "${PYTHON_MODULE_PREFIX}"
                                         SUFFIX "${PYTHON_MODULE_EXTENSION}")
