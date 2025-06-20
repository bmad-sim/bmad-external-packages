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

# Include src and tools/lib directories
AM_CPPFLAGS+=-I$(top_srcdir)/src -I$(top_srcdir)/tools/lib -I$(top_srcdir)/hl/src

# These are our main targets, the tools

bin_PROGRAMS=h5watch

# Add h5watch specific linker flags here
h5watch_LDFLAGS = $(LT_STATIC_EXEC) $(AM_LDFLAGS)

# Programs all depend on the hdf5 library, the tools library, and the HL
# library.
LDADD=$(LIBH5_HL) $(LIBH5TOOLS) $(LIBHDF5)

if BUILD_TESTS_CONDITIONAL
  AM_CPPFLAGS+=-I$(top_srcdir)/hl/test
  TEST_SCRIPT=testh5watch.sh
  check_SCRIPTS=$(TEST_SCRIPT)
  SCRIPT_DEPEND=swmr_check_compat_vfd$(EXEEXT) extend_dset$(EXEEXT) h5watch$(EXEEXT)
  noinst_PROGRAMS=h5watchgentest extend_dset
  # Add extend_dset specific preprocessor flags here
  # (add the main test subdirectory to the include file path)
  extend_dset_CPPFLAGS=$(AM_CPPFLAGS) -I$(top_srcdir)/test
  # Add extend_dset specific library flags here
  # (add the main test library to the list of libraries)
  extend_dset_LDADD=$(LDADD) $(LIBH5TEST) $(LIBHDF5)
endif
#
CHECK_CLEANFILES+=*.h5
DISTCLEANFILES=testh5watch.sh

include $(top_srcdir)/config/conclude.am
