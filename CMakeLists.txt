cmake_minimum_required(VERSION 3.10)
project(Euclid CXX)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules)

option(BUILD_DOC "Build documentation" ON)
option(BUILD_TEST "Build testing" ON)

if(${BUILD_DOC} STREQUAL "ON")
    add_subdirectory(docs)
endif()

if(${BUILD_TEST} STREQUAL "ON")
    add_subdirectory(test)
endif()
