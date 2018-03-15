# Add target
add_executable(run_test
    "${CMAKE_CURRENT_SOURCE_DIR}/config.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/main.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Analysis/test_AABB.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Analysis/test_OBB.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Geometry/test_Polyhedron_3.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Geometry/test_Surface_mesh.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Geometry/test_MeshBuilder.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ImgProc/test_Histogram.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/IO/test_OffIO.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/IO/test_PlyIO.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/IO/test_InputFixer.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Math/test_Matrix.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Math/test_Numeric.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Math/test_Transformation.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Math/test_Vector.cpp")

# Compiler settings
target_compile_features(run_test PUBLIC cxx_std_17)

if (${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU" OR
    ${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang")
    set(CMAKE_CXX_FLAGS "") # Override default cxx_flags
    target_compile_options(run_test PUBLIC
        $<$<CONFIG:Debug>:-O0 -Wall -Wextra>)
elseif(${CMAKE_CXX_COMPILER_ID} STREQUAL "MSVC")
endif()

# Find Boost
find_package(Boost REQUIRED)

# Find Eigen3
find_package(Eigen3 REQUIRED)

# Find CGAL
find_package(CGAL REQUIRED)
include(${CGAL_USE_FILE})

# Find OpenCV
find_package(OpenCV REQUIRED core imgproc)

# Find OpenMP
set(WITH_OPENMP TRUE CACHE BOOL "Enable OpenMP")
if(WITH_OPENMP)
    find_package(OpenMP)
    if(OPENMP_FOUND)
        target_compile_options(run_test PUBLIC ${OpenMP_CXX_FLAGS})
    endif()
endif()

# Config files
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.h.in ${CMAKE_CURRENT_SOURCE_DIR}/config.h)

# Include directories
target_include_directories(run_test PUBLIC
    ${Boost_INCLUDE_DIRS}
    ${EIGEN3_INCLUDE_DIR}
    ${CGAL_INCLUDE_DIRS}
    ${OpenCV_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/../include/
    ${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/)

# Linking
target_link_libraries(run_test PUBLIC
    ${OpenMP_CXX_LIBRARIES}
    ${CGAL_LIBRARIES}
    ${GMP_LIBRARIES}
    ${MPFR_LIBRARIES}
    ${OpenCV_LIBS})

# Output
set_target_properties(run_test PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/)
