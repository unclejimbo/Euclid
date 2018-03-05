# - Try to find euclid
#
# Once done this will define
#
#  Euclid_FOUND
#  Euclid_INCLUDE_DIR

set(Euclid_INCLUDE_SEARCH_PATHS
  /usr/include
  /usr/local/include
  $ENV{Euclid_HOME}/include
)

find_path(Euclid_INCLUDE_DIR Euclid/Geometry/Polyhedron_3.h PATHS ${Euclid_INCLUDE_SEARCH_PATHS})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Euclid DEFAULT_MSG Euclid_INCLUDE_DIR)

if(Euclid_FOUND)
  mark_as_advanced(Euclid_INCLUDE_DIR)
endif(Euclid_FOUND)
