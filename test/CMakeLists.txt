add_executable(run_test
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Analysis/test_AABB.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Analysis/test_Descriptor.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Analysis/test_OBB.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Analysis/test_Segmentation.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Analysis/test_ViewSelection.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Geometry/test_Geodesics.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Geometry/test_MeshHelpers.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Geometry/test_PrimitiveGenerator.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Geometry/test_Spectral.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Geometry/test_TriMeshGeometry.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/IO/test_ObjIO.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/IO/test_OffIO.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/IO/test_PlyIO.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/IO/test_InputFixer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Math/test_Distance.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Math/test_Numeric.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Math/test_Statistics.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Math/test_Transformation.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Math/test_Vector.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Render/test_Rasterizer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Render/test_RayTracer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Render/test_RenderCore.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Util/test_Color.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Util/test_Memory.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Util/test_Serialize.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Util/test_Timer.cpp
)

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/config.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/config.h
)

# Required pcakges
find_package(Boost REQUIRED)
find_package(Eigen3 REQUIRED)
find_package(Libigl REQUIRED)
find_package(Embree 3.0 REQUIRED)
find_package(Vulkan REQUIRED)
find_package(CGAL REQUIRED)
find_package(cereal REQUIRED)

target_compile_options(run_test PRIVATE
    $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:GNU>>:
        -pipe -fstack-protector-strong -fno-plt -march=native
        $<$<CONFIG:Debug>:-O0 -Wall -Wextra>>
    $<$<CXX_COMPILER_ID:GNU>:-frounding-math>
    $<$<CXX_COMPILER_ID:MSVC>:
        $<$<CONFIG:Debug>:/Od /W3 /Zi>>
)

target_compile_definitions(run_test PRIVATE
    EUCLID_NO_WARNING
    $<$<CXX_COMPILER_ID:MSVC>:_SILENCE_CXX17_NEGATORS_DEPRECATION_WARNING>
)

target_include_directories(run_test PRIVATE
    ${CMAKE_SOURCE_DIR}/3rdparty/
    ${CMAKE_CURRENT_BINARY_DIR}
)

target_link_libraries(run_test PRIVATE
    Euclid::Euclid
    $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:GNU>>:stdc++fs>
)

set_target_properties(run_test PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/test
)
