cmake_minimum_required(VERSION 3.10)

project(EUCLID
    LANGUAGES CXX
    VERSION 0.1.0
)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules)

# The library target
add_library(Euclid INTERFACE)
add_library(Euclid::Euclid ALIAS Euclid)

find_package(Boost)
find_package(Eigen3)
find_package(Spectra)
find_package(Libigl)
find_package(Embree 3.0)
find_package(Vulkan)
find_package(CGAL)
find_package(cereal)

option(USE_BLAS "Use BLAS" OFF)
if(USE_BLAS)
    find_package(BLAS)
endif()

option(USE_LAPACK "Use LAPACK" OFF)
if(USE_LAPACK)
    find_package(LAPACK)
endif()

option(USE_OPENMP "Use OPENMP" OFF)
if(USE_OPENMP)
    find_package(OpenMP)
endif()

# CGAL tries to override CMAKE_*_FLAGS, do not let it
set(CGAL_DONT_OVERRIDE_CMAKE_FLAGS TRUE CACHE BOOL
    "Force GGAL to maintain CMAKE_*_FLAGS"
)

target_compile_features(Euclid INTERFACE cxx_std_17)

target_include_directories(Euclid INTERFACE
    ${Spectra_INCLUDE_DIR}
    ${Libigl_INCLUDE_DIR}
    ${EMBREE_INCLUDE_DIRS}
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(Euclid INTERFACE
    Boost::boost
    Eigen3::Eigen
    CGAL::CGAL
    cereal
    Vulkan::Vulkan
    ${EMBREE_LIBRARIES}
    $<$<AND:$<BOOL:${USE_BLAS}>,$<BOOL:${BLAS_FOUND}>>:${BLAS_LIBRARIES}>
    $<$<AND:$<BOOL:${USE_LAPACK}>,$<BOOL:${LAPACK_FOUND}>>:${LAPACK_LIBRARIES}>
    $<$<AND:$<BOOL:${USE_OPENMP}>,$<BOOL:${OpenMP_FOUND}>>:OpenMP::OpenMP_CXX>
)

target_compile_definitions(Euclid INTERFACE
    $<$<AND:$<BOOL:${USE_BLAS}>,$<BOOL:${BLAS_FOUND}>>:"EIGEN_USE_BLAS" ${BLAS_DEFINITIONS}>
    $<$<AND:$<BOOL:${USE_LAPACK}>,$<BOOL:${LAPACK_FOUND}>>:"EIGEN_USE_LAPACK" ${LAPACK_DEFINITIONS}>
)

include(GNUInstallDirs)

install(TARGETS Euclid
    EXPORT EuclidTargets
    INCLUDES DESTINATION
        ${Spectra_INCLUDE_DIR}
        ${Libigl_INCLUDE_DIR}
        ${EMBREE_INCLUDE_DIRS}
)

install(DIRECTORY ${CMAKE_SOURCE_DIR}/include/Euclid
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(EXPORT EuclidTargets
    FILE EuclidTargets.cmake
    NAMESPACE Euclid::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/Euclid
)

include(CMakePackageConfigHelpers)

write_basic_package_version_file(
    ${CMAKE_BINARY_DIR}/cmake/EuclidConfigVersion.cmake
    VERSION ${EUCLID_VERSION}
    COMPATIBILITY AnyNewerVersion
)

configure_package_config_file(
    ${CMAKE_SOURCE_DIR}/cmake/EuclidConfig.cmake.in
    ${CMAKE_BINARY_DIR}/cmake/EuclidConfig.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/Euclid
)

install(
    FILES
        ${CMAKE_BINARY_DIR}/cmake/EuclidConfig.cmake
        ${CMAKE_BINARY_DIR}/cmake/EuclidConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/Euclid
)
install(
    FILES
        ${CMAKE_SOURCE_DIR}/cmake/Modules/FindSpectra.cmake
        ${CMAKE_SOURCE_DIR}/cmake/Modules/FindLibigl.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/Euclid/Modules
)

configure_file(
    ${CMAKE_SOURCE_DIR}/cmake/Modules/FindSpectra.cmake
    ${CMAKE_BINARY_DIR}/cmake/Modules/FindSpectra.cmake
    COPYONLY
)
configure_file(
    ${CMAKE_SOURCE_DIR}/cmake/Modules/FindLibigl.cmake
    ${CMAKE_BINARY_DIR}/cmake/Modules/FindLibigl.cmake
    COPYONLY
)

export(EXPORT EuclidTargets
    FILE ${CMAKE_BINARY_DIR}/cmake/EuclidTargets.cmake
    NAMESPACE Euclid::
)

export(PACKAGE Euclid)

# Optional targets
option(BUILD_DOC "Build documentation" ON)
option(BUILD_TEST "Build testing" ON)
option(BUILD_EXAMPLE "Build examples" ON)

if(BUILD_DOC)
    add_subdirectory(docs)
endif()

if(BUILD_TEST)
    add_subdirectory(test)
endif()

if(BUILD_EXAMPLE)
    add_subdirectory(examples)
endif()
