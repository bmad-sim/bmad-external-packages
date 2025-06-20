!************************************************************
!
!  This example shows how to read and write opaque datatypes
!  to an attribute.  The program first writes opaque data to
!  an attribute with a dataspace of DIM0, then closes the
!  file. Next, it reopens the file, reads back the data, and
!  outputs it to the screen.
!
!  This file is intended for use with HDF5 Library version 1.8
!  with --enable-fortran2003
!
!************************************************************
PROGRAM main

  USE HDF5
  USE ISO_C_BINDING

  IMPLICIT NONE
  CHARACTER(LEN=23), PARAMETER :: filename  = "h5ex_t_opaqueatt_F03.h5"
  CHARACTER(LEN=3) , PARAMETER :: dataset   = "DS1"
  CHARACTER(LEN=2) , PARAMETER :: attribute = "A1"
  INTEGER          , PARAMETER :: dim0      = 4
  INTEGER(SIZE_T)  , PARAMETER :: size      = 7
  INTEGER(HID_T)  :: file, space, dtype, dset, attr ! Handles
  INTEGER :: hdferr
  INTEGER(size_t) :: len
  INTEGER(hsize_t),   DIMENSION(1:1) :: dims = (/DIM0/)

  CHARACTER(LEN=size), DIMENSION(1:dim0), TARGET :: wdata ! Write buffer
  CHARACTER(LEN=size), DIMENSION(:), ALLOCATABLE, TARGET :: rdata ! Read buffer
  CHARACTER(LEN=size-1) :: str = "OPAQUE"
  CHARACTER(LEN=80) :: tag
  INTEGER :: taglen
  INTEGER(HSIZE_T), DIMENSION(1:1) :: maxdims
  INTEGER :: i
  CHARACTER(LEN=1) :: ichr
  TYPE(C_PTR) :: f_ptr
  !
  ! Initialize FORTRAN interface.
  !
  CALL h5open_f(hdferr)
  !
  ! Initialize data.
  !
  DO i = 1, dim0
     WRITE(ichr,'(I1)') i-1 
     wdata(i) = str//ichr
  ENDDO
  !
  ! Create a new file using the default properties.
  !
  CALL h5fcreate_f(filename, H5F_ACC_TRUNC_F, file, hdferr)
  !
  ! Create dataset with a null dataspace.
  !
  CALL h5screate_f(H5S_NULL_F,space,hdferr)
  CALL h5dcreate_f(file, dataset, H5T_STD_I32LE, space, dset, hdferr)

  CALL h5sclose_f(space, hdferr)
  !
  ! Create opaque datatype and set the tag to something appropriate.
  ! For this example we will write and view the data as a character
  ! array.
  !
  CALL h5tcreate_f(h5T_OPAQUE_F, size, dtype, hdferr)
  CALL h5tset_tag_f(dtype,"Character array",hdferr)
  !
  ! Create dataspace.  Setting maximum size to be the current size.
  !
  CALL h5screate_simple_f(1, dims, space, hdferr)
  !
  ! Create the attribute and write the opaque data to it.
  !
  CALL H5Acreate_f(dset, attribute, dtype, space, attr, hdferr)
  f_ptr = C_LOC(wdata(1)(1:1))
  CALL H5Awrite_f(attr, dtype, f_ptr, hdferr)
  !
  ! Close and release resources.
  !
  CALL H5Aclose_f(attr, hdferr)
  CALL H5Dclose_f(dset, hdferr)
  CALL H5Sclose_f(space, hdferr)
  CALL H5Tclose_f(dtype, hdferr)
  CALL H5Fclose_f(file, hdferr)
  !
  ! Now we begin the read section of this example.
  !
  ! Open file and dataset, and attribute.
  !
  CALL h5fopen_f(filename, H5F_ACC_RDONLY_F, file, hdferr)
  CALL h5dopen_f(file, dataset, dset, hdferr)
  CALL h5aopen_f(dset, attribute, attr, hdferr)
  !
  ! Get datatype and properties for the datatype.
  !
  CALL h5aget_type_f(attr, dtype, hdferr)
  CALL h5tget_size_f(dtype, len, hdferr)
  CALL h5tget_tag_f(dtype, tag, taglen, hdferr)
  !
  ! Get dataspace and allocate memory for read buffer.
  !
  CALL h5aget_space_f(attr, space, hdferr)
  CALL h5sget_simple_extent_dims_f(space, dims, maxdims, hdferr)
  ALLOCATE(rdata(1:dims(1)))
  !
  ! Read the data.
  !
  f_ptr = C_LOC(rdata(1)(1:1))
  CALL h5aread_f(attr, dtype, f_ptr, hdferr)
  !
  ! Output the data to the screen.
  !
  WRITE(*,'("Datatype tag for ",A," is: ",A)') ATTRIBUTE, '"'//tag(1:taglen)//'"'

  DO i = 1, dims(1)
     WRITE(*,'(A,"[",i1,"]: ",A)') attribute,i-1,rdata(i)
  ENDDO
  !
  ! Close and release resources.
  !
  DEALLOCATE(rdata)
  CALL H5Aclose_f(attr, hdferr)
  CALL H5Dclose_f(dset, hdferr)
  CALL H5Sclose_f(space, hdferr)
  CALL H5Tclose_f(dtype, hdferr)
  CALL H5Fclose_f(file, hdferr)

END PROGRAM main
