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
