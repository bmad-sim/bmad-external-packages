#
# Copyright by The HDF Group.
# All rights reserved.
#
# This file is part of HDF5.  The full HDF5 copyright notice, including
# terms governing use, modification, and redistribution, is contained in
# the COPYING file, which can be found at the root of the source code
# distribution tree, or in https://www.hdfgroup.org/licenses.
# If you do not have access to either file, you may request a copy from
# help@hdfgroup.org.
#
#############################################################################################
####  Change default configuration of options in config/cmake/cacheinit.cmake file        ###
####  format: set(ADD_BUILD_OPTIONS "${ADD_BUILD_OPTIONS} -DXXX:YY=ZZZZ")                 ###
####  DEFAULT:                                                                            ###
####         BUILD_SHARED_LIBS:BOOL=OFF                                                   ###
####         H5EX_BUILD_C:BOOL=ON                                                         ###
####         H5EX_BUILD_HL:BOOL=ON                                                        ###
####         H5EX_BUILD_CXX:BOOL=OFF                                                      ###
####         H5EX_BUILD_FORTRAN:BOOL=OFF                                                  ###
####         H5EX_BUILD_JAVA:BOOL=OFF                                                     ###
####         H5EX_BUILD_FILTERS:BOOL=OFF                                                  ###
####         H5EX_BUILD_TESTING:BOOL=OFF                                                  ###
####         H5EX_ENABLE_PARALLEL:BOOL=OFF                                                ###
#############################################################################################

### uncomment/comment and change the following lines for other configuration options
### build with shared libraries
#set(ADD_BUILD_OPTIONS "${ADD_BUILD_OPTIONS} -DBUILD_SHARED_LIBS:BOOL=ON")

#############################################################################################
####      maximum parallel processor count for build and test       ####
#set(MAX_PROC_COUNT 8)

#############################################################################################
####      alternate toolsets (Windows usually)        ####
#set(CMAKE_GENERATOR_TOOLSET "Intel C++ Compiler 17.0")

#############################################################################################
### use a toolchain file (supported everywhere)       ####
#set(ADD_BUILD_OPTIONS "${ADD_BUILD_OPTIONS} -DCMAKE_TOOLCHAIN_FILE:STRING=config/toolchain/clang.cmake")
#set(ADD_BUILD_OPTIONS "${ADD_BUILD_OPTIONS} -DCMAKE_TOOLCHAIN_FILE:STRING=config/toolchain/intel.cmake")

#############################################################################################
####      languages       ####
### disable C builds
#set(ADD_BUILD_OPTIONS "${ADD_BUILD_OPTIONS} -DH5EX_BUILD_C:BOOL=OFF")

### enable HL builds
#set(ADD_BUILD_OPTIONS "${ADD_BUILD_OPTIONS} -DH5EX_BUILD_HL:BOOL=ON")

### enable C++ builds
#set(ADD_BUILD_OPTIONS "${ADD_BUILD_OPTIONS} -DH5EX_BUILD_CXX:BOOL=ON")

### enable Fortran builds
#set(ADD_BUILD_OPTIONS "${ADD_BUILD_OPTIONS} -DH5EX_BUILD_FORTRAN:BOOL=ON")

### enable JAVA builds
#set(ADD_BUILD_OPTIONS "${ADD_BUILD_OPTIONS} -DH5EX_BUILD_JAVA:BOOL=ON")

#############################################################################################
### enable FILTERS builds
#set(ADD_BUILD_OPTIONS "${ADD_BUILD_OPTIONS} -DH5EX_BUILD_FILTERS:BOOL=ON")
### default HDF5_PLUGIN_PATH to where the filter libraries are located
#set(ENV{HDF5_PLUGIN_PATH} "${INSTALLDIR}/lib/plugin")

#############################################################################################
### enable parallel program builds
#set(ADD_BUILD_OPTIONS "${ADD_BUILD_OPTIONS} -DH5EX_ENABLE_PARALLEL:BOOL=ON")

#############################################################################################
### match the hdf5 library namespace
set(ADD_BUILD_OPTIONS "${ADD_BUILD_OPTIONS} -DHDF5_NAMESPACE:STRING=hdf5::")

#############################################################################################
### enable test program builds, requires reference files in testfiles subdirectory
#set(ADD_BUILD_OPTIONS "${ADD_BUILD_OPTIONS} -DH5EX_BUILD_TESTING:BOOL=ON")
#set(ADD_BUILD_OPTIONS "${ADD_BUILD_OPTIONS} -DH5EX_COMPARE_TESTING:BOOL=ON")

#############################################################################################
