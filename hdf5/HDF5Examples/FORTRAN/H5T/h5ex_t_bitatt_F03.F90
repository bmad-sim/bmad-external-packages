!************************************************************
!
!  This example shows how to read and write bitfield
!  datatypes to an attribute.  The program first writes bit
!  fields to an attribute with a dataspace of DIM0xDIM1, then
!  closes the file.  Next, it reopens the file, reads back
!  the data, and outputs it to the screen.
!
!  This file is intended for use with HDF5 Library version 1.8
!  with --enable-fortran2003
!
!************************************************************
PROGRAM main

  USE HDF5
  USE ISO_C_BINDING
  
  IMPLICIT NONE

  CHARACTER(LEN=20), PARAMETER :: filename  = "h5ex_t_bitatt_F03.h5"
  CHARACTER(LEN=3) , PARAMETER :: dataset   = "DS1"
  CHARACTER(LEN=2) , PARAMETER :: attribute = "A1"
  INTEGER          , PARAMETER :: dim0      = 4
  INTEGER          , PARAMETER :: dim1      = 7

  INTEGER(HID_T)  :: file, space, dset, attr ! Handles
  INTEGER :: hdferr
  INTEGER(HSIZE_T), DIMENSION(1:2)   :: dims = (/dim0, dim1/)
  INTEGER(HSIZE_T), DIMENSION(1:2)   :: maxdims
  INTEGER(C_SIGNED_CHAR), DIMENSION(1:dim0, 1:dim1), TARGET :: wdata              ! Write buffer 
  INTEGER(C_SIGNED_CHAR), DIMENSION(:,:), ALLOCATABLE, TARGET :: rdata    ! Read buffer
  INTEGER :: A, B, C, D
  INTEGER(C_SIGNED_CHAR) :: i, j
  INTEGER(C_SIGNED_CHAR) :: hex
  TYPE(C_PTR) :: f_ptr
  DATA hex /Z'03'/
  !
  ! Initialize FORTRAN interface.
  !
  CALL h5open_f(hdferr)
  !
  ! Initialize data.  We will manually pack 4 2-bit integers into
  ! each unsigned char data element.
  !
  DO i = 0, dim0-1
     DO j = 0, dim1-1
        wdata(i+1,j+1) = 0
        wdata(i+1,j+1) = IOR( wdata(i+1,j+1), IAND(i * j - j, hex))   ! Field "A"
        wdata(i+1,j+1) = IOR( wdata(i+1,j+1), ISHFT(IAND(i,hex),2))   ! Field "B"
        wdata(i+1,j+1) = IOR( wdata(i+1,j+1), ISHFT(IAND(j,hex),4))   ! Field "C"
        wdata(i+1,j+1) = IOR( wdata(i+1,j+1), ISHFT(IAND(i+j,hex),6)) ! Field "D"
     ENDDO
  ENDDO
  !
  ! Create a new file using the default properties.
  !
  CALL h5fcreate_f(filename, H5F_ACC_TRUNC_F, file, hdferr)
  !
  ! Create dataset with a null dataspace.
  !
  CALL H5Screate_f( H5S_NULL_F, space, hdferr)
  CALL h5dcreate_f(file, dataset, H5T_STD_I32LE, space, dset, hdferr)
  CALL H5Sclose_f(space, hdferr)
  !
  ! Create dataspace.  Setting maximum size to be the current size.
  !
  CALL h5screate_simple_f(2, dims, space, hdferr)
  !
  ! Create the attribute and write the bitfield data to it.
  !
  CALL H5Acreate_f(dset, attribute,  H5T_STD_B8BE, space, attr, hdferr)
  f_ptr = C_LOC(wdata(1,1))
  CALL H5Awrite_f(attr, H5T_NATIVE_B8, f_ptr, hdferr)
  !
  ! Close and release resources.
  !
  CALL H5Aclose_f(attr, hdferr)
  CALL H5Dclose_f(dset, hdferr)
  CALL H5Sclose_f(space, hdferr)
  CALL H5Fclose_f(file, hdferr)
  !
  ! Now we begin the read section of this example. 
  !
  ! Open file, dataset, and attribute.
  !
  CALL h5fopen_f(filename, H5F_ACC_RDONLY_F, file, hdferr)
  CALL h5dopen_f(file, dataset, dset, hdferr)
  CALL h5aopen_f(dset, attribute, attr, hdferr)
  !
  ! Get dataspace and allocate memory for read buffer.
  !
  CALL H5Aget_space_f(attr, space, hdferr)
  CALL H5Sget_simple_extent_dims_f(space, dims, maxdims, hdferr)
  ALLOCATE(rdata(1:dims(1),1:dims(2)))
  !
  ! Read the data.
  !
  f_ptr = C_LOC(rdata)
  CALL H5Aread_f(attr,  H5T_NATIVE_B8, f_ptr, hdferr)
  !
  ! Output the data to the screen.
  !
  WRITE(*,'(A,":")') attribute
  DO i = 1, dims(1)
     WRITE(*,'(" [ ")', ADVANCE='NO')
     DO j = 1, dims(2)
        A = IAND(rdata(i,j), hex) ! Retrieve field "A"
        B = IAND(ISHFT(rdata(i,j),-2), hex) ! Retrieve field "B"
        C = IAND(ISHFT(rdata(i,j),-4), hex) ! Retrieve field "C"
        D = IAND(ISHFT(rdata(i,j),-6), hex) ! Retrieve field "D"
        WRITE(*,'(A1,4I2,A2)', ADVANCE='NO') "{",A, B, C, D,"} "
     ENDDO
     WRITE(*,'(A1)') "]"
  ENDDO
  !
  ! Close and release resources.
  !
  DEALLOCATE(rdata)
  CALL H5Aclose_f(attr, hdferr)
  CALL H5Dclose_f(dset, hdferr)
  CALL H5Sclose_f(space, hdferr)
  CALL H5Fclose_f(file, hdferr)

END PROGRAM main
