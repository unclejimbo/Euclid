# Add target
add_executable(run_test
    "${CMAKE_CURRENT_SOURCE_DIR}/config.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/main.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Analysis/test_AABB.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Analysis/test_OBB.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Analysis/test_ViewSelection.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Geometry/test_MeshHelpers.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Geometry/test_MeshProperties.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Geometry/test_PrimitiveGenerator.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ImgProc/test_Histogram.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/IO/test_OffIO.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/IO/test_PlyIO.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/IO/test_InputFixer.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Math/test_Matrix.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Math/test_Numeric.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Math/test_Transformation.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Math/test_Vector.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Render/test_RayTracer.cpp")

# Compiler settings
# TODO: msvc is not tested
target_compile_features(run_test PUBLIC cxx_std_17)
target_compile_options(run_test PUBLIC
    -pipe -fstack-protector-strong -fno-plt
    $<$<CXX_COMPILER_ID:GNU>:-frounding-math>
    $<$<CONFIG:Debug>:-O0 -Wall -Wextra>)

# Find Boost
find_package(Boost REQUIRED)

# Find Eigen3
find_package(Eigen3 REQUIRED)

# Find CGAL
find_package(CGAL REQUIRED)
set(CGAL_DONT_OVERRIDE_CMAKE_FLAGS TRUE CACHE BOOL "Force GGAL to maintain CMAKE_*_FLAGS")
include(${CGAL_USE_FILE})
message(STATUS "cgal include ${CGAL_INCLUDE_DIRS}")
message(STATUS "cgal lib ${CGAL_LIBRARIES}")
message(STATUS "cgal 3rd ${CGAL_3RD_PARTY_LIBRARIES}")

# Find Libigl
find_package(Libigl REQUIRED)

# Find OpenCV
find_package(OpenCV REQUIRED core imgproc)

# Find Embree
find_package(embree 3.0 REQUIRED)

# Config files
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.h.in ${CMAKE_CURRENT_SOURCE_DIR}/config.h)

# Include directories
target_include_directories(run_test PUBLIC
    ${Boost_INCLUDE_DIRS}
    ${EIGEN3_INCLUDE_DIR}
    ${CGAL_INCLUDE_DIRS}
    ${GMP_INCLUDE_DIR}
    ${MPFR_INCLUDE_DIR}
    ${Libigl_INCLUDE_DIR}
    ${OpenCV_INCLUDE_DIRS}
    ${EMBREE_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/../include/
    ${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/)

# Linking
target_link_libraries(run_test PUBLIC
    stdc++fs
    ${CGAL_LIBRARIES}
    ${CGAL_3RD_PARTY_LIBRARIES}
    ${GMP_LIBRARIES}
    ${MPFR_LIBRARIES}
    ${OpenCV_LIBS}
    ${EMBREE_LIBRARY})

    message(STATUS ${EMBREE_LIBRARY})

# Output
set_target_properties(run_test PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/)
