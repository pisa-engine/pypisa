cmake_minimum_required(VERSION 3.0)
project(pypisa)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


add_subdirectory(external)

add_library(pypisa MODULE src/binder.cpp)
target_link_libraries(pypisa PRIVATE pybind11::module pisa)
set_target_properties(pypisa PROPERTIES PREFIX "${PYTHON_MODULE_PREFIX}"
                                         SUFFIX "${PYTHON_MODULE_EXTENSION}")



