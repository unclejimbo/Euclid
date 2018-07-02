add_executable(run_test
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Analysis/test_AABB.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Analysis/test_Descriptor.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Analysis/test_OBB.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Analysis/test_ViewSelection.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Geometry/test_MeshHelpers.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Geometry/test_MeshProperties.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Geometry/test_PrimitiveGenerator.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ImgProc/test_Histogram.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/IO/test_ObjIO.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/IO/test_OffIO.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/IO/test_PlyIO.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/IO/test_InputFixer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Math/test_Matrix.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Math/test_Numeric.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Math/test_Transformation.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Math/test_Vector.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Render/test_RayTracer.cpp
)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.h.in ${CMAKE_CURRENT_BINARY_DIR}/config.h)

# Required pcakges
find_package(Boost REQUIRED)
find_package(Eigen3 REQUIRED)
find_package(Libigl REQUIRED)
find_package(OpenCV REQUIRED core imgproc)
find_package(Embree 3.0 REQUIRED)

# CGAL tries to override CMAKE_*_FLAGS, do not let it
set(CGAL_DONT_OVERRIDE_CMAKE_FLAGS TRUE CACHE BOOL
    "Force GGAL to maintain CMAKE_*_FLAGS"
)
find_package(CGAL REQUIRED)

target_compile_features(run_test PRIVATE cxx_std_17)

target_compile_options(run_test PRIVATE
    $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:GNU>>:
        -pipe -fstack-protector-strong -fno-plt -march=native
        $<$<CONFIG:Debug>:-O0 -Wall -Wextra>>
    $<$<CXX_COMPILER_ID:GNU>:-frounding-math>
    $<$<CXX_COMPILER_ID:MSVC>:
        $<$<CONFIG:Debug>:/Od /W3 /Zi>>
)

target_compile_definitions(run_test PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:_SILENCE_CXX17_NEGATORS_DEPRECATION_WARNING>
)

target_include_directories(run_test PRIVATE
    ${Libigl_INCLUDE_DIR}
    ${EMBREE_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}/include/
    ${CMAKE_SOURCE_DIR}/3rdparty/
    ${CMAKE_CURRENT_BINARY_DIR}
)

target_link_libraries(run_test PRIVATE
    Boost::boost
    Eigen3::Eigen
    CGAL::CGAL
    ${OpenCV_LIBS}
    ${EMBREE_LIBRARIES}
    $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:GNU>>:stdc++fs>
)

# Optional packages
find_package(BLAS)
if(BLAS_FOUND)
    target_link_libraries(run_test PRIVATE ${BLAS_LIBRARIES})
    target_compile_definitions(run_test PRIVATE "EIGEN_USE_BLAS")
endif()

find_package(LAPACK)
if(LAPACK_FOUND)
    target_link_libraries(run_test PRIVATE ${LAPACK_LIBRARIES})
    target_compile_definitions(run_test PRIVATE "EIGEN_USE_LAPACK")
endif()

find_package(OpenMP)
if(OpenMP_FOUND)
    target_link_libraries(run_test PRIVATE
        $<$<CONFIG:Release>:OpenMP::OpenMP_CXX>
    )
endif()

# Output
set_target_properties(run_test PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/
)
