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
# HDF5 Library Makefile(.in)
#

include $(top_srcdir)/config/commence.am

# Include files in /src directory and /tools/lib directory
AM_CPPFLAGS+=-I$(top_srcdir)/src -I$(top_srcdir)/tools/lib

# Our main target, the h5dump tool.
bin_PROGRAMS=h5dump

# Add h5dump specific linker flags here
h5dump_LDFLAGS = $(LT_STATIC_EXEC) $(AM_LDFLAGS)

# All the programs depend on the hdf5 and h5tools libraries
LDADD=$(LIBH5TOOLS) $(LIBHDF5)

# Source files for the program
h5dump_SOURCES=h5dump.c h5dump_ddl.c h5dump_xml.c

include $(top_srcdir)/config/conclude.am
