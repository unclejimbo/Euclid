# Find libigl.
#
# The folllowing variables are set if libigl is found. If libigl is not
# found, Libigl_FOUND is set to false.
#
# Libigl_FOUND
# Libigl_INCLUDE_DIR

find_path(Libigl_INCLUDE_DIR igl/AABB.h PATHS
    /usr/include
    /usr/local/include
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Libigl DEFAULT_MSG Libigl_INCLUDE_DIR)

if(Libigl_FOUND)
    mark_as_advanced(Libigl_INCLUDE_DIR)
endif()
