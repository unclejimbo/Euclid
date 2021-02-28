cmake_minimum_required(VERSION 3.10)

project(EUCLID
    LANGUAGES CXX
    VERSION 0.1.0
)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules)

# Main target
include(cmake/Euclid.cmake)

# Optional targets
option(BUILD_DOC "Build documentation" ON)
if(BUILD_DOC)
    add_subdirectory(docs)
endif()

option(BUILD_TEST "Build testing" ON)
if(BUILD_TEST)
    add_subdirectory(test)
endif()

option(BUILD_EXAMPLE "Build examples" ON)
if(BUILD_EXAMPLE)
    add_subdirectory(examples)
endif()
