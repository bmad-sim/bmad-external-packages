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
##
## Makefile.am
## Run automake to generate a Makefile.in from this file.
##
#
# HDF5 High-Level Fortran Makefile(.in)
#
# Autoconf cannot figure out dependencies between modules; disable parallel make
.NOTPARALLEL:
include $(top_srcdir)/config/commence.am

AM_CPPFLAGS+=-I$(top_srcdir)/src -I$(top_builddir)/src -I$(top_srcdir)/hl/src
AM_FCFLAGS+=-I$(top_builddir)/fortran/src -I$(top_builddir)/hl/fortran/src -I$(top_builddir)/fortran/test $(F9XMODFLAG)$(top_builddir)/fortran/src $(F9XMODFLAG)$(top_builddir)/fortran/test $(F9XMODFLAG)$(top_builddir)/hl/fortran/src

# Some Fortran compilers can't build shared libraries, so sometimes we
# need to make sure the Fortran programs link against the static version
# of the HDF5 library.  We do this by passing the -static flag to the linker.
if FORTRAN_SHARED_CONDITIONAL
else
   AM_LDFLAGS+=-static
endif

# Our main target, the test programs
TEST_PROG=tstds tstlite tstimage tsttable
check_PROGRAMS=$(TEST_PROG)

LIBOBJS=$(top_builddir)/fortran/test/tf_gen.o

LDADD=$(LIBOBJS) $(LIBH5F_HL) $(LIBH5F) $(LIBH5_HL)  $(LIBHDF5)

# Source files for the programs
tstds_SOURCES=tstds.F90
tstlite_SOURCES=tstlite.F90
tstimage_SOURCES=tstimage.F90
tsttable_SOURCES=tsttable.F90

# Temporary files.
CHECK_CLEANFILES+=dsetf[1-5].h5 f1img.h5 f[1-2]tab.h5 tstds.h5

# Fortran module files can have different extensions and different names
# (e.g., different capitalizations) on different platforms.  Write rules
# for them explicitly rather than trying to teach automake about them.
# They should be installed as headers and removed during clean.
maintainer-clean-local: clean-local
distclean-local: clean-local
clean-local:
	@if test -n "$(F9XMODEXT)" && test "X$(F9XMODEXT)" != "Xo"; then \
	  $(RM) *.$(F9XMODEXT); \
	fi

# Mark this directory as part of the Fortran API (this affects output
# from tests in conclude.am)
FORTRAN_API=yes

include $(top_srcdir)/config/conclude_fc.am
