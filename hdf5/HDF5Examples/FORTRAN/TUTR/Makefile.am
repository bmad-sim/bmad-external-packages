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
##
#
# HDF5-Fortran Examples Makefile(.in)
#

include $(top_srcdir)/config/commence.am

INSTALL_SCRIPT_FILES = run-fortran-ex.sh

# These are the programs that 'make all' or 'make tests' will build and
# that 'make check' will run.  List them in the order in which they should
# be run.
# We don't tell automake about these programs so that it doesn't try to
# compile them with the regular fortran compiler.

EXAMPLE_PROG=h5_crtdat h5_rdwt h5_crtatt h5_crtgrp \
     h5_crtgrpar h5_crtgrpd h5_extend h5_subset h5_cmprss hyperslab selectele \
     refobjexample refregexample mountexample compound

# List files to be installed here
INSTALL_FILES=h5_crtdat.f90 h5_rdwt.f90 \
     h5_crtatt.f90 h5_crtgrp.f90 h5_crtgrpar.f90 h5_crtgrpd.f90  \
     h5_extend.f90 h5_subset.f90 h5_cmprss.f90 hyperslab.f90 selectele.f90 refobjexample.f90 \
     refregexample.f90 mountexample.f90 compound.f90 ph5example.f90


# Add attention tests for Fortran 2003 features
EXAMPLE_PROG +=  rwdset_fortran2003 nested_derived_type \
    compound_fortran2003 compound_complex_fortran2003
INSTALL_FILES +=  rwdset_fortran2003.f90 nested_derived_type.f90 \
    compound_fortran2003.f90 compound_complex_fortran2003.f90
TEST_SCRIPT=testh5fc.sh
TEST_EXAMPLES_SCRIPT=$(INSTALL_SCRIPT_FILES)

# Mark this directory as part of the Fortran API
FORTRAN_API=yes

# Some examples depend on files created by other examples.
h5_crtgrpd.chkexe_: h5_crtgrpar.chkexe_
refregexample.chkexe_: refobjexample.chkexe_
# h5_rdwt and h5_crtatt both modify the same file created by
# h5_crtdat. Serialize them.
h5_rdwt.chkexe_: h5_crtdat.chkexe_
h5_crtatt.chkexe_: h5_rdwt.chkexe_

# Tell automake how to build examples using h5fc
# Additional dependencies for the examples are listed below
$(EXTRA_PROG): $(H5FC)
	$(H5FC) $(H5CCFLAGS) -o $@ $(srcdir)/$@.f90;

# Tell automake how to install examples
# Note: no '/' after DESTDIR.  Explanation in commence.am
EXAMPLEDIR=${DESTDIR}$(examplesdir)/fortran
EXAMPLETOPDIR=${DESTDIR}$(examplesdir)

# List dependencies for each example.  Normally, automake would take
# care of this for us, but if we tell automake about the programs it
# will try to build them with the normal C compiler, not h5cc.  This is
# an inelegant way of solving the problem.
# All programs share the same build rule and a dependency on the main hdf5
# and fortran libraries above.
h5_crtdat: h5_crtdat.f90
h5_extend: h5_extend.f90
h5_subset: h5_subset.f90
h5_rdwt: h5_rdwt.f90
h5_crtatt: h5_crtatt.f90
h5_crtgrp: h5_crtgrp.f90
h5_crtgrpar: h5_crtgrpar.f90
h5_crtgrpd: h5_crtgrpd.f90
h5_cmprss: h5_cmprss.f90
hyperslab: hyperslab.f90
selectele: selectele.f90
refobjexample: refobjexample.f90
refregexample: refregexample.f90
mountexample: mountexample.f90
compound: compound.f90
rwdset_fortran2003: rwdset_fortran2003.f90
nested_derived_type: nested_derived_type.f90
compound_fortran2003: compound_fortran2003.f90
compound_complex_fortran2003: compound_complex_fortran2003.f90

include $(top_srcdir)/config/examples.am
include $(top_srcdir)/config/conclude_fc.am
