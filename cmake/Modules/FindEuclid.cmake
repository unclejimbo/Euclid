# Find Euclid.
#
# The folllowing variables are set if Euclid is found.
#
# Euclid_FOUND
# Euclid_INCLUDE_DIR


find_path(Euclid_INCLUDE_DIR Euclid/Geometry/TriMeshGeometry.h PATHS
    /usr/include
    /usr/local/include
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Euclid DEFAULT_MSG Euclid_INCLUDE_DIR)

if(Euclid_FOUND)
    mark_as_advanced(Euclid_INCLUDE_DIR)
endif()
