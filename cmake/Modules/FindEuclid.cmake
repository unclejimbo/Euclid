# - Try to find euclid
#
# Once done this will define
#
#  Euclid_FOUND
#  Euclid_INCLUDE_DIR

find_path(Euclid_INCLUDE_DIR Euclid/Util/Assert.h PATHS
  /usr/include
  /usr/local/include)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Euclid DEFAULT_MSG Euclid_INCLUDE_DIR)

if(Euclid_FOUND)
  mark_as_advanced(Euclid_INCLUDE_DIR)
endif(Euclid_FOUND)
