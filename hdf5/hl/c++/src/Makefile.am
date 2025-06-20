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
##
## Makefile.am
## Run automake to generate a Makefile.in from this file.
#
# HDF5-C++ Makefile(.in)
#

include $(top_srcdir)/config/commence.am
include $(top_srcdir)/config/lt_vers.am

# Include src directory
AM_CPPFLAGS+=-I$(top_srcdir)/src -I$(top_srcdir)/hl/src

# This is our main target
lib_LTLIBRARIES=libhdf5_hl_cpp.la

# Add libtool numbers to the HDF5 HL C++ library (from config/lt_vers.am)
libhdf5_hl_cpp_la_LDFLAGS= -version-info $(LT_HL_CXX_VERS_INTERFACE):$(LT_HL_CXX_VERS_REVISION):$(LT_HL_CXX_VERS_AGE) $(AM_LDFLAGS)

# Source files for the library
# At the moment, only the H5PT Packet Table has a C++ API.
libhdf5_hl_cpp_la_SOURCES=H5PacketTable.cpp

# HDF5 HL C++ library depends on HDF5 Library.
libhdf5_hl_cpp_la_LIBADD=$(LIBH5_HL) $(LIBH5CPP)

# Public headers
include_HEADERS=H5PacketTable.h

include $(top_srcdir)/config/conclude.am
