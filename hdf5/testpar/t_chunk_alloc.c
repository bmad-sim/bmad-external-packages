/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Copyright by The HDF Group.                                               *
 * All rights reserved.                                                      *
 *                                                                           *
 * This file is part of HDF5.  The full HDF5 copyright notice, including     *
 * terms governing use, modification, and redistribution, is contained in    *
 * the COPYING file, which can be found at the root of the source code       *
 * distribution tree, or in https://www.hdfgroup.org/licenses.               *
 * If you do not have access to either file, you may request a copy from     *
 * help@hdfgroup.org.                                                        *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/*
 * This verifies if the storage space allocation methods are compatible between
 * serial and parallel modes.
 */

#include "testphdf5.h"
static int mpi_size, mpi_rank;

#define DSET_NAME    "ExtendibleArray"
#define CHUNK_SIZE   1000 /* #elements per chunk */
#define CHUNK_FACTOR 200  /* default dataset size in terms of chunks */
#define CLOSE        1
#define NO_CLOSE     0

static MPI_Offset
get_filesize(const char *filename)
{
    int        mpierr;
    MPI_File   fd;
    MPI_Offset filesize;

    mpierr = MPI_File_open(MPI_COMM_SELF, filename, MPI_MODE_RDONLY, MPI_INFO_NULL, &fd);
    VRFY((mpierr == MPI_SUCCESS), "");

    mpierr = MPI_File_get_size(fd, &filesize);
    VRFY((mpierr == MPI_SUCCESS), "");

    mpierr = MPI_File_close(&fd);
    VRFY((mpierr == MPI_SUCCESS), "");

    return (filesize);
}

typedef enum write_pattern { none, sec_last, all } write_type;

typedef enum access_ { write_all, open_only, extend_only } access_type;

/*
 * This creates a dataset serially with chunks, each of CHUNK_SIZE
 * elements. The allocation time is set to H5D_ALLOC_TIME_EARLY. Another
 * routine will open this in parallel for extension test.
 */
