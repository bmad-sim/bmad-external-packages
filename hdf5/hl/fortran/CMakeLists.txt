cmake_minimum_required (VERSION 3.18)
project (HDF5_HL_F90 C Fortran)

#-----------------------------------------------------------------------------
# List Source files
#-----------------------------------------------------------------------------
add_subdirectory (src)

#-----------------------------------------------------------------------------
# Testing
#-----------------------------------------------------------------------------
if (BUILD_TESTING)
  add_subdirectory (test)
endif ()
