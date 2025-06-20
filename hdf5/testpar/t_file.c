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
 * Parallel tests for file operations
 */

#include "testphdf5.h"

#include "H5CXprivate.h" /* API Contexts                         */
#include "H5Iprivate.h"
#include "H5PBprivate.h"

/*
 * This file needs to access private information from the H5F package.
 */
#define H5AC_FRIEND /*suppress error about including H5ACpkg      */
#include "H5ACpkg.h"
#define H5C_FRIEND /*suppress error about including H5Cpkg      */
#include "H5Cpkg.h"
#define H5F_FRIEND /*suppress error about including H5Fpkg      */
#define H5F_TESTING
#include "H5Fpkg.h"
#define H5MF_FRIEND /*suppress error about including H5MFpkg      */
#include "H5MFpkg.h"

#ifdef PB_OUT
#define NUM_DSETS 5
#endif

int mpi_size, mpi_rank;

#ifdef PB_OUT
static int create_file(const char *filename, hid_t fcpl, hid_t fapl, int metadata_write_strategy);
static int open_file(const char *filename, hid_t fapl, int metadata_write_strategy, hsize_t page_size,
                     size_t page_buffer_size);
#endif

/*
 * test file access by communicator besides COMM_WORLD.
 * Split COMM_WORLD into two, one (even_comm) contains the original
 * processes of even ranks.  The other (odd_comm) contains the original
 * processes of odd ranks.  Processes in even_comm creates a file, then
 * cloose it, using even_comm.  Processes in old_comm just do a barrier
 * using odd_comm.  Then they all do a barrier using COMM_WORLD.
 * If the file creation and cloose does not do correct collective action
 * according to the communicator argument, the processes will freeze up
 * sooner or later due to barrier mixed up.
 */
void
test_split_comm_access(void)
{
    MPI_Comm    comm;
    MPI_Info    info = MPI_INFO_NULL;
    int         is_old, mrc;
    int         newrank, newprocs;
    hid_t       fid;     /* file IDs */
    hid_t       acc_tpl; /* File access properties */
    herr_t      ret;     /* generic return value */
    const char *filename;

    filename = (const char *)GetTestParameters();
    if (VERBOSE_MED)
        printf("Split Communicator access test on file %s\n", filename);

    /* set up MPI parameters */
    MPI_Comm_size(MPI_COMM_WORLD, &mpi_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &mpi_rank);

    /* Make sure the connector supports the API functions being tested */
    if (!(vol_cap_flags_g & H5VL_CAP_FLAG_FILE_BASIC)) {
        if (MAINPROCESS) {
            puts("SKIPPED");
            printf("    API functions for basic file aren't supported with this connector\n");
            fflush(stdout);
        }

        return;
    }

    is_old = mpi_rank % 2;
    mrc    = MPI_Comm_split(MPI_COMM_WORLD, is_old, mpi_rank, &comm);
    VRFY((mrc == MPI_SUCCESS), "");
    MPI_Comm_size(comm, &newprocs);
    MPI_Comm_rank(comm, &newrank);

    if (is_old) {
        /* odd-rank processes */
        mrc = MPI_Barrier(comm);
        VRFY((mrc == MPI_SUCCESS), "");
    }
    else {
        /* even-rank processes */
        int sub_mpi_rank; /* rank in the sub-comm */
        MPI_Comm_rank(comm, &sub_mpi_rank);

        /* setup file access template */
        acc_tpl = create_faccess_plist(comm, info, facc_type);
        VRFY((acc_tpl >= 0), "");

        /* create the file collectively */
        fid = H5Fcreate(filename, H5F_ACC_TRUNC, H5P_DEFAULT, acc_tpl);
        VRFY((fid >= 0), "H5Fcreate succeeded");

        /* close the file */
        ret = H5Fclose(fid);
        VRFY((ret >= 0), "");

        /* delete the test file */
        H5E_BEGIN_TRY
        {
            ret = H5Fdelete(filename, acc_tpl);
        }
        H5E_END_TRY
        VRFY((ret >= 0), "H5Fdelete succeeded");

        /* Release file-access template */
        ret = H5Pclose(acc_tpl);
        VRFY((ret >= 0), "");
    }
    mrc = MPI_Comm_free(&comm);
    VRFY((mrc == MPI_SUCCESS), "MPI_Comm_free succeeded");
    mrc = MPI_Barrier(MPI_COMM_WORLD);
    VRFY((mrc == MPI_SUCCESS), "final MPI_Barrier succeeded");
}

