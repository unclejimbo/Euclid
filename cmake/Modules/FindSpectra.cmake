# Find Spectra.
# The following variables are set if Spectra is found.
#
# Spectra_FOUND
# Spectra_INCLUDE_DIR

find_path(Spectra_INCLUDE_DIR SymEigsSolver.h
    /usr/include
    /usr/local/include
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Spectra DEFAULT_MSG Spectra_INCLUDE_DIR)

if(Spetra_FOUND)
    mark_as_advanced(Spectra_INCLUDE_DIR)
endif()
