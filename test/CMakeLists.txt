cmake_minimum_required(VERSION 3.5)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/${CONFIG})

find_package(Eigen3 REQUIRED)
find_package(CGAL REQUIRED)
find_package(CGAL REQUIRED COMPONENTS CGAL_Core)

set(WITH_OPENMP TRUE CACHE BOOL "Enable OpenMP")
if (WITH_OPENMP)
    find_package(OpenMP)
    if (OPENMP_FOUND)
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
    endif()
endif()

include(${CGAL_USE_FILE})
set(EUCLID_INCLUDE_DIR "../include")
include_directories(${CGAL_INCLUDE_DIRS} ${EIGEN3_INCLUDE_DIR} ${EUCLID_INCLUDE_DIR})

file(GLOB_RECURSE SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
file(GLOB_RECURSE HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

add_executable(test_Euclid ${SOURCES} ${HEADERS})

target_link_libraries(test_Euclid ${CGAL_LIBRARIES})