void
test_page_buffer_access(void)
{
    const char *filename;
    hid_t       file_id = H5I_INVALID_HID; /* File ID */
    hid_t       fcpl, fapl;
    herr_t      ret; /* generic return value */
#ifdef PB_OUT
    size_t  page_count = 0;
    int     i, num_elements = 200;
    haddr_t raw_addr, meta_addr;
    int    *data;
    H5F_t  *f              = NULL;
    bool    api_ctx_pushed = false; /* Whether API context pushed */
#endif

    MPI_Comm_rank(MPI_COMM_WORLD, &mpi_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &mpi_size);

    filename = (const char *)GetTestParameters();

    /* Until page buffering is supported in parallel in some form (even if
     * just for a single MPI process), this test just will just check to
     * make sure that an error is thrown when page buffering is enabled
     * with parallel access.
     */
    if (VERBOSE_MED)
        printf("Page Buffer Usage in Parallel %s\n", filename);

    fapl = create_faccess_plist(MPI_COMM_WORLD, MPI_INFO_NULL, facc_type);
    VRFY((fapl >= 0), "create_faccess_plist succeeded");
    fcpl = H5Pcreate(H5P_FILE_CREATE);
    VRFY((fcpl >= 0), "");

    ret = H5Pset_file_space_strategy(fcpl, H5F_FSPACE_STRATEGY_PAGE, 1, (hsize_t)0);
    VRFY((ret == 0), "");
    ret = H5Pset_file_space_page_size(fcpl, sizeof(int) * 128);
    VRFY((ret == 0), "");
    ret = H5Pset_page_buffer_size(fapl, sizeof(int) * 100000, 0, 0);
    VRFY((ret == 0), "");

    /* This should fail because collective metadata writes are not supported with page buffering */
    H5E_BEGIN_TRY
    {
        file_id = H5Fcreate(filename, H5F_ACC_TRUNC, fcpl, fapl);
    }
    H5E_END_TRY
    VRFY((file_id < 0), "H5Fcreate failed");

    /* disable collective metadata writes for page buffering to work */
    ret = H5Pset_coll_metadata_write(fapl, false);
    VRFY((ret >= 0), "");

    /* This should fail due to page buffering not being supported in parallel */
    H5E_BEGIN_TRY
    {
        file_id = H5Fcreate(filename, H5F_ACC_TRUNC, fcpl, fapl);
    }
    H5E_END_TRY
    VRFY((file_id < 0), "H5Fcreate failed");

#ifdef PB_OUT
    ret = create_file(filename, fcpl, fapl, H5AC_METADATA_WRITE_STRATEGY__DISTRIBUTED);
    VRFY((ret == 0), "");
    ret = open_file(filename, fapl, H5AC_METADATA_WRITE_STRATEGY__DISTRIBUTED, sizeof(int) * 100,
                    sizeof(int) * 100000);
    VRFY((ret == 0), "");

    ret = create_file(filename, fcpl, fapl, H5AC_METADATA_WRITE_STRATEGY__PROCESS_0_ONLY);
    VRFY((ret == 0), "");
    ret = open_file(filename, fapl, H5AC_METADATA_WRITE_STRATEGY__PROCESS_0_ONLY, sizeof(int) * 100,
                    sizeof(int) * 100000);
    VRFY((ret == 0), "");

    ret = H5Pset_file_space_page_size(fcpl, sizeof(int) * 100);
    VRFY((ret == 0), "");

    data = (int *)malloc(sizeof(int) * (size_t)num_elements);

    /* initialize all the elements to have a value of -1 */
    for (i = 0; i < num_elements; i++)
        data[i] = -1;
    if (MAINPROCESS) {
        hid_t fapl_self = H5I_INVALID_HID;
        fapl_self       = create_faccess_plist(MPI_COMM_SELF, MPI_INFO_NULL, facc_type);

        ret = H5Pset_page_buffer_size(fapl_self, sizeof(int) * 1000, 0, 0);
        VRFY((ret == 0), "");
        /* collective metadata writes do not work with page buffering */
        ret = H5Pset_coll_metadata_write(fapl_self, false);
        VRFY((ret >= 0), "");

        file_id = H5Fcreate(filename, H5F_ACC_TRUNC, fcpl, fapl_self);
        VRFY((file_id >= 0), "");

        /* Push API context */
        ret = H5CX_push();
        VRFY((ret == 0), "H5CX_push()");
        api_ctx_pushed = true;

        /* Get a pointer to the internal file object */
        f = (H5F_t *)H5I_object(file_id);

        VRFY((f->shared->page_buf != NULL), "Page Buffer created with 1 process");

        /* allocate space for 200 raw elements */
        raw_addr = H5MF_alloc(f, H5FD_MEM_DRAW, sizeof(int) * (size_t)num_elements);
        VRFY((raw_addr != HADDR_UNDEF), "");

        /* allocate space for 200 metadata elements */
        meta_addr = H5MF_alloc(f, H5FD_MEM_SUPER, sizeof(int) * (size_t)num_elements);
        VRFY((meta_addr != HADDR_UNDEF), "");

        page_count = 0;

        ret = H5F_block_write(f, H5FD_MEM_SUPER, meta_addr, sizeof(int) * (size_t)num_elements, data);
        VRFY((ret == 0), "");
        ret = H5F_block_write(f, H5FD_MEM_SUPER, meta_addr, sizeof(int) * (size_t)num_elements, data);
        ret = H5F_block_write(f, H5FD_MEM_DRAW, raw_addr, sizeof(int) * (size_t)num_elements, data);
        VRFY((ret == 0), "");

        VRFY((H5SL_count(f->shared->page_buf->slist_ptr) == page_count), "Wrong number of pages in PB");

        /* update the first 50 elements */
        for (i = 0; i < 50; i++)
            data[i] = i;
        ret = H5F_block_write(f, H5FD_MEM_DRAW, raw_addr, sizeof(int) * 50, data);
        H5Eprint2(H5E_DEFAULT, stderr);
        VRFY((ret == 0), "");
        ret = H5F_block_write(f, H5FD_MEM_SUPER, meta_addr, sizeof(int) * 50, data);
        VRFY((ret == 0), "");
        page_count += 2;
        VRFY((H5SL_count(f->shared->page_buf->slist_ptr) == page_count), "Wrong number of pages in PB");

        /* update the second 50 elements */
        for (i = 0; i < 50; i++)
            data[i] = i + 50;
        ret = H5F_block_write(f, H5FD_MEM_DRAW, raw_addr + (sizeof(int) * 50), sizeof(int) * 50, data);
        VRFY((ret == 0), "");
        ret = H5F_block_write(f, H5FD_MEM_SUPER, meta_addr + (sizeof(int) * 50), sizeof(int) * 50, data);
        VRFY((ret == 0), "");
        VRFY((H5SL_count(f->shared->page_buf->slist_ptr) == page_count), "Wrong number of pages in PB");

        /* update 100 - 200 */
        for (i = 0; i < 100; i++)
            data[i] = i + 100;
        ret = H5F_block_write(f, H5FD_MEM_DRAW, raw_addr + (sizeof(int) * 100), sizeof(int) * 100, data);
        VRFY((ret == 0), "");
        ret = H5F_block_write(f, H5FD_MEM_SUPER, meta_addr + (sizeof(int) * 100), sizeof(int) * 100, data);
        VRFY((ret == 0), "");
        VRFY((H5SL_count(f->shared->page_buf->slist_ptr) == page_count), "Wrong number of pages in PB");

        ret = H5PB_flush(f->shared);
        VRFY((ret == 0), "");

        /* read elements 0 - 200 */
        ret = H5F_block_read(f, H5FD_MEM_DRAW, raw_addr, sizeof(int) * 200, data);
        VRFY((ret == 0), "");
        VRFY((H5SL_count(f->shared->page_buf->slist_ptr) == page_count), "Wrong number of pages in PB");
        for (i = 0; i < 200; i++)
            VRFY((data[i] == i), "Read different values than written");
        ret = H5F_block_read(f, H5FD_MEM_SUPER, meta_addr, sizeof(int) * 200, data);
        VRFY((ret == 0), "");
        VRFY((H5SL_count(f->shared->page_buf->slist_ptr) == page_count), "Wrong number of pages in PB");
        for (i = 0; i < 200; i++)
            VRFY((data[i] == i), "Read different values than written");

        /* read elements 0 - 50 */
        ret = H5F_block_read(f, H5FD_MEM_DRAW, raw_addr, sizeof(int) * 50, data);
        VRFY((ret == 0), "");
        VRFY((H5SL_count(f->shared->page_buf->slist_ptr) == page_count), "Wrong number of pages in PB");
        for (i = 0; i < 50; i++)
            VRFY((data[i] == i), "Read different values than written");
        ret = H5F_block_read(f, H5FD_MEM_SUPER, meta_addr, sizeof(int) * 50, data);
        VRFY((ret == 0), "");
        VRFY((H5SL_count(f->shared->page_buf->slist_ptr) == page_count), "Wrong number of pages in PB");
        for (i = 0; i < 50; i++)
            VRFY((data[i] == i), "Read different values than written");

        /* close the file */
        ret = H5Fclose(file_id);
        VRFY((ret >= 0), "H5Fclose succeeded");
        ret = H5Pclose(fapl_self);
        VRFY((ret >= 0), "H5Pclose succeeded");

        /* Pop API context */
        if (api_ctx_pushed) {
            ret = H5CX_pop(false);
            VRFY((ret == 0), "H5CX_pop()");
            api_ctx_pushed = false;
        }
    }

    MPI_Barrier(MPI_COMM_WORLD);

    if (mpi_size > 1) {
        ret = H5Pset_page_buffer_size(fapl, sizeof(int) * 1000, 0, 0);
        VRFY((ret == 0), "");
        /* collective metadata writes do not work with page buffering */
        ret = H5Pset_coll_metadata_write(fapl, false);
        VRFY((ret >= 0), "");

        file_id = H5Fcreate(filename, H5F_ACC_TRUNC, fcpl, fapl);
        VRFY((file_id >= 0), "");

        /* Push API context */
        ret = H5CX_push();
        VRFY((ret == 0), "H5CX_push()");
        api_ctx_pushed = true;

        /* Get a pointer to the internal file object */
        f = (H5F_t *)H5I_object(file_id);

        VRFY((f->shared->page_buf != NULL), "Page Buffer created with 1 process");

        /* allocate space for 200 raw elements */
        raw_addr = H5MF_alloc(f, H5FD_MEM_DRAW, sizeof(int) * (size_t)num_elements);
        VRFY((raw_addr != HADDR_UNDEF), "");
        /* allocate space for 200 metadata elements */
        meta_addr = H5MF_alloc(f, H5FD_MEM_SUPER, sizeof(int) * (size_t)num_elements);
        VRFY((meta_addr != HADDR_UNDEF), "");

        page_count = 0;

        ret = H5F_block_write(f, H5FD_MEM_SUPER, meta_addr, sizeof(int) * (size_t)num_elements, data);
        VRFY((ret == 0), "");
        ret = H5F_block_write(f, H5FD_MEM_DRAW, raw_addr, sizeof(int) * (size_t)num_elements, data);
        VRFY((ret == 0), "");

        VRFY((H5SL_count(f->shared->page_buf->slist_ptr) == page_count), "Wrong number of pages in PB");

        /* update the first 50 elements */
        for (i = 0; i < 50; i++)
            data[i] = i;
        ret = H5F_block_write(f, H5FD_MEM_DRAW, raw_addr, sizeof(int) * 50, data);
        VRFY((ret == 0), "");
        ret = H5F_block_write(f, H5FD_MEM_SUPER, meta_addr, sizeof(int) * 50, data);
        VRFY((ret == 0), "");
        VRFY((H5SL_count(f->shared->page_buf->slist_ptr) == page_count), "Wrong number of pages in PB");

        /* update the second 50 elements */
        for (i = 0; i < 50; i++)
            data[i] = i + 50;
        ret = H5F_block_write(f, H5FD_MEM_DRAW, raw_addr + (sizeof(int) * 50), sizeof(int) * 50, data);
        VRFY((ret == 0), "");
        ret = H5F_block_write(f, H5FD_MEM_SUPER, meta_addr + (sizeof(int) * 50), sizeof(int) * 50, data);
        VRFY((ret == 0), "");
        VRFY((H5SL_count(f->shared->page_buf->slist_ptr) == page_count), "Wrong number of pages in PB");

        /* update 100 - 200 */
        for (i = 0; i < 100; i++)
            data[i] = i + 100;
        ret = H5F_block_write(f, H5FD_MEM_DRAW, raw_addr + (sizeof(int) * 100), sizeof(int) * 100, data);
        VRFY((ret == 0), "");
        ret = H5F_block_write(f, H5FD_MEM_SUPER, meta_addr + (sizeof(int) * 100), sizeof(int) * 100, data);
        VRFY((ret == 0), "");
        VRFY((H5SL_count(f->shared->page_buf->slist_ptr) == page_count), "Wrong number of pages in PB");

        ret = H5Fflush(file_id, H5F_SCOPE_GLOBAL);
        VRFY((ret == 0), "");

        /* read elements 0 - 200 */
        ret = H5F_block_read(f, H5FD_MEM_DRAW, raw_addr, sizeof(int) * 200, data);
        VRFY((ret == 0), "");
        VRFY((H5SL_count(f->shared->page_buf->slist_ptr) == page_count), "Wrong number of pages in PB");
        for (i = 0; i < 200; i++)
            VRFY((data[i] == i), "Read different values than written");
        ret = H5F_block_read(f, H5FD_MEM_SUPER, meta_addr, sizeof(int) * 200, data);
        VRFY((ret == 0), "");
        VRFY((H5SL_count(f->shared->page_buf->slist_ptr) == page_count), "Wrong number of pages in PB");
        for (i = 0; i < 200; i++)
            VRFY((data[i] == i), "Read different values than written");

        /* read elements 0 - 50 */
        ret = H5F_block_read(f, H5FD_MEM_DRAW, raw_addr, sizeof(int) * 50, data);
        VRFY((ret == 0), "");
        VRFY((H5SL_count(f->shared->page_buf->slist_ptr) == page_count), "Wrong number of pages in PB");
        for (i = 0; i < 50; i++)
            VRFY((data[i] == i), "Read different values than written");
        ret = H5F_block_read(f, H5FD_MEM_SUPER, meta_addr, sizeof(int) * 50, data);
        VRFY((ret == 0), "");
        page_count += 1;
        VRFY((H5SL_count(f->shared->page_buf->slist_ptr) == page_count), "Wrong number of pages in PB");
        for (i = 0; i < 50; i++)
            VRFY((data[i] == i), "Read different values than written");

        MPI_Barrier(MPI_COMM_WORLD);
        /* reset the first 50 elements to -1*/
        for (i = 0; i < 50; i++)
            data[i] = -1;
        ret = H5F_block_write(f, H5FD_MEM_DRAW, raw_addr, sizeof(int) * 50, data);
        VRFY((ret == 0), "");
        VRFY((H5SL_count(f->shared->page_buf->slist_ptr) == page_count), "Wrong number of pages in PB");
        ret = H5F_block_write(f, H5FD_MEM_SUPER, meta_addr, sizeof(int) * 50, data);
        VRFY((ret == 0), "");
        VRFY((H5SL_count(f->shared->page_buf->slist_ptr) == page_count), "Wrong number of pages in PB");

        /* read elements 0 - 50 */
        ret = H5F_block_read(f, H5FD_MEM_DRAW, raw_addr, sizeof(int) * 50, data);
        VRFY((ret == 0), "");
        VRFY((H5SL_count(f->shared->page_buf->slist_ptr) == page_count), "Wrong number of pages in PB");
        for (i = 0; i < 50; i++)
            VRFY((data[i] == -1), "Read different values than written");
        ret = H5F_block_read(f, H5FD_MEM_SUPER, meta_addr, sizeof(int) * 50, data);
        VRFY((ret == 0), "");
        VRFY((H5SL_count(f->shared->page_buf->slist_ptr) == page_count), "Wrong number of pages in PB");
        for (i = 0; i < 50; i++)
            VRFY((data[i] == -1), "Read different values than written");

        /* close the file */
        ret = H5Fclose(file_id);
        VRFY((ret >= 0), "H5Fclose succeeded");
    }

    ret = H5Pclose(fapl);
    VRFY((ret >= 0), "H5Pclose succeeded");
    ret = H5Pclose(fcpl);
    VRFY((ret >= 0), "H5Pclose succeeded");

    /* Pop API context */
    if (api_ctx_pushed) {
        ret = H5CX_pop(false);
        VRFY((ret == 0), "H5CX_pop()");
        api_ctx_pushed = false;
    }

    free(data);
    data = NULL;
    MPI_Barrier(MPI_COMM_WORLD);
#endif
}

