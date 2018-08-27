find_package(Boost REQUIRED)
find_package(Eigen3 REQUIRED)
find_package(Libigl REQUIRED)
find_package(Embree 3.0 REQUIRED)
find_package(CGAL REQUIRED)
find_package(cereal REQUIRED)

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/config.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/config.h
)

add_subdirectory(hello_world)