static void
create_chunked_dataset(const char *filename, int chunk_factor, write_type write_pattern)
{
    hid_t   file_id, dataset; /* handles */
    hid_t   dataspace, memspace;
    hid_t   cparms;
    hsize_t dims[1];
    hsize_t maxdims[1] = {H5S_UNLIMITED};

    hsize_t chunk_dims[1] = {CHUNK_SIZE};
    hsize_t count[1];
    hsize_t stride[1];
    hsize_t block[1];
    hsize_t offset[1]; /* Selection offset within dataspace */
    /* Variables used in reading data back */
    char   buffer[CHUNK_SIZE];
    long   nchunks;
    herr_t hrc;

    MPI_Offset filesize, /* actual file size */
        est_filesize;    /* estimated file size */

    /* set up MPI parameters */
    MPI_Comm_size(MPI_COMM_WORLD, &mpi_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &mpi_rank);

    /* Only MAINPROCESS should create the file.  Others just wait. */
    if (MAINPROCESS) {
        bool vol_is_native;

        nchunks = chunk_factor * mpi_size;
        dims[0] = (hsize_t)(nchunks * CHUNK_SIZE);
        /* Create the data space with unlimited dimensions. */
        dataspace = H5Screate_simple(1, dims, maxdims);
        VRFY((dataspace >= 0), "");

        memspace = H5Screate_simple(1, chunk_dims, NULL);
        VRFY((memspace >= 0), "");

        /* Create a new file. If file exists its contents will be overwritten. */
        file_id = H5Fcreate(filename, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
        VRFY((file_id >= 0), "H5Fcreate");

        /* Check if native VOL is being used */
        VRFY((h5_using_native_vol(H5P_DEFAULT, file_id, &vol_is_native) >= 0), "h5_using_native_vol");

        /* Modify dataset creation properties, i.e. enable chunking  */
        cparms = H5Pcreate(H5P_DATASET_CREATE);
        VRFY((cparms >= 0), "");

        hrc = H5Pset_alloc_time(cparms, H5D_ALLOC_TIME_EARLY);
        VRFY((hrc >= 0), "");

        hrc = H5Pset_chunk(cparms, 1, chunk_dims);
        VRFY((hrc >= 0), "");

        /* Create a new dataset within the file using cparms creation properties. */
        dataset =
            H5Dcreate2(file_id, DSET_NAME, H5T_NATIVE_UCHAR, dataspace, H5P_DEFAULT, cparms, H5P_DEFAULT);
        VRFY((dataset >= 0), "");

        if (write_pattern == sec_last) {
            memset(buffer, 100, CHUNK_SIZE);

            count[0]  = 1;
            stride[0] = 1;
            block[0]  = chunk_dims[0];
            offset[0] = (hsize_t)(nchunks - 2) * chunk_dims[0];

            hrc = H5Sselect_hyperslab(dataspace, H5S_SELECT_SET, offset, stride, count, block);
            VRFY((hrc >= 0), "");

            /* Write sec_last chunk */
            hrc = H5Dwrite(dataset, H5T_NATIVE_UCHAR, memspace, dataspace, H5P_DEFAULT, buffer);
            VRFY((hrc >= 0), "H5Dwrite");
        } /* end if */

        /* Close resources */
        hrc = H5Dclose(dataset);
        VRFY((hrc >= 0), "");
        dataset = -1;

        hrc = H5Sclose(dataspace);
        VRFY((hrc >= 0), "");

        hrc = H5Sclose(memspace);
        VRFY((hrc >= 0), "");

        hrc = H5Pclose(cparms);
        VRFY((hrc >= 0), "");

        hrc = H5Fclose(file_id);
        VRFY((hrc >= 0), "");
        file_id = -1;

        if (vol_is_native) {
            /* verify file size */
            filesize     = get_filesize(filename);
            est_filesize = (MPI_Offset)nchunks * (MPI_Offset)CHUNK_SIZE * (MPI_Offset)sizeof(unsigned char);
            VRFY((filesize >= est_filesize), "file size check");
        }
    }

    /* Make sure all processes are done before exiting this routine.  Otherwise,
     * other tests may start and change the test data file before some processes
     * of this test are still accessing the file.
     */

    MPI_Barrier(MPI_COMM_WORLD);
}

/*
 * This program performs three different types of parallel access. It writes on
 * the entire dataset, it extends the dataset to nchunks*CHUNK_SIZE, and it only
 * opens the dataset. At the end, it verifies the size of the dataset to be
 * consistent with argument 'chunk_factor'.
 */
static void
parallel_access_dataset(const char *filename, int chunk_factor, access_type action, hid_t *file_id,
                        hid_t *dataset)
{
    hid_t   memspace, dataspace; /* HDF5 file identifier */
    hid_t   access_plist;        /* HDF5 ID for file access property list */
    herr_t  hrc;                 /* HDF5 return code */
    hsize_t size[1];

    hsize_t chunk_dims[1] = {CHUNK_SIZE};
    hsize_t count[1];
    hsize_t stride[1];
    hsize_t block[1];
    hsize_t offset[1]; /* Selection offset within dataspace */
    hsize_t dims[1];
    hsize_t maxdims[1];

    /* Variables used in reading data back */
    char buffer[CHUNK_SIZE];
    int  i;
    long nchunks;
    /* MPI Gubbins */
    MPI_Offset filesize, /* actual file size */
        est_filesize;    /* estimated file size */

    bool vol_is_native;

    /* Initialize MPI */
    MPI_Comm_size(MPI_COMM_WORLD, &mpi_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &mpi_rank);

    nchunks = chunk_factor * mpi_size;

    /* Set up MPIO file access property lists */
    access_plist = H5Pcreate(H5P_FILE_ACCESS);
    VRFY((access_plist >= 0), "");

    hrc = H5Pset_fapl_mpio(access_plist, MPI_COMM_WORLD, MPI_INFO_NULL);
    VRFY((hrc >= 0), "");

    /* Open the file */
    if (*file_id < 0) {
        *file_id = H5Fopen(filename, H5F_ACC_RDWR, access_plist);
        VRFY((*file_id >= 0), "");
    }

    /* Check if native VOL is being used */
    VRFY((h5_using_native_vol(H5P_DEFAULT, *file_id, &vol_is_native) >= 0), "h5_using_native_vol");

    /* Open dataset*/
    if (*dataset < 0) {
        *dataset = H5Dopen2(*file_id, DSET_NAME, H5P_DEFAULT);
        VRFY((*dataset >= 0), "");
    }

    /* Make sure all processes are done before continuing.  Otherwise, one
     * process could change the dataset extent before another finishes opening
     * it, resulting in only some of the processes calling H5Dset_extent(). */
    MPI_Barrier(MPI_COMM_WORLD);

    memspace = H5Screate_simple(1, chunk_dims, NULL);
    VRFY((memspace >= 0), "");

    dataspace = H5Dget_space(*dataset);
    VRFY((dataspace >= 0), "");

    size[0] = (hsize_t)nchunks * CHUNK_SIZE;

    switch (action) {

        /* all chunks are written by all the processes in an interleaved way*/
        case write_all:

            memset(buffer, mpi_rank + 1, CHUNK_SIZE);
            count[0]  = 1;
            stride[0] = 1;
            block[0]  = chunk_dims[0];
            for (i = 0; i < nchunks / mpi_size; i++) {
                offset[0] = (hsize_t)(i * mpi_size + mpi_rank) * chunk_dims[0];

                hrc = H5Sselect_hyperslab(dataspace, H5S_SELECT_SET, offset, stride, count, block);
                VRFY((hrc >= 0), "");

                /* Write the buffer out */
                hrc = H5Dwrite(*dataset, H5T_NATIVE_UCHAR, memspace, dataspace, H5P_DEFAULT, buffer);
                VRFY((hrc >= 0), "H5Dwrite");
            }

            break;

        /* only extends the dataset */
        case extend_only:
            /* check if new size is larger than old size */
            hrc = H5Sget_simple_extent_dims(dataspace, dims, maxdims);
            VRFY((hrc >= 0), "");

            /* Extend dataset*/
            if (size[0] > dims[0]) {
                hrc = H5Dset_extent(*dataset, size);
                VRFY((hrc >= 0), "");
            }
            break;

        /* only opens the *dataset */
        case open_only:
            break;
        default:
            assert(0);
    }

    /* Close up */
    hrc = H5Dclose(*dataset);
    VRFY((hrc >= 0), "");
    *dataset = -1;

    hrc = H5Sclose(dataspace);
    VRFY((hrc >= 0), "");

    hrc = H5Sclose(memspace);
    VRFY((hrc >= 0), "");

    hrc = H5Fclose(*file_id);
    VRFY((hrc >= 0), "");
    *file_id = -1;

    if (vol_is_native) {
        /* verify file size */
        filesize     = get_filesize(filename);
        est_filesize = (MPI_Offset)nchunks * (MPI_Offset)CHUNK_SIZE * (MPI_Offset)sizeof(unsigned char);
        VRFY((filesize >= est_filesize), "file size check");
    }

    /* Can close some plists */
    hrc = H5Pclose(access_plist);
    VRFY((hrc >= 0), "");

    /* Make sure all processes are done before exiting this routine.  Otherwise,
     * other tests may start and change the test data file before some processes
     * of this test are still accessing the file.
     */
    MPI_Barrier(MPI_COMM_WORLD);
}

/*
 * This routine verifies the data written in the dataset. It does one of the
 * three cases according to the value of parameter `write_pattern'.
 * 1. it returns correct fill values though the dataset has not been written;
 * 2. it still returns correct fill values though only a small part is written;
 * 3. it returns correct values when the whole dataset has been written in an
 *    interleaved pattern.
 */
static void
verify_data(const char *filename, int chunk_factor, write_type write_pattern, int vclose, hid_t *file_id,
            hid_t *dataset)
{
    hid_t  dataspace, memspace; /* HDF5 file identifier */
    hid_t  access_plist;        /* HDF5 ID for file access property list */
    herr_t hrc;                 /* HDF5 return code */

    hsize_t chunk_dims[1] = {CHUNK_SIZE};
    hsize_t count[1];
    hsize_t stride[1];
    hsize_t block[1];
    hsize_t offset[1]; /* Selection offset within dataspace */
    /* Variables used in reading data back */
    char buffer[CHUNK_SIZE];
    int  value, i;
    int  index_l;
    long nchunks;
    /* Initialize MPI */
    MPI_Comm_size(MPI_COMM_WORLD, &mpi_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &mpi_rank);

    nchunks = chunk_factor * mpi_size;

    /* Set up MPIO file access property lists */
    access_plist = H5Pcreate(H5P_FILE_ACCESS);
    VRFY((access_plist >= 0), "");

    hrc = H5Pset_fapl_mpio(access_plist, MPI_COMM_WORLD, MPI_INFO_NULL);
    VRFY((hrc >= 0), "");

    /* Open the file */
    if (*file_id < 0) {
        *file_id = H5Fopen(filename, H5F_ACC_RDWR, access_plist);
        VRFY((*file_id >= 0), "");
    }

    /* Open dataset*/
    if (*dataset < 0) {
        *dataset = H5Dopen2(*file_id, DSET_NAME, H5P_DEFAULT);
        VRFY((*dataset >= 0), "");
    }

    memspace = H5Screate_simple(1, chunk_dims, NULL);
    VRFY((memspace >= 0), "");

    dataspace = H5Dget_space(*dataset);
    VRFY((dataspace >= 0), "");

    /* all processes check all chunks. */
    count[0]  = 1;
    stride[0] = 1;
    block[0]  = chunk_dims[0];
    for (i = 0; i < nchunks; i++) {
        /* reset buffer values */
        memset(buffer, -1, CHUNK_SIZE);

        offset[0] = (hsize_t)i * chunk_dims[0];

        hrc = H5Sselect_hyperslab(dataspace, H5S_SELECT_SET, offset, stride, count, block);
        VRFY((hrc >= 0), "");

        /* Read the chunk */
        hrc = H5Dread(*dataset, H5T_NATIVE_UCHAR, memspace, dataspace, H5P_DEFAULT, buffer);
        VRFY((hrc >= 0), "H5Dread");

        /* set expected value according the write pattern */
        switch (write_pattern) {
            case all:
                value = i % mpi_size + 1;
                break;
            case none:
                value = 0;
                break;
            case sec_last:
                if (i == nchunks - 2)
                    value = 100;
                else
                    value = 0;
                break;
            default:
                assert(0);
        }

        /* verify content of the chunk */
        for (index_l = 0; index_l < CHUNK_SIZE; index_l++)
            VRFY((buffer[index_l] == value), "data verification");
    }

    hrc = H5Sclose(dataspace);
    VRFY((hrc >= 0), "");

    hrc = H5Sclose(memspace);
    VRFY((hrc >= 0), "");

    /* Can close some plists */
    hrc = H5Pclose(access_plist);
    VRFY((hrc >= 0), "");

    /* Close up */
    if (vclose) {
        hrc = H5Dclose(*dataset);
        VRFY((hrc >= 0), "");
        *dataset = -1;

        hrc = H5Fclose(*file_id);
        VRFY((hrc >= 0), "");
        *file_id = -1;
    }

    /* Make sure all processes are done before exiting this routine.  Otherwise,
     * other tests may start and change the test data file before some processes
     * of this test are still accessing the file.
     */
    MPI_Barrier(MPI_COMM_WORLD);
}

/*
 * Test following possible scenarios,
 * Case 1:
 * Sequential create a file and dataset with H5D_ALLOC_TIME_EARLY and large
 * size, no write, close, reopen in parallel, read to verify all return
 * the fill value.
 * Case 2:
 * Sequential create a file and dataset with H5D_ALLOC_TIME_EARLY but small
 * size, no write, close, reopen in parallel, extend to large size, then close,
 * then reopen in parallel and read to verify all return the fill value.
 * Case 3:
 * Sequential create a file and dataset with H5D_ALLOC_TIME_EARLY and large
 * size, write just a small part of the dataset (second to the last), close,
 * then reopen in parallel, read to verify all return the fill value except
 * those small portion that has been written.  Without closing it, writes
 * all parts of the dataset in a interleave pattern, close it, and reopen
 * it, read to verify all data are as written.
 */
void
test_chunk_alloc(void)
{
    const char *filename;
    hid_t       file_id, dataset;

    file_id = dataset = -1;

    /* Initialize MPI */
    MPI_Comm_size(MPI_COMM_WORLD, &mpi_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &mpi_rank);

    /* Make sure the connector supports the API functions being tested */
    if (!(vol_cap_flags_g & H5VL_CAP_FLAG_FILE_BASIC) || !(vol_cap_flags_g & H5VL_CAP_FLAG_DATASET_BASIC) ||
        !(vol_cap_flags_g & H5VL_CAP_FLAG_DATASET_MORE)) {
        if (MAINPROCESS) {
            puts("SKIPPED");
            printf("    API functions for basic file, dataset, or dataset more aren't supported with this "
                   "connector\n");
            fflush(stdout);
        }

        return;
    }

    filename = (const char *)GetTestParameters();
    if (VERBOSE_MED)
        printf("Extend Chunked allocation test on file %s\n", filename);

    /* Case 1 */
    /* Create chunked dataset without writing anything.*/
    create_chunked_dataset(filename, CHUNK_FACTOR, none);
    /* reopen dataset in parallel and check for file size */
    parallel_access_dataset(filename, CHUNK_FACTOR, open_only, &file_id, &dataset);
    /* reopen dataset in parallel, read and verify the data */
    verify_data(filename, CHUNK_FACTOR, none, CLOSE, &file_id, &dataset);

    /* Case 2 */
    /* Create chunked dataset without writing anything */
    create_chunked_dataset(filename, 20, none);
    /* reopen dataset in parallel and only extend it */
    parallel_access_dataset(filename, CHUNK_FACTOR, extend_only, &file_id, &dataset);
    /* reopen dataset in parallel, read and verify the data */
    verify_data(filename, CHUNK_FACTOR, none, CLOSE, &file_id, &dataset);

    /* Case 3 */
    /* Create chunked dataset and write in the second to last chunk */
    create_chunked_dataset(filename, CHUNK_FACTOR, sec_last);
    /* Reopen dataset in parallel, read and verify the data. The file and dataset are not closed*/
    verify_data(filename, CHUNK_FACTOR, sec_last, NO_CLOSE, &file_id, &dataset);
    /* All processes write in all the chunks in a interleaved way */
    parallel_access_dataset(filename, CHUNK_FACTOR, write_all, &file_id, &dataset);
    /* reopen dataset in parallel, read and verify the data */
    verify_data(filename, CHUNK_FACTOR, all, CLOSE, &file_id, &dataset);
}

/*
 * A test to verify the following:
 *
 *   - That the library forces allocation of all space in the file
 *     for a chunked dataset opened with parallel file access when
 *     that dataset:
 *
 *       - was created with serial file access
 *       - was created with the default incremental file space
 *         allocation time
 *       - has no filters applied to it
 *
 *     In this case, the library has to ensure that all the file
 *     space for the dataset is allocated so that the MPI processes
 *     can write to chunks independently of each other and still have
 *     a consistent view of the file.
 *
 *   - That the library DOES NOT force allocation of all space in
 *     the file for a chunked dataset opened with parallel file access
 *     when that dataset:
 *
 *       - was created with serial file access
 *       - was created with the default incremental file space
 *         allocation time
 *       - has filters applied to it
 *
 *     In this case, writes to the dataset are required to be collective,
 *     so file space can be allocated incrementally in a coordinated
 *     fashion.
 */
void
test_chunk_alloc_incr_ser_to_par(void)
{
    H5D_space_status_t space_status;
    const char        *filename;
    hsize_t            dset_dims[1];
    hsize_t            start[1];
    hsize_t            stride[1];
    hsize_t            count[1];
    hsize_t            block[1];
    hsize_t            alloc_size;
    size_t             nchunks;
    herr_t             ret;
    hid_t              fid          = H5I_INVALID_HID;
    hid_t              fapl_id      = H5I_INVALID_HID;
    hid_t              dset_id      = H5I_INVALID_HID;
    hid_t              fspace_id    = H5I_INVALID_HID;
    hid_t              dxpl_id      = H5I_INVALID_HID;
    int               *data         = NULL;
    int               *correct_data = NULL;
    int               *read_data    = NULL;
    bool               vol_is_native;

    MPI_Comm_size(MPI_COMM_WORLD, &mpi_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &mpi_rank);

    filename = (const char *)GetTestParameters();
    if (MAINPROCESS && VERBOSE_MED)
        printf("Chunked dataset incremental file space allocation serial to parallel test on file %s\n",
               filename);

    nchunks      = (size_t)(CHUNK_FACTOR * mpi_size);
    dset_dims[0] = (hsize_t)(nchunks * CHUNK_SIZE);

    if (mpi_rank == 0) {
        hsize_t chunk_dims[1] = {CHUNK_SIZE};
        hid_t   space_id      = H5I_INVALID_HID;
        hid_t   dcpl_id       = H5I_INVALID_HID;

        fid = H5Fcreate(filename, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
        VRFY((fid >= 0), "H5Fcreate");

        dcpl_id = H5Pcreate(H5P_DATASET_CREATE);
        VRFY((dcpl_id >= 0), "H5Pcreate");

        ret = H5Pset_chunk(dcpl_id, 1, chunk_dims);
        VRFY((ret == SUCCEED), "H5Pset_chunk");

        ret = H5Pset_alloc_time(dcpl_id, H5D_ALLOC_TIME_INCR);
        VRFY((ret == SUCCEED), "H5Pset_alloc_time");

        space_id = H5Screate_simple(1, dset_dims, NULL);
        VRFY((space_id >= 0), "H5Screate_simple");

        /* Create a chunked dataset without a filter applied to it */
        dset_id =
            H5Dcreate2(fid, "dset_no_filter", H5T_NATIVE_INT, space_id, H5P_DEFAULT, dcpl_id, H5P_DEFAULT);
        VRFY((dset_id >= 0), "H5Dcreate2");

        ret = H5Dclose(dset_id);
        VRFY((ret == SUCCEED), "H5Dclose");

        /* Create a chunked dataset with a filter applied to it */
        ret = H5Pset_shuffle(dcpl_id);
        VRFY((ret == SUCCEED), "H5Pset_shuffle");

        dset_id = H5Dcreate2(fid, "dset_filter", H5T_NATIVE_INT, space_id, H5P_DEFAULT, dcpl_id, H5P_DEFAULT);
        VRFY((dset_id >= 0), "H5Dcreate2");

        ret = H5Dclose(dset_id);
        VRFY((ret == SUCCEED), "H5Dclose");
        ret = H5Pclose(dcpl_id);
        VRFY((ret == SUCCEED), "H5Pclose");
        ret = H5Sclose(space_id);
        VRFY((ret == SUCCEED), "H5Sclose");
        ret = H5Fclose(fid);
        VRFY((ret == SUCCEED), "H5Fclose");
    }

    MPI_Barrier(MPI_COMM_WORLD);

    fapl_id = H5Pcreate(H5P_FILE_ACCESS);
    VRFY((fapl_id >= 0), "H5Pcreate");

    ret = H5Pset_fapl_mpio(fapl_id, MPI_COMM_WORLD, MPI_INFO_NULL);
    VRFY((ret == SUCCEED), "H5Pset_fapl_mpio");

    fid = H5Fopen(filename, H5F_ACC_RDWR, fapl_id);
    VRFY((fid >= 0), "H5Fopen");

    /* Check if native VOL is being used */
    VRFY((h5_using_native_vol(H5P_DEFAULT, fid, &vol_is_native) >= 0), "h5_using_native_vol");

    data = malloc((dset_dims[0] / (hsize_t)mpi_size) * sizeof(int));
    VRFY(data, "malloc");
    read_data = malloc(dset_dims[0] * sizeof(int));
    VRFY(read_data, "malloc");
    correct_data = malloc(dset_dims[0] * sizeof(int));
    VRFY(correct_data, "malloc");

    /*
     * Check the file space allocation status/size and dataset
     * data before and after writing to the dataset without a
     * filter
     */
    dset_id = H5Dopen2(fid, "dset_no_filter", H5P_DEFAULT);
    VRFY((dset_id >= 0), "H5Dopen2");

    if (vol_is_native) {
        ret = H5Dget_space_status(dset_id, &space_status);
        VRFY((ret == SUCCEED), "H5Dread");

        VRFY((space_status == H5D_SPACE_STATUS_ALLOCATED),
             "file space allocation status verification succeeded");

        alloc_size = H5Dget_storage_size(dset_id);
        VRFY(((dset_dims[0] * sizeof(int)) == alloc_size),
             "file space allocation size verification succeeded");
    }

    memset(read_data, 255, dset_dims[0] * sizeof(int));
    memset(correct_data, 0, dset_dims[0] * sizeof(int));

    ret = H5Dread(dset_id, H5T_NATIVE_INT, H5S_BLOCK, H5S_ALL, H5P_DEFAULT, read_data);
    VRFY((ret == SUCCEED), "H5Dread");

    MPI_Barrier(MPI_COMM_WORLD);

    VRFY((0 == memcmp(read_data, correct_data, dset_dims[0] * sizeof(int))), "data verification succeeded");

    fspace_id = H5Dget_space(dset_id);
    VRFY((ret == SUCCEED), "H5Dget_space");

    start[0]  = ((hsize_t)mpi_rank * (dset_dims[0] / (hsize_t)mpi_size));
    stride[0] = 1;
    count[0]  = (dset_dims[0] / (hsize_t)mpi_size);
    block[0]  = 1;

    ret = H5Sselect_hyperslab(fspace_id, H5S_SELECT_SET, start, stride, count, block);
    VRFY((ret == SUCCEED), "H5Sselect_hyperslab");

    memset(data, 255, (dset_dims[0] / (hsize_t)mpi_size) * sizeof(int));

    ret = H5Dwrite(dset_id, H5T_NATIVE_INT, H5S_BLOCK, fspace_id, H5P_DEFAULT, data);
    VRFY((ret == SUCCEED), "H5Dwrite");

    MPI_Barrier(MPI_COMM_WORLD);

    if (vol_is_native) {
        ret = H5Dget_space_status(dset_id, &space_status);
        VRFY((ret == SUCCEED), "H5Dread");

        VRFY((space_status == H5D_SPACE_STATUS_ALLOCATED),
             "file space allocation status verification succeeded");

        alloc_size = H5Dget_storage_size(dset_id);
        VRFY(((dset_dims[0] * sizeof(int)) == alloc_size),
             "file space allocation size verification succeeded");
    }

    memset(read_data, 0, dset_dims[0] * sizeof(int));
    memset(correct_data, 255, dset_dims[0] * sizeof(int));

    ret = H5Dread(dset_id, H5T_NATIVE_INT, H5S_BLOCK, H5S_ALL, H5P_DEFAULT, read_data);
    VRFY((ret == SUCCEED), "H5Dread");

    MPI_Barrier(MPI_COMM_WORLD);

    VRFY((0 == memcmp(read_data, correct_data, dset_dims[0] * sizeof(int))), "data verification succeeded");

    ret = H5Sclose(fspace_id);
    VRFY((ret == SUCCEED), "H5Sclose");
    ret = H5Dclose(dset_id);
    VRFY((ret == SUCCEED), "H5Dclose");

    /*
     * Check the file space allocation status/size and dataset
     * data before and after writing to the dataset with a
     * filter
     */
    dset_id = H5Dopen2(fid, "dset_filter", H5P_DEFAULT);
    VRFY((dset_id >= 0), "H5Dopen2");

    if (vol_is_native) {
        ret = H5Dget_space_status(dset_id, &space_status);
        VRFY((ret == SUCCEED), "H5Dread");

        VRFY((space_status == H5D_SPACE_STATUS_NOT_ALLOCATED),
             "file space allocation status verification succeeded");

        alloc_size = H5Dget_storage_size(dset_id);
        VRFY((0 == alloc_size), "file space allocation size verification succeeded");
    }

    memset(read_data, 255, dset_dims[0] * sizeof(int));
    memset(correct_data, 0, dset_dims[0] * sizeof(int));

    ret = H5Dread(dset_id, H5T_NATIVE_INT, H5S_BLOCK, H5S_ALL, H5P_DEFAULT, read_data);
    VRFY((ret == SUCCEED), "H5Dread");

    MPI_Barrier(MPI_COMM_WORLD);

    VRFY((0 == memcmp(read_data, correct_data, dset_dims[0] * sizeof(int))), "data verification succeeded");

    fspace_id = H5Dget_space(dset_id);
    VRFY((ret == SUCCEED), "H5Dget_space");

    start[0]  = ((hsize_t)mpi_rank * (dset_dims[0] / (hsize_t)mpi_size));
    stride[0] = 1;
    count[0]  = (dset_dims[0] / (hsize_t)mpi_size);
    block[0]  = 1;

    ret = H5Sselect_hyperslab(fspace_id, H5S_SELECT_SET, start, stride, count, block);
    VRFY((ret == SUCCEED), "H5Sselect_hyperslab");

    memset(data, 255, (dset_dims[0] / (hsize_t)mpi_size) * sizeof(int));

    dxpl_id = H5Pcreate(H5P_DATASET_XFER);
    VRFY((dxpl_id >= 0), "H5Pcreate");

    ret = H5Pset_dxpl_mpio(dxpl_id, H5FD_MPIO_COLLECTIVE);
    VRFY((ret == SUCCEED), "H5Pset_dxpl_mpio");

    ret = H5Dwrite(dset_id, H5T_NATIVE_INT, H5S_BLOCK, fspace_id, dxpl_id, data);
    VRFY((ret == SUCCEED), "H5Dwrite");

    MPI_Barrier(MPI_COMM_WORLD);

    if (vol_is_native) {
        ret = H5Dget_space_status(dset_id, &space_status);
        VRFY((ret == SUCCEED), "H5Dread");

        VRFY((space_status == H5D_SPACE_STATUS_ALLOCATED),
             "file space allocation status verification succeeded");

        alloc_size = H5Dget_storage_size(dset_id);
        VRFY(((dset_dims[0] * sizeof(int)) == alloc_size),
             "file space allocation size verification succeeded");
    }

    memset(read_data, 0, dset_dims[0] * sizeof(int));
    memset(correct_data, 255, dset_dims[0] * sizeof(int));

    ret = H5Dread(dset_id, H5T_NATIVE_INT, H5S_BLOCK, H5S_ALL, H5P_DEFAULT, read_data);
    VRFY((ret == SUCCEED), "H5Dread");

    MPI_Barrier(MPI_COMM_WORLD);

    VRFY((0 == memcmp(read_data, correct_data, dset_dims[0] * sizeof(int))), "data verification succeeded");

    ret = H5Pclose(dxpl_id);
    VRFY((ret == SUCCEED), "H5Pclose");
    ret = H5Sclose(fspace_id);
    VRFY((ret == SUCCEED), "H5Sclose");
    ret = H5Dclose(dset_id);
    VRFY((ret == SUCCEED), "H5Dclose");

    free(correct_data);
    free(read_data);
    free(data);

    H5Pclose(fapl_id);
    H5Fclose(fid);
}
