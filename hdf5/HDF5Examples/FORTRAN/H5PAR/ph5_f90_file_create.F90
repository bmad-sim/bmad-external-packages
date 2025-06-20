!
! This example creates HDF5 file in a parallel environment
!

     PROGRAM FILE_CREATE

     USE HDF5 ! This module contains all necessary modules
     USE MPI

     IMPLICIT NONE

     CHARACTER(LEN=10), PARAMETER :: filename = "sds.h5"  ! File name

     INTEGER(HID_T) :: file_id       ! File identifier
     INTEGER(HID_T) :: plist_id      ! Property list identifier
     INTEGER        :: error

     !
     ! MPI definitions and calls.
     !
     INTEGER(KIND=MPI_INTEGER_KIND) :: mpierror       ! MPI error flag
     INTEGER(KIND=MPI_INTEGER_KIND) :: comm, info
     INTEGER(KIND=MPI_INTEGER_KIND) :: mpi_size, mpi_rank
     comm = MPI_COMM_WORLD
     info = MPI_INFO_NULL

     CALL MPI_INIT(mpierror)
     CALL MPI_COMM_SIZE(comm, mpi_size, mpierror)
     CALL MPI_COMM_RANK(comm, mpi_rank, mpierror)
     !
     ! Initialize FORTRAN predefined datatypes
     !
     CALL h5open_f(error)

     !
     ! Setup file access property list with parallel I/O access.
     !
     CALL h5pcreate_f(H5P_FILE_ACCESS_F, plist_id, error)
     CALL h5pset_fapl_mpio_f(plist_id, comm, info, error)

     !
     ! Create the file collectively.
     !
     CALL h5fcreate_f(filename, H5F_ACC_TRUNC_F, file_id, error, access_prp = plist_id)

     !
     ! Close property list and the file.
     !
     CALL h5pclose_f(plist_id, error)
     CALL h5fclose_f(file_id, error)

     !
     ! Close FORTRAN interface
     !
     CALL h5close_f(error)
     IF(mpi_rank.EQ.0) WRITE(*,'(A)') "PHDF5 example finished with no errors"
     CALL MPI_FINALIZE(mpierror)

     END PROGRAM FILE_CREATE
