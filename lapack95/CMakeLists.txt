cmake_minimum_required(VERSION $ENV{ACC_CMAKE_VERSION})
project(ACC)

SET (LIBNAME lapack95)

SET (CREATE_SHARED true)

SET (SRC_DIRS 
  code
)

set (SHARED_DEPS
  lapack
  blas
)

include($ENV{ACC_BUILD_SYSTEM}/Master.cmake)
