cmake_minimum_required(VERSION 3.10)

project(EUCLID
    LANGUAGES CXX
    VERSION 0.1.0
)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules)

option(BUILD_DOC "Build documentation" ON)
option(BUILD_TEST "Build testing" ON)

if(BUILD_DOC)
    add_subdirectory(docs)
endif()

if(BUILD_TEST)
    add_subdirectory(test)
endif()