#ifdef PB_OUT
static int
create_file(const char *filename, hid_t fcpl, hid_t fapl, int metadata_write_strategy)
{
    hid_t               file_id, dset_id, grp_id;
    hid_t               sid, mem_dataspace;
    hsize_t             start[RANK];
    hsize_t             count[RANK];
    hsize_t             stride[RANK];
    hsize_t             block[RANK];
    DATATYPE           *data_array = NULL;
    hsize_t             dims[RANK], i;
    hsize_t             num_elements;
    int                 k;
    char                dset_name[20];
    H5F_t              *f         = NULL;
    H5C_t              *cache_ptr = NULL;
    H5AC_cache_config_t config;
    bool                api_ctx_pushed = false; /* Whether API context pushed */
    herr_t              ret;

    file_id = H5Fcreate(filename, H5F_ACC_TRUNC, fcpl, fapl);
    VRFY((file_id >= 0), "");

    ret = H5Fflush(file_id, H5F_SCOPE_GLOBAL);
    VRFY((ret == 0), "");

    /* Push API context */
    ret = H5CX_push();
    VRFY((ret == 0), "H5CX_push()");
    api_ctx_pushed = true;

    f = (H5F_t *)H5I_object(file_id);
    VRFY((f != NULL), "");

    cache_ptr = f->shared->cache;

    cache_ptr->ignore_tags = true;
    H5C_stats__reset(cache_ptr);
    config.version = H5AC__CURR_CACHE_CONFIG_VERSION;

    ret = H5AC_get_cache_auto_resize_config(cache_ptr, &config);
    VRFY((ret == 0), "");

    config.metadata_write_strategy = metadata_write_strategy;

    ret = H5AC_set_cache_auto_resize_config(cache_ptr, &config);
    VRFY((ret == 0), "");

    grp_id = H5Gcreate2(file_id, "GROUP", H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
    VRFY((grp_id >= 0), "");

    dims[0] = (hsize_t)(ROW_FACTOR * mpi_size);
    dims[1] = (hsize_t)(COL_FACTOR * mpi_size);
    sid     = H5Screate_simple(RANK, dims, NULL);
    VRFY((sid >= 0), "H5Screate_simple succeeded");

    /* Each process takes a slabs of rows. */
    block[0]  = dims[0] / (hsize_t)mpi_size;
    block[1]  = dims[1];
    stride[0] = block[0];
    stride[1] = block[1];
    count[0]  = 1;
    count[1]  = 1;
    start[0]  = (hsize_t)mpi_rank * block[0];
    start[1]  = 0;

    num_elements = block[0] * block[1];
    /* allocate memory for data buffer */
    data_array = (DATATYPE *)malloc(num_elements * sizeof(DATATYPE));
    VRFY((data_array != NULL), "data_array malloc succeeded");
    /* put some trivial data in the data_array */
    for (i = 0; i < num_elements; i++)
        data_array[i] = mpi_rank + 1;

    ret = H5Sselect_hyperslab(sid, H5S_SELECT_SET, start, stride, count, block);
    VRFY((ret >= 0), "H5Sset_hyperslab succeeded");

    /* create a memory dataspace independently */
    mem_dataspace = H5Screate_simple(1, &num_elements, NULL);
    VRFY((mem_dataspace >= 0), "");

    for (k = 0; k < NUM_DSETS; k++) {
        snprintf(dset_name, sizeof(dset_name), "D1dset%d", k);
        dset_id = H5Dcreate2(grp_id, dset_name, H5T_NATIVE_INT, sid, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
        VRFY((dset_id >= 0), "");
        ret = H5Dclose(dset_id);
        VRFY((ret == 0), "");

        snprintf(dset_name, sizeof(dset_name), "D2dset%d", k);
        dset_id = H5Dcreate2(grp_id, dset_name, H5T_NATIVE_INT, sid, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
        VRFY((dset_id >= 0), "");
        ret = H5Dclose(dset_id);
        VRFY((ret == 0), "");

        snprintf(dset_name, sizeof(dset_name), "D3dset%d", k);
        dset_id = H5Dcreate2(grp_id, dset_name, H5T_NATIVE_INT, sid, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
        VRFY((dset_id >= 0), "");
        ret = H5Dclose(dset_id);
        VRFY((ret == 0), "");

        snprintf(dset_name, sizeof(dset_name), "dset%d", k);
        dset_id = H5Dcreate2(grp_id, dset_name, H5T_NATIVE_INT, sid, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
        VRFY((dset_id >= 0), "");

        ret = H5Dwrite(dset_id, H5T_NATIVE_INT, mem_dataspace, sid, H5P_DEFAULT, data_array);
        VRFY((ret == 0), "");

        ret = H5Dclose(dset_id);
        VRFY((ret == 0), "");

        memset(data_array, 0, num_elements * sizeof(DATATYPE));
        dset_id = H5Dopen2(grp_id, dset_name, H5P_DEFAULT);
        VRFY((dset_id >= 0), "");

        ret = H5Dread(dset_id, H5T_NATIVE_INT, mem_dataspace, sid, H5P_DEFAULT, data_array);
        VRFY((ret == 0), "");

        ret = H5Dclose(dset_id);
        VRFY((ret == 0), "");

        for (i = 0; i < num_elements; i++)
            VRFY((data_array[i] == mpi_rank + 1), "Dataset Verify failed");

        snprintf(dset_name, sizeof(dset_name), "D1dset%d", k);
        ret = H5Ldelete(grp_id, dset_name, H5P_DEFAULT);
        VRFY((ret == 0), "");
        snprintf(dset_name, sizeof(dset_name), "D2dset%d", k);
        ret = H5Ldelete(grp_id, dset_name, H5P_DEFAULT);
        VRFY((ret == 0), "");
        snprintf(dset_name, sizeof(dset_name), "D3dset%d", k);
        ret = H5Ldelete(grp_id, dset_name, H5P_DEFAULT);
        VRFY((ret == 0), "");
    }

    ret = H5Gclose(grp_id);
    VRFY((ret == 0), "");
    ret = H5Fclose(file_id);
    VRFY((ret == 0), "");
    ret = H5Sclose(sid);
    VRFY((ret == 0), "");
    ret = H5Sclose(mem_dataspace);
    VRFY((ret == 0), "");

    /* Pop API context */
    if (api_ctx_pushed) {
        ret = H5CX_pop(false);
        VRFY((ret == 0), "H5CX_pop()");
        api_ctx_pushed = false;
    }

    MPI_Barrier(MPI_COMM_WORLD);
    free(data_array);
    return 0;
} /* create_file */

static int
open_file(const char *filename, hid_t fapl, int metadata_write_strategy, hsize_t page_size,
          size_t page_buffer_size)
{
    hid_t               file_id, dset_id, grp_id, grp_id2;
    hid_t               sid, mem_dataspace;
    DATATYPE           *data_array = NULL;
    hsize_t             dims[RANK];
    hsize_t             start[RANK];
    hsize_t             count[RANK];
    hsize_t             stride[RANK];
    hsize_t             block[RANK];
    int                 i, k, ndims;
    hsize_t             num_elements;
    char                dset_name[20];
    H5F_t              *f         = NULL;
    H5C_t              *cache_ptr = NULL;
    H5AC_cache_config_t config;
    bool                api_ctx_pushed = false; /* Whether API context pushed */
    herr_t              ret;

    config.version = H5AC__CURR_CACHE_CONFIG_VERSION;
    ret            = H5Pget_mdc_config(fapl, &config);
    VRFY((ret == 0), "");

    config.metadata_write_strategy = metadata_write_strategy;

    ret = H5Pget_mdc_config(fapl, &config);
    VRFY((ret == 0), "");

    file_id = H5Fopen(filename, H5F_ACC_RDWR, fapl);
    H5Eprint2(H5E_DEFAULT, stderr);
    VRFY((file_id >= 0), "");

    /* Push API context */
    ret = H5CX_push();
    VRFY((ret == 0), "H5CX_push()");
    api_ctx_pushed = true;

    ret = H5Fflush(file_id, H5F_SCOPE_GLOBAL);
    VRFY((ret == 0), "");

    f = (H5F_t *)H5I_object(file_id);
    VRFY((f != NULL), "");

    cache_ptr = f->shared->cache;

    MPI_Barrier(MPI_COMM_WORLD);

    VRFY((f->shared->page_buf != NULL), "");
    VRFY((f->shared->page_buf->page_size == page_size), "");
    VRFY((f->shared->page_buf->max_size == page_buffer_size), "");

    grp_id = H5Gopen2(file_id, "GROUP", H5P_DEFAULT);
    VRFY((grp_id >= 0), "");

    dims[0] = (hsize_t)(ROW_FACTOR * mpi_size);
    dims[1] = (hsize_t)(COL_FACTOR * mpi_size);

    /* Each process takes a slabs of rows. */
    block[0]  = dims[0] / (hsize_t)mpi_size;
    block[1]  = dims[1];
    stride[0] = block[0];
    stride[1] = block[1];
    count[0]  = 1;
    count[1]  = 1;
    start[0]  = (hsize_t)mpi_rank * block[0];
    start[1]  = 0;

    num_elements = block[0] * block[1];
    /* allocate memory for data buffer */
    data_array = (DATATYPE *)malloc(num_elements * sizeof(DATATYPE));
    VRFY((data_array != NULL), "data_array malloc succeeded");

    /* create a memory dataspace independently */
    mem_dataspace = H5Screate_simple(1, &num_elements, NULL);
    VRFY((mem_dataspace >= 0), "");

    for (k = 0; k < NUM_DSETS; k++) {
        snprintf(dset_name, sizeof(dset_name), "dset%d", k);
        dset_id = H5Dopen2(grp_id, dset_name, H5P_DEFAULT);
        VRFY((dset_id >= 0), "");

        sid = H5Dget_space(dset_id);
        VRFY((dset_id >= 0), "H5Dget_space succeeded");

        ndims = H5Sget_simple_extent_dims(sid, dims, NULL);
        VRFY((ndims == 2), "H5Sget_simple_extent_dims succeeded");
        VRFY(dims[0] == (hsize_t)(ROW_FACTOR * mpi_size), "Wrong dataset dimensions");
        VRFY(dims[1] == (hsize_t)(COL_FACTOR * mpi_size), "Wrong dataset dimensions");

        ret = H5Sselect_hyperslab(sid, H5S_SELECT_SET, start, stride, count, block);
        VRFY((ret >= 0), "H5Sset_hyperslab succeeded");

        ret = H5Dread(dset_id, H5T_NATIVE_INT, mem_dataspace, sid, H5P_DEFAULT, data_array);
        VRFY((ret >= 0), "");

        ret = H5Dclose(dset_id);
        VRFY((ret >= 0), "");
        ret = H5Sclose(sid);
        VRFY((ret == 0), "");

        for (i = 0; i < (int)num_elements; i++)
            VRFY((data_array[i] == mpi_rank + 1), "Dataset Verify failed");
    }

    grp_id2 = H5Gcreate2(file_id, "GROUP/GROUP2", H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
    VRFY((grp_id2 >= 0), "");
    ret = H5Gclose(grp_id2);
    VRFY((ret == 0), "");

    ret = H5Fflush(file_id, H5F_SCOPE_GLOBAL);
    VRFY((ret == 0), "");

    MPI_Barrier(MPI_COMM_WORLD);
    /* flush invalidate each ring, starting from the outermost ring and
     * working inward.
     */
    for (i = 0; i < H5C__HASH_TABLE_LEN; i++) {
        H5C_cache_entry_t *entry_ptr = NULL;

        entry_ptr = cache_ptr->index[i];

        while (entry_ptr != NULL) {
            assert(entry_ptr->is_dirty == false);

            if (!entry_ptr->is_pinned && !entry_ptr->is_protected) {
                ret = H5AC_expunge_entry(f, entry_ptr->type, entry_ptr->addr, 0);
                VRFY((ret == 0), "");
            }

            entry_ptr = entry_ptr->ht_next;
        }
    }
    MPI_Barrier(MPI_COMM_WORLD);

    grp_id2 = H5Gopen2(file_id, "GROUP/GROUP2", H5P_DEFAULT);
    H5Eprint2(H5E_DEFAULT, stderr);
    VRFY((grp_id2 >= 0), "");
    ret = H5Gclose(grp_id2);
    H5Eprint2(H5E_DEFAULT, stderr);
    VRFY((ret == 0), "");

    ret = H5Gclose(grp_id);
    VRFY((ret == 0), "");
    ret = H5Fclose(file_id);
    VRFY((ret == 0), "");
    ret = H5Sclose(mem_dataspace);
    VRFY((ret == 0), "");

    /* Pop API context */
    if (api_ctx_pushed) {
        ret = H5CX_pop(false);
        VRFY((ret == 0), "H5CX_pop()");
        api_ctx_pushed = false;
    }

    free(data_array);

    return nerrors;
}
#endif

/*
 * NOTE:  See HDFFV-10894 and add tests later to verify MPI-specific properties in the
 *        incoming fapl that could conflict with the existing values in H5F_shared_t on
 *        multiple opens of the same file.
 */
void
test_file_properties(void)
{
    hid_t       fid          = H5I_INVALID_HID; /* HDF5 file ID */
    hid_t       fapl_id      = H5I_INVALID_HID; /* File access plist */
    hid_t       fapl_copy_id = H5I_INVALID_HID; /* File access plist */
    bool        is_coll;
    htri_t      are_equal;
    const char *filename;
    MPI_Comm    comm     = MPI_COMM_WORLD;
    MPI_Info    info     = MPI_INFO_NULL;
    MPI_Comm    comm_out = MPI_COMM_NULL;
    MPI_Info    info_out = MPI_INFO_NULL;
    herr_t      ret;     /* Generic return value */
    int         mpi_ret; /* MPI return value */
    int         cmp;     /* Compare value */

    /* set up MPI parameters */
    mpi_ret = MPI_Comm_size(MPI_COMM_WORLD, &mpi_size);
    VRFY((mpi_ret >= 0), "MPI_Comm_size succeeded");
    mpi_ret = MPI_Comm_rank(MPI_COMM_WORLD, &mpi_rank);
    VRFY((mpi_ret >= 0), "MPI_Comm_rank succeeded");

    /* Make sure the connector supports the API functions being tested */
    if (!(vol_cap_flags_g & H5VL_CAP_FLAG_FILE_BASIC)) {
        if (MAINPROCESS) {
            puts("SKIPPED");
            printf("    API functions for basic file aren't supported with this connector\n");
            fflush(stdout);
        }

        return;
    }

    filename = (const char *)GetTestParameters();

    mpi_ret = MPI_Info_create(&info);
    VRFY((mpi_ret >= 0), "MPI_Info_create succeeded");
    mpi_ret = MPI_Info_set(info, "hdf_info_prop1", "xyz");
    VRFY((mpi_ret == MPI_SUCCESS), "MPI_Info_set succeeded");

    /* setup file access plist */
    fapl_id = H5Pcreate(H5P_FILE_ACCESS);
    VRFY((fapl_id != H5I_INVALID_HID), "H5Pcreate");
    ret = H5Pset_fapl_mpio(fapl_id, comm, info);
    VRFY((ret >= 0), "H5Pset_fapl_mpio");

    /* Check getting and setting MPI properties
     * (for use in VOL connectors, not the MPI-I/O VFD)
     */
    ret = H5Pset_mpi_params(fapl_id, comm, info);
    VRFY((ret >= 0), "H5Pset_mpi_params succeeded");
    ret = H5Pget_mpi_params(fapl_id, &comm_out, &info_out);
    VRFY((ret >= 0), "H5Pget_mpi_params succeeded");

    /* Check the communicator */
    VRFY((comm != comm_out), "Communicators should not be bitwise identical");
    cmp     = MPI_UNEQUAL;
    mpi_ret = MPI_Comm_compare(comm, comm_out, &cmp);
    VRFY((ret >= 0), "MPI_Comm_compare succeeded");
    VRFY((cmp == MPI_CONGRUENT), "Communicators should be congruent via MPI_Comm_compare");

    /* Check the info object */
    VRFY((info != info_out), "Info objects should not be bitwise identical");

    /* Free the obtained comm and info object */
    mpi_ret = MPI_Comm_free(&comm_out);
    VRFY((mpi_ret >= 0), "MPI_Comm_free succeeded");
    mpi_ret = MPI_Info_free(&info_out);
    VRFY((mpi_ret >= 0), "MPI_Info_free succeeded");

    /* Copy the fapl and ensure it's equal to the original */
    fapl_copy_id = H5Pcopy(fapl_id);
    VRFY((fapl_copy_id != H5I_INVALID_HID), "H5Pcopy");
    are_equal = H5Pequal(fapl_id, fapl_copy_id);
    VRFY((true == are_equal), "H5Pequal");

    /* Add a property to the copy and ensure it's different now */
    mpi_ret = MPI_Info_set(info, "hdf_info_prop2", "abc");
    VRFY((mpi_ret == MPI_SUCCESS), "MPI_Info_set");
    ret = H5Pset_mpi_params(fapl_copy_id, comm, info);
    VRFY((ret >= 0), "H5Pset_mpi_params succeeded");
    are_equal = H5Pequal(fapl_id, fapl_copy_id);
    VRFY((false == are_equal), "H5Pequal");

    /* Add a property with the same key but a different value to the original
     * and ensure they are still different.
     */
    mpi_ret = MPI_Info_set(info, "hdf_info_prop2", "ijk");
    VRFY((mpi_ret == MPI_SUCCESS), "MPI_Info_set");
    ret = H5Pset_mpi_params(fapl_id, comm, info);
    VRFY((ret >= 0), "H5Pset_mpi_params succeeded");
    are_equal = H5Pequal(fapl_id, fapl_copy_id);
    VRFY((false == are_equal), "H5Pequal");

    /* Set the second property in the original to the same
     * value as the copy and ensure they are the same now.
     */
    mpi_ret = MPI_Info_set(info, "hdf_info_prop2", "abc");
    VRFY((mpi_ret == MPI_SUCCESS), "MPI_Info_set");
    ret = H5Pset_mpi_params(fapl_id, comm, info);
    VRFY((ret >= 0), "H5Pset_mpi_params succeeded");
    are_equal = H5Pequal(fapl_id, fapl_copy_id);
    VRFY((true == are_equal), "H5Pequal");

    /* create the file */
    fid = H5Fcreate(filename, H5F_ACC_TRUNC, H5P_DEFAULT, fapl_id);
    VRFY((fid != H5I_INVALID_HID), "H5Fcreate succeeded");

    /* verify settings for file access properties */

    /* Collective metadata writes */
    ret = H5Pget_coll_metadata_write(fapl_id, &is_coll);
    VRFY((ret >= 0), "H5Pget_coll_metadata_write succeeded");
    VRFY((is_coll == false), "Incorrect property setting for coll metadata writes");

    /* Collective metadata read API calling requirement */
    ret = H5Pget_all_coll_metadata_ops(fapl_id, &is_coll);
    VRFY((ret >= 0), "H5Pget_all_coll_metadata_ops succeeded");
    VRFY((is_coll == false), "Incorrect property setting for coll metadata API calls requirement");

    ret = H5Fclose(fid);
    VRFY((ret >= 0), "H5Fclose succeeded");

    /* Open the file with the MPI-IO driver */
    ret = H5Pset_fapl_mpio(fapl_id, comm, info);
    VRFY((ret >= 0), "H5Pset_fapl_mpio failed");
    fid = H5Fopen(filename, H5F_ACC_RDWR, fapl_id);
    VRFY((fid != H5I_INVALID_HID), "H5Fopen succeeded");

    /* verify settings for file access properties */

    /* Collective metadata writes */
    ret = H5Pget_coll_metadata_write(fapl_id, &is_coll);
    VRFY((ret >= 0), "H5Pget_coll_metadata_write succeeded");
    VRFY((is_coll == false), "Incorrect property setting for coll metadata writes");

    /* Collective metadata read API calling requirement */
    ret = H5Pget_all_coll_metadata_ops(fapl_id, &is_coll);
    VRFY((ret >= 0), "H5Pget_all_coll_metadata_ops succeeded");
    VRFY((is_coll == false), "Incorrect property setting for coll metadata API calls requirement");

    ret = H5Fclose(fid);
    VRFY((ret >= 0), "H5Fclose succeeded");

    /* Open the file with the MPI-IO driver w/ collective settings */
    ret = H5Pset_fapl_mpio(fapl_id, comm, info);
    VRFY((ret >= 0), "H5Pset_fapl_mpio failed");
    /* Collective metadata writes */
    ret = H5Pset_coll_metadata_write(fapl_id, true);
    VRFY((ret >= 0), "H5Pget_coll_metadata_write succeeded");
    /* Collective metadata read API calling requirement */
    ret = H5Pset_all_coll_metadata_ops(fapl_id, true);
    VRFY((ret >= 0), "H5Pget_all_coll_metadata_ops succeeded");
    fid = H5Fopen(filename, H5F_ACC_RDWR, fapl_id);
    VRFY((fid != H5I_INVALID_HID), "H5Fopen succeeded");

    /* verify settings for file access properties */

    /* Collective metadata writes */
    ret = H5Pget_coll_metadata_write(fapl_id, &is_coll);
    VRFY((ret >= 0), "H5Pget_coll_metadata_write succeeded");
    VRFY((is_coll == true), "Incorrect property setting for coll metadata writes");

    /* Collective metadata read API calling requirement */
    ret = H5Pget_all_coll_metadata_ops(fapl_id, &is_coll);
    VRFY((ret >= 0), "H5Pget_all_coll_metadata_ops succeeded");
    VRFY((is_coll == true), "Incorrect property setting for coll metadata API calls requirement");

    /* close fapl and retrieve it from file */
    ret = H5Pclose(fapl_id);
    VRFY((ret >= 0), "H5Pclose succeeded");
    fapl_id = H5I_INVALID_HID;

    fapl_id = H5Fget_access_plist(fid);
    VRFY((fapl_id != H5I_INVALID_HID), "H5P_FILE_ACCESS");

    /* verify settings for file access properties */

    /* Collective metadata writes */
    ret = H5Pget_coll_metadata_write(fapl_id, &is_coll);
    VRFY((ret >= 0), "H5Pget_coll_metadata_write succeeded");
    VRFY((is_coll == true), "Incorrect property setting for coll metadata writes");

    /* Collective metadata read API calling requirement */
    ret = H5Pget_all_coll_metadata_ops(fapl_id, &is_coll);
    VRFY((ret >= 0), "H5Pget_all_coll_metadata_ops succeeded");
    VRFY((is_coll == true), "Incorrect property setting for coll metadata API calls requirement");

    /* close file */
    ret = H5Fclose(fid);
    VRFY((ret >= 0), "H5Fclose succeeded");

    /* Release file-access plist */
    ret = H5Pclose(fapl_id);
    VRFY((ret >= 0), "H5Pclose succeeded");
    ret = H5Pclose(fapl_copy_id);
    VRFY((ret >= 0), "H5Pclose succeeded");

    /* Free the MPI info object */
    mpi_ret = MPI_Info_free(&info);
    VRFY((mpi_ret >= 0), "MPI_Info_free succeeded");

} /* end test_file_properties() */

void
test_delete(void)
{
    hid_t       fid           = H5I_INVALID_HID; /* HDF5 file ID */
    hid_t       fapl_id       = H5I_INVALID_HID; /* File access plist */
    const char *filename      = NULL;
    MPI_Comm    comm          = MPI_COMM_WORLD;
    MPI_Info    info          = MPI_INFO_NULL;
    htri_t      is_accessible = FAIL; /* Whether a file is accessible */
    herr_t      ret;                  /* Generic return value */

    filename = (const char *)GetTestParameters();

    /* set up MPI parameters */
    MPI_Comm_size(MPI_COMM_WORLD, &mpi_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &mpi_rank);

    /* Make sure the connector supports the API functions being tested */
    if (!(vol_cap_flags_g & H5VL_CAP_FLAG_FILE_BASIC) || !(vol_cap_flags_g & H5VL_CAP_FLAG_FILE_MORE)) {
        if (MAINPROCESS) {
            puts("SKIPPED");
            printf("    API functions for basic file or file more aren't supported with this "
                   "connector\n");
            fflush(stdout);
        }

        return;
    }

    /* setup file access plist */
    fapl_id = H5Pcreate(H5P_FILE_ACCESS);
    VRFY((fapl_id != H5I_INVALID_HID), "H5Pcreate");
    ret = H5Pset_fapl_mpio(fapl_id, comm, info);
    VRFY((SUCCEED == ret), "H5Pset_fapl_mpio");

    /* create the file */
    fid = H5Fcreate(filename, H5F_ACC_TRUNC, H5P_DEFAULT, fapl_id);
    VRFY((fid != H5I_INVALID_HID), "H5Fcreate");

    /* close the file */
    ret = H5Fclose(fid);
    VRFY((SUCCEED == ret), "H5Fclose");

    /* Verify that the file is an HDF5 file */
    is_accessible = H5Fis_accessible(filename, fapl_id);
    VRFY((true == is_accessible), "H5Fis_accessible");

    /* Delete the file */
    ret = H5Fdelete(filename, fapl_id);
    VRFY((SUCCEED == ret), "H5Fdelete");

    /*
     * Work around a Cray MPICH bug that causes
     * H5Fis_accessible to re-create the just-deleted
     * file as a 0-byte file with strange Unix
     * permissions, causing the routine to return
     * false here instead of FAIL.
     */
    H5Pset_fapl_mpio(fapl_id, comm, info);

    /* Verify that the file is NO LONGER an HDF5 file */
    /* This should fail since there is no file */
    H5E_BEGIN_TRY
    {
        is_accessible = H5Fis_accessible(filename, fapl_id);
    }
    H5E_END_TRY
    VRFY((FAIL == is_accessible), "H5Fis_accessible failed as expected");

    /* Release file-access plist */
    ret = H5Pclose(fapl_id);
    VRFY((SUCCEED == ret), "H5Pclose");

} /* end test_delete() */

/*
 * Tests for an assertion failure during file close that used
 * to occur when the library fails to create a file in parallel
 * due to an invalid library version bounds setting
 */
void
test_invalid_libver_bounds_file_close_assert(void)
{
    const char *filename = NULL;
    MPI_Comm    comm     = MPI_COMM_WORLD;
    MPI_Info    info     = MPI_INFO_NULL;
    herr_t      ret;
    hid_t       fid     = H5I_INVALID_HID;
    hid_t       fapl_id = H5I_INVALID_HID;
    hid_t       fcpl_id = H5I_INVALID_HID;

    filename = (const char *)GetTestParameters();

    /* set up MPI parameters */
    MPI_Comm_size(MPI_COMM_WORLD, &mpi_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &mpi_rank);

    /* setup file access plist */
    fapl_id = H5Pcreate(H5P_FILE_ACCESS);
    VRFY((fapl_id != H5I_INVALID_HID), "H5Pcreate");
    ret = H5Pset_fapl_mpio(fapl_id, comm, info);
    VRFY((SUCCEED == ret), "H5Pset_fapl_mpio");
    ret = H5Pset_libver_bounds(fapl_id, H5F_LIBVER_EARLIEST, H5F_LIBVER_V18);
    VRFY((SUCCEED == ret), "H5Pset_libver_bounds");

    /* setup file creation plist */
    fcpl_id = H5Pcreate(H5P_FILE_CREATE);
    VRFY((fcpl_id != H5I_INVALID_HID), "H5Pcreate");

    ret = H5Pset_file_space_strategy(fcpl_id, H5F_FSPACE_STRATEGY_PAGE, true, 1);
    VRFY((SUCCEED == ret), "H5Pset_file_space_strategy");

    /* create the file */
    H5E_BEGIN_TRY
    {
        fid = H5Fcreate(filename, H5F_ACC_TRUNC, fcpl_id, fapl_id);
    }
    H5E_END_TRY
    VRFY((fid == H5I_INVALID_HID), "H5Fcreate");

    ret = H5Pclose(fapl_id);
    VRFY((SUCCEED == ret), "H5Pclose");
    ret = H5Pclose(fcpl_id);
    VRFY((SUCCEED == ret), "H5Pclose");
}

/*
 * Tests that H5Pevict_on_close properly succeeds in serial/one rank and fails when
 * called by multiple ranks.
 */
void
test_evict_on_close_parallel_unsupp(void)
{
    const char *filename = NULL;
    MPI_Comm    comm     = MPI_COMM_WORLD;
    MPI_Info    info     = MPI_INFO_NULL;
    hid_t       fid      = H5I_INVALID_HID;
    hid_t       fapl_id  = H5I_INVALID_HID;
    herr_t      ret;

    filename = (const char *)GetTestParameters();

    /* set up MPI parameters */
    MPI_Comm_size(MPI_COMM_WORLD, &mpi_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &mpi_rank);

    /* setup file access plist */
    fapl_id = H5Pcreate(H5P_FILE_ACCESS);
    VRFY((fapl_id != H5I_INVALID_HID), "H5Pcreate");
    ret = H5Pset_libver_bounds(fapl_id, H5F_LIBVER_EARLIEST, H5F_LIBVER_V18);
    VRFY((SUCCEED == ret), "H5Pset_libver_bounds");

    ret = H5Pset_evict_on_close(fapl_id, true);
    VRFY((SUCCEED == ret), "H5Pset_evict_on_close");

    /* test on 1 rank */
    ret = H5Pset_fapl_mpio(fapl_id, MPI_COMM_SELF, info);
    VRFY((SUCCEED == ret), "H5Pset_fapl_mpio");

    if (mpi_rank == 0) {
        fid = H5Fcreate(filename, H5F_ACC_TRUNC, H5P_DEFAULT, fapl_id);
        VRFY((SUCCEED == ret), "H5Fcreate");
        ret = H5Fclose(fid);
        VRFY((SUCCEED == ret), "H5Fclose");
    }

    VRFY((MPI_SUCCESS == MPI_Barrier(MPI_COMM_WORLD)), "MPI_Barrier");

    /* test on multiple ranks if we have them */
    if (mpi_size > 1) {
        ret = H5Pset_fapl_mpio(fapl_id, comm, info);
        VRFY((SUCCEED == ret), "H5Pset_fapl_mpio");

        H5E_BEGIN_TRY
        {
            fid = H5Fcreate(filename, H5F_ACC_TRUNC, H5P_DEFAULT, fapl_id);
        }
        H5E_END_TRY
        VRFY((fid == H5I_INVALID_HID), "H5Fcreate");
    }

    ret = H5Pclose(fapl_id);
    VRFY((SUCCEED == ret), "H5Pclose");
}

/*
 * Verify that MPI I/O hints are preserved after closing the file access property list
 * as described in issue #3025
 * This is a test program from the user.
 */
void
test_fapl_preserve_hints(void)
{
    const char *filename;
    const char *key       = "hdf_info_fapl";
    const char *value     = "xyz";
    MPI_Info    info_used = MPI_INFO_NULL;
    MPI_Info    info      = MPI_INFO_NULL;
    hid_t       fid       = H5I_INVALID_HID; /* HDF5 file ID */
    hid_t       fapl_id   = H5I_INVALID_HID; /* File access plist */
    char        key_used[MPI_MAX_INFO_KEY + 1];
    char       *value_used = NULL;
    bool        same       = false;
    int         flag       = -1;
    int         nkeys_used;
    int         i;
    int         mpi_ret; /* MPI return value */
    herr_t      ret;     /* Generic return value */

    filename = (const char *)GetTestParameters();

    value_used = malloc(MPI_MAX_INFO_VAL + 1);
    VRFY(value_used, "malloc succeeded");

    /* set up MPI parameters */
    mpi_ret = MPI_Info_create(&info);
    VRFY((mpi_ret >= 0), "MPI_Info_create succeeded");

    mpi_ret = MPI_Info_set(info, key, value);
    VRFY((mpi_ret == MPI_SUCCESS), "MPI_Info_set succeeded");

    fapl_id = H5Pcreate(H5P_FILE_ACCESS);
    VRFY((fapl_id != H5I_INVALID_HID), "H5Pcreate");

    ret = H5Pset_fapl_mpio(fapl_id, MPI_COMM_WORLD, info);
    VRFY((ret >= 0), "H5Pset_fapl_mpio");

    fid = H5Fcreate(filename, H5F_ACC_TRUNC, H5P_DEFAULT, fapl_id);
    VRFY((fid != H5I_INVALID_HID), "H5Fcreate succeeded");

    ret = H5Pclose(fapl_id);
    VRFY((ret >= 0), "H5Pclose succeeded");

    fapl_id = H5Fget_access_plist(fid);
    VRFY((fapl_id != H5I_INVALID_HID), "H5Fget_access_plist succeeded");

    ret = H5Pget_fapl_mpio(fapl_id, NULL, &info_used);
    VRFY((ret >= 0), "H5Pget_fapl_mpio succeeded");

    VRFY((info_used != MPI_INFO_NULL), "H5Pget_fapl_mpio");

    mpi_ret = MPI_Info_get_nkeys(info_used, &nkeys_used);
    VRFY((mpi_ret == MPI_SUCCESS), "MPI_Info_get_nkeys succeeded");

    /* Loop over the # of keys */
    for (i = 0; i < nkeys_used; i++) {

        /* Memset the buffers to zero */
        memset(key_used, 0, MPI_MAX_INFO_KEY + 1);
        memset(value_used, 0, MPI_MAX_INFO_VAL + 1);

        /* Get the nth key */
        mpi_ret = MPI_Info_get_nthkey(info_used, i, key_used);
        VRFY((mpi_ret == MPI_SUCCESS), "MPI_Info_get_nthkey succeeded");

        if (!strcmp(key_used, key)) {
            mpi_ret = MPI_Info_get(info_used, key_used, MPI_MAX_INFO_VAL, value_used, &flag);
            VRFY((mpi_ret == MPI_SUCCESS), "MPI_Info_get succeeded");

            if (!strcmp(value_used, value)) {

                /* Both key_used and value_used are the same */
                same = true;
                break;
            }
        }
    } /* end for */

    VRFY((same == true), "key_used and value_used are the same");

    ret = H5Pclose(fapl_id);
    VRFY((ret >= 0), "H5Pclose succeeded");

    ret = H5Fclose(fid);
    VRFY((ret >= 0), "H5Fclose succeeded");

    /* Free the MPI info object */
    mpi_ret = MPI_Info_free(&info);
    VRFY((mpi_ret >= 0), "MPI_Info_free succeeded");

    mpi_ret = MPI_Info_free(&info_used);
    VRFY((mpi_ret >= 0), "MPI_Info_free succeeded");

    free(value_used);

} /* end test_fapl_preserve_hints() */
