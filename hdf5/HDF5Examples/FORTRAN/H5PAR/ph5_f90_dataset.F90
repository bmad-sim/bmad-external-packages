     PROGRAM DATASET

     USE HDF5 ! This module contains all necessary modules
     USE MPI

     IMPLICIT NONE
     CHARACTER(LEN=10), PARAMETER :: filename = "sds.h5"  ! File name
     CHARACTER(LEN=8), PARAMETER :: dsetname = "IntArray" ! Dataset name

     INTEGER(HID_T) :: file_id       ! File identifier
     INTEGER(HID_T) :: dset_id       ! Dataset identifier
     INTEGER(HID_T) :: filespace     ! Dataspace identifier in file
     INTEGER(HID_T) :: plist_id      ! Property list identifier

     INTEGER(HSIZE_T), DIMENSION(2) :: dimsf = (/5,8/) ! Dataset dimensions.
!     INTEGER, DIMENSION(7) :: dimsfi = (/5,8,0,0,0,0,0/)
!     INTEGER(HSIZE_T), DIMENSION(2) :: dimsfi = (/5,8/)
     INTEGER(HSIZE_T), DIMENSION(2) :: dimsfi

     INTEGER, ALLOCATABLE :: data(:,:)   ! Data to write
     INTEGER :: rank = 2 ! Dataset rank

     INTEGER :: error, error_n  ! Error flags
     INTEGER :: i, j
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
     ! Initialize data buffer with trivial data.
     !
     ALLOCATE ( data(dimsf(1),dimsf(2)))
     do i = 1, dimsf(2)
     do j = 1, dimsf(1)
        data(j,i) = j - 1 + (i-1)*dimsf(1)
     enddo
     enddo
     !
     ! Initialize FORTRAN interface
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
     CALL h5pclose_f(plist_id, error)
     !
     ! Create the data space for the  dataset.
     !
     CALL h5screate_simple_f(rank, dimsf, filespace, error)

     !
     ! Create the dataset with default properties.
     !
     CALL h5dcreate_f(file_id, dsetname, H5T_NATIVE_INTEGER, filespace, &
                      dset_id, error)
     !
     ! Create property list for collective dataset write
     !
     CALL h5pcreate_f(H5P_DATASET_XFER_F, plist_id, error)
     CALL h5pset_dxpl_mpio_f(plist_id, H5FD_MPIO_COLLECTIVE_F, error)
     !
     ! For independent write use
     ! CALL h5pset_dxpl_mpio_f(plist_id, H5FD_MPIO_INDEPENDENT_F, error)
     !

     !
     ! Write the dataset collectively.
     !
     CALL h5dwrite_f(dset_id, H5T_NATIVE_INTEGER, data, dimsfi, error, &
                      xfer_prp = plist_id)
     !
     ! Deallocate data buffer.
     !
     DEALLOCATE(data)

     !
     ! Close resources.
     !
     CALL h5sclose_f(filespace, error)
     CALL h5dclose_f(dset_id, error)
     CALL h5pclose_f(plist_id, error)
     CALL h5fclose_f(file_id, error)

     !
     ! Close FORTRAN interface
     !
     CALL h5close_f(error)
     IF(mpi_rank.EQ.0) WRITE(*,'(A)') "PHDF5 example finished with no errors"

     CALL MPI_FINALIZE(mpierror)

     END PROGRAM DATASET
