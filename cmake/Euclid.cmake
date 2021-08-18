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

find_package(Libigl REQUIRED)
target_include_directories(Euclid INTERFACE ${Libigl_INCLUDE_DIR})

find_package(Threads REQUIRED)
target_link_libraries(Euclid INTERFACE Threads::Threads)

# installation
include(GNUInstallDirs)

install(TARGETS Euclid
    EXPORT EuclidTargets
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
