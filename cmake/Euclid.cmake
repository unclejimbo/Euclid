add_library(Euclid INTERFACE)

add_library(Euclid::Euclid ALIAS Euclid)

target_compile_features(Euclid INTERFACE cxx_std_17)

target_include_directories(Euclid INTERFACE
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

# mandatory dependencies
find_package(Boost REQUIRED)
target_link_libraries(Euclid INTERFACE Boost::boost)

find_package(CGAL CONFIG REQUIRED)
# CGAL tries to override CMAKE_*_FLAGS, do not let it
set(CGAL_DONT_OVERRIDE_CMAKE_FLAGS TRUE CACHE BOOL
    "Force GGAL to maintain CMAKE_*_FLAGS"
)
target_link_libraries(Euclid INTERFACE CGAL::CGAL)

find_package(Eigen3 CONFIG REQUIRED)
target_link_libraries(Euclid INTERFACE Eigen3::Eigen)

# optional dependencies can be switched on and off by requirement,
# headers requiring these packages will be automatically disabled if dependencies are not set
option(EUCLID_USE_SPECTRA "Use Spectra" ON)
if(EUCLID_USE_SPECTRA)
    find_package(Spectra REQUIRED)
    target_include_directories(Euclid INTERFACE ${Spectra_INCLUDE_DIRS})
endif()

option(EUCLID_USE_LIBIGL "Use Libigl" ON)
if(EUCLID_USE_LIBIGL)
    find_package(Libigl REQUIRED)
    target_include_directories(Euclid INTERFACE ${Libigl_INCLUDE_DIR})
endif()

option(EUCLID_USE_EMBREE "Use Embree" ON)
if(EUCLID_USE_EMBREE)
    find_package(Embree 3.0 CONFIG REQUIRED)
    target_link_libraries(Euclid INTERFACE embree)
endif()

option(EUCLID_USE_VULKAN "Use Vulkan" ON)
if(EUCLID_USE_VULKAN)
    find_package(Vulkan 1.2 REQUIRED)
    target_link_libraries(Euclid INTERFACE Vulkan::Vulkan)
endif()

option(EUCLID_USE_TTK "Use ttk" ON)
if(EUCLID_USE_TTK)
    find_package(TTKBase CONFIG REQUIRED)
    target_link_libraries(Euclid INTERFACE ttk::base::baseAll)
endif()

option(EUCLID_USE_CEREAL "Use cereal" ON)
if(EUCLID_USE_CEREAL)
    find_package(cereal CONFIG REQUIRED)
    target_link_libraries(Euclid INTERFACE cereal)
endif()

option(EUCLID_USE_BLAS "Use BLAS" OFF)
if(EUCLID_USE_BLAS)
    find_package(BLAS REQUIRED)
    target_compile_definitions(Euclid ${BLAS_DEFINITIONS})
    target_link_libraries(Euclid ${BLAS_LIBRARIES})
endif()

option(EUCLID_USE_LAPACK "Use LAPACK" OFF)
if(EUCLID_USE_LAPACK)
    find_package(LAPACK REQUIRED)
    target_compile_definitions(Euclid ${LAPACK_DEFINITIONS})
    target_link_libraries(Euclid ${LAPACK_LIBRARIES})
endif()

option(EUCLID_USE_OPENMP "Use OPENMP" OFF)
if(EUCLID_USE_OPENMP)
    find_package(OpenMP REQUIRED)
    target_link_libraries(Euclid OpenMP::OpenMP_CXX)
endif()

# installation
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
