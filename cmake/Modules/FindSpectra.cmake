# Find Spectra.
# The following variables are set if Spectra is found.
#
# Spectra_FOUND
# Spectra_INCLUDE_DIRS

find_path(Spectra_INCLUDE_DIRS SymEigsSolver.h
    /usr/include
    /usr/local/include
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Spectra DEFAULT_MSG Spectra_INCLUDE_DIRS)

if(Spetra_FOUND)
    mark_as_advanced(Spectra_INCLUDE_DIRS)
endif()
