list(APPEND SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/BoundingVolume/test_AABB.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/BoundingVolume/test_OBB.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Descriptor/test_Histogram.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Descriptor/test_SpinImage.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Distance/test_GeodesicsInHeat.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Geometry/test_TriMeshGeometry.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/IO/test_ObjIO.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/IO/test_OffIO.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/IO/test_PlyIO.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/IO/test_InputFixer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Math/test_Numeric.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Math/test_Statistics.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Math/test_Transformation.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Math/test_Vector.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/MeshUtil/test_CGALMesh.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/MeshUtil/test_EigenMesh.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/MeshUtil/test_MeshHelpers.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/MeshUtil/test_PrimitiveGenerator.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Parameterization/test_HolomorphicOneForms.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Parameterization/test_RicciFlow.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Parameterization/test_SCP.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Parameterization/test_SeamMesh.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Render/test_RenderCore.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Segmentation/test_RandomWalk.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/SurfaceDelaunay/test_DelaunayMesh.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Topology/test_Chain.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Topology/test_MeshTopology.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Topology/test_HomologyGenerator.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Topology/test_HomotopyGenerator.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Util/test_Memory.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Util/test_Timer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ViewSelection/test_ViewSphere.cpp
)

option(EUCLID_TEST_ENABLE_SPECTRA "Test spectra related packages" ON)
if(${EUCLID_TEST_ENABLE_SPECTRA})
    find_package(Spectra REQUIRED 1.0)
    list(APPEND SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/Descriptor/test_HKS.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Descriptor/test_WKS.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Distance/test_BiharmonicDistance.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Distance/test_DiffusionDistance.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/FeatureDetection/test_NativeHKS.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Geometry/test_Spectral.cpp
    )
endif()

option(EUCLID_TEST_ENABLE_EMBREE "Test embree related packages" ON)
if(${EUCLID_TEST_ENABLE_EMBREE})
    find_package(Embree 3.0 CONFIG REQUIRED)
    list(APPEND SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/Render/test_RayTracer.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/ViewSelection/test_ProxyView.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/ViewSelection/test_ViewEntropy.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Util/test_Color.cpp
    )
endif()

option(EUCLID_TEST_ENABLE_VULKAN "Test vulkan related packages" ON)
if(${EUCLID_TEST_ENABLE_VULKAN})
    find_package(Vulkan 1.2 REQUIRED)
    list(APPEND SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/Render/test_Rasterizer.cpp
    )
endif()

option(EUCLID_TEST_ENABLE_TTK "Test ttk related packages" ON)
if(${EUCLID_TEST_ENABLE_TTK})
    find_package(TTKBase CONFIG REQUIRED)
    list(APPEND SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/MeshUtil/test_TTKMesh.cpp
    )
endif()

option(EUCLID_TEST_ENABLE_CEREAL "Test cereal related packages" ON)
if(${EUCLID_TEST_ENABLE_CEREAL})
    find_package(cereal CONFIG REQUIRED)
    list(APPEND SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/Util/test_Serialize.cpp
    )
endif()

add_executable(run_test ${SOURCES})

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/config.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/config.h
)

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

if(${EUCLID_TEST_ENABLE_SPECTRA})
    target_include_directories(run_test PRIVATE ${Spectra_INCLUDE_DIRS})
endif()

if(${EUCLID_TEST_ENABLE_EMBREE})
    target_link_libraries(run_test PRIVATE embree)
endif()

if(${EUCLID_TEST_ENABLE_VULKAN})
    target_link_libraries(run_test PRIVATE Vulkan::Vulkan)
endif()

if(${EUCLID_TEST_ENABLE_TTK})
    target_link_libraries(run_test PRIVATE
        ttk::base::explicitTriangulation
        ttk::base::scalarFieldSmoother
    )
    # set_target_properties(ttk::base::dijkstra PROPERTIES INTERFACE_COMPILE_OPTIONS "")
    # set_target_properties(ttk::base::rangeMinimumQuery PROPERTIES INTERFACE_COMPILE_OPTIONS "")
endif()

if(${EUCLID_TEST_ENABLE_CEREAL})
    target_link_libraries(run_test PRIVATE cereal)
endif()

option(EUCLID_TEST_ENABLE_OPENMP "Enable OPENMP" OFF)
if(${EUCLID_TEST_ENABLE_OPENMP})
    find_package(OpenMP REQUIRED)
    target_link_libraries(runtest PRIVATE OpenMP::OpenMP_CXX)
endif()

set_target_properties(run_test PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/test
)
