cmake_minimum_required(VERSION 3.10)

project(EUCLID
    LANGUAGES CXX
    VERSION 0.1.0
)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules)

# Main target
include(cmake/Euclid.cmake)

# Optional targets
option(EUCLID_BUILD_DOC "Build documentation" ON)
if(EUCLID_BUILD_DOC)
    add_subdirectory(docs)
endif()

option(EUCLID_BUILD_TEST "Build testing" ON)
if(EUCLID_BUILD_TEST)
    add_subdirectory(test)
endif()

option(EUCLID_BUILD_EXAMPLE "Build examples" ON)
if(EUCLID_BUILD_EXAMPLE)
    add_subdirectory(examples)
endif()
