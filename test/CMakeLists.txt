add_executable(run_test
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/BoundingVolume/test_AABB.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/BoundingVolume/test_OBB.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Descriptor/test_Histogram.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Descriptor/test_HKS.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Descriptor/test_SpinImage.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Descriptor/test_WKS.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Distance/test_BiharmonicDistance.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Distance/test_DiffusionDistance.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Distance/test_GeodesicsInHeat.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Geometry/test_Spectral.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Geometry/test_TriMeshGeometry.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/IO/test_ObjIO.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/IO/test_OffIO.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/IO/test_PlyIO.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/IO/test_InputFixer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Math/test_Numeric.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Math/test_Statistics.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Math/test_Transformation.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Math/test_Vector.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/MeshUtil/test_MeshHelpers.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/MeshUtil/test_PrimitiveGenerator.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Parameterization/test_HolomorphicOneForms.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Parameterization/test_SeamMesh.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Render/test_Rasterizer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Render/test_RayTracer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Render/test_RenderCore.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Segmentation/test_RandomWalk.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Topology/test_Chain.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Topology/test_MeshTopology.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Topology/test_HomologyGenerator.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Topology/test_HomotopyGenerator.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Util/test_Color.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Util/test_Memory.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Util/test_Serialize.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Util/test_Timer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ViewSelection/test_ProxyView.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ViewSelection/test_ViewEntropy.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ViewSelection/test_ViewSphere.cpp
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
find_package(Vulkan 1.2 REQUIRED)
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
