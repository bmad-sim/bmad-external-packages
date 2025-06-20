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
 * Purpose:    Tests the plugin module (H5PL)
 */

#include "h5test.h"
#include "H5srcdir.h"

/*
 * This file needs to access private datatypes from the H5Z and H5PL package.
 */
#define H5PL_FRIEND
#include "H5PLpkg.h"
#define H5Z_FRIEND
#include "H5Zpkg.h"

/* Filters IDs for test filter plugins */
#define FILTER1_ID 257
#define FILTER2_ID 258
#define FILTER3_ID 259
#define FILTER4_ID 260

static const char *FILENAME[] = {"filter_plugin", NULL};
#define FILENAME_BUF_SIZE 1024

/* Dataset names */
#define DSET_DEFLATE_NAME "deflate dset"
#define DSET_FILTER1_NAME "filter 1 dset"
#define DSET_FILTER2_NAME "filter 2 dset"
#define DSET_FILTER3_NAME "filter 3 dset"

/* Array sizes used throughout the test */
#define DSET_DIM1         100
#define DSET_DIM2         200
#define CHUNK_DIM1        2
#define CHUNK_DIM2        25
#define HYPERSLAB_OFFSET1 7
#define HYPERSLAB_OFFSET2 30
#define HYPERSLAB_SIZE1   4
#define HYPERSLAB_SIZE2   50

/* Global size arrays */
static const hsize_t sizes_g[2]       = {DSET_DIM1, DSET_DIM2};                 /* Dataset dimensions   */
static const hsize_t hs_sizes_g[2]    = {HYPERSLAB_SIZE1, HYPERSLAB_SIZE2};     /* Hyperslab sizes      */
static const hsize_t hs_offsets_g[2]  = {HYPERSLAB_OFFSET1, HYPERSLAB_OFFSET2}; /* Hyperslab offsets    */
static const hsize_t chunk_sizes_g[2] = {CHUNK_DIM1, CHUNK_DIM2};               /* Chunk dimensions     */

/* Limit random number within 20000 */
#define RANDOM_LIMIT 20000

/* Things used in the groups + filter plugins test */
#define N_SUBGROUPS          1000
#define SUBGROUP_PREFIX      "subgroup_"
#define TOP_LEVEL_GROUP_NAME "top-level group"

/* Global arrays in which to save data */
static int **orig_deflate_g = NULL;
static int **orig_dynlib1_g = NULL;
static int **orig_dynlib2_g = NULL;
static int **orig_dynlib4_g = NULL;

/*-------------------------------------------------------------------------
 * Function:  free_2D_array
 *
 * Purpose:   Free up a dynamic 2D pseudo array and set the pointer to NULL
 *            Designed to be callable in error conditions so NULLs are okay
 *
 * Return:    SUCCEED (always)
 *
 *-------------------------------------------------------------------------
 */
static herr_t
free_2D_array(int ***arr)
{

    if (arr && *arr && (*arr)[0])
        free((*arr)[0]);
    if (arr && *arr)
        free(*arr);
    *arr = NULL;

    return SUCCEED;
} /* end free_2D_array() */

/*-------------------------------------------------------------------------
 * Function:  allocate_and_init_2D_array
 *
 * Purpose:   Initialize an array as a pseudo 2D array and copy in some
 *            initial values.
 *
 * Return:    SUCCEED/FAIL
 *
 *-------------------------------------------------------------------------
 */
static herr_t
allocate_and_init_2D_array(int ***arr, const hsize_t *sizes, int **initial_values)
{

    size_t r, c;    /* Data rows and columns    */
    size_t i;       /* Iterator                 */
    size_t n_bytes; /* # of bytes to copy       */

    r = (size_t)sizes[0];
    c = (size_t)sizes[1];

    /* Allocate and set up pseudo-2D array */
    if (NULL == (*arr = (int **)calloc(r, sizeof(int *))))
        TEST_ERROR;
    if (NULL == ((*arr)[0] = (int *)calloc(r * c, sizeof(int))))
        TEST_ERROR;
    for (i = 0; i < r; i++)
        (*arr)[i] = (**arr + c * i);

    /* Copy over the data elements */
    if (initial_values) {
        n_bytes = r * c * sizeof(int);
        memcpy((*arr)[0], initial_values[0], n_bytes);
    }

    return SUCCEED;
error:
    free_2D_array(arr);

    return FAIL;
} /* end allocate_and_init_2D_array() */

/*-------------------------------------------------------------------------
 * Function:  compare_2D_arrays
 *
 * Purpose:   Compare two pseudo 2D arrays
 *
 * Return:    SUCCEED/FAIL
 *
 *-------------------------------------------------------------------------
 */
static herr_t
compare_2D_arrays(int **dset1, int **dset2, const hsize_t *sizes, /*OUT*/ bool *are_same)
{
    hsize_t i, j; /* index variables */

    *are_same = true;

    /* Check all the array values. This could optionally emit any
     * bad data, but it's not clear how that would help debugging.
     */
    for (i = 0; i < sizes[0]; i++)
        for (j = 0; j < sizes[1]; j++)
            if (dset1[i][j] != dset2[i][j]) {
                *are_same = false;
                return SUCCEED;
            }

    return SUCCEED;

} /* end compare_2D_arrays() */

/*-------------------------------------------------------------------------
 * Function:  ensure_filter_works
 *
 * Purpose:   Tests writing entire data and partial data with filters
 *
 * Return:    SUCCEED/FAIL
 *
 *-------------------------------------------------------------------------
 */
static herr_t
ensure_filter_works(hid_t fid, const char *name, hid_t dcpl_id)
{
    hid_t  did           = H5I_INVALID_HID; /* Dataset ID                                   */
    hid_t  dxpl_id       = H5I_INVALID_HID; /* Dataset xfer property list ID                */
    hid_t  write_dxpl_id = H5I_INVALID_HID; /* Dataset xfer property list ID for writing    */
    hid_t  sid           = H5I_INVALID_HID; /* Dataspace ID                                 */
    void  *tconv_buf     = NULL;            /* Temporary conversion buffer                  */
    int  **orig          = NULL;            /* Data written to the dataset                  */
    int  **read          = NULL;            /* Data read from the dataset                   */
    size_t r, c;                            /* Data rows and columns                        */
    size_t hs_r, hs_c, hs_offr, hs_offc;    /* Hypserslab sizes and offsets                 */
    size_t i, j;                            /* Local index variables                        */
    int    n = 0;                           /* Value written to point array                 */
    bool   are_same;                        /* Output from dataset compare function         */
    int ***save_array = NULL;               /* (Global) array where the final data go       */

    /* initialize */
    r = (size_t)sizes_g[0];
    c = (size_t)sizes_g[1];

    /* Create the data space */
    if ((sid = H5Screate_simple(2, sizes_g, NULL)) < 0)
        TEST_ERROR;

    /* Allocate memory for the data buffers
     * We're using the hacky way of doing 2D arrays that uses a
     * single data buffer but which allows normal 2D access.
     */
    if (allocate_and_init_2D_array(&orig, sizes_g, NULL) < 0)
        TEST_ERROR;
    if (allocate_and_init_2D_array(&read, sizes_g, NULL) < 0)
        TEST_ERROR;

    /* Create a small conversion buffer to test strip mining. We
     * might as well test all we can!
     */
    if ((dxpl_id = H5Pcreate(H5P_DATASET_XFER)) < 0)
        TEST_ERROR;
    if (NULL == (tconv_buf = calloc((size_t)1000, sizeof(char))))
        TEST_ERROR;
    if (H5Pset_buffer(dxpl_id, (size_t)1000, tconv_buf, NULL) < 0)
        TEST_ERROR;
    if ((write_dxpl_id = H5Pcopy(dxpl_id)) < 0)
        TEST_ERROR;

    TESTING("    filters (setup)");

    /* Check if all the filters are available */
    if (H5Pall_filters_avail(dcpl_id) != true)
        TEST_ERROR;

    /* Create the dataset */
    if ((did = H5Dcreate2(fid, name, H5T_NATIVE_INT, sid, H5P_DEFAULT, dcpl_id, H5P_DEFAULT)) < 0)
        TEST_ERROR;

    PASSED();

    /*----------------------------------------------------------------------
     * STEP 1: Read uninitialized data.  It should be zero.
     *----------------------------------------------------------------------
     */
    TESTING("    filters (uninitialized read)");

    if (H5Dread(did, H5T_NATIVE_INT, H5S_ALL, H5S_ALL, dxpl_id, *read) < 0)
        TEST_ERROR;

    /* The input buffer was calloc'd and has not been initialized yet */
    if (compare_2D_arrays(orig, read, sizes_g, &are_same) < 0)
        TEST_ERROR;
    if (false == are_same)
        TEST_ERROR;

    PASSED();

    /*----------------------------------------------------------------------
     * STEP 2: Test filters by setting up a chunked dataset and writing
     * to it.
     *----------------------------------------------------------------------
     */
    TESTING("    filters (write)");

    n = 0;
    for (i = 0; i < r; i++)
        for (j = 0; j < c; j++)
            orig[i][j] = n++;

    if (H5Dwrite(did, H5T_NATIVE_INT, H5S_ALL, H5S_ALL, write_dxpl_id, *orig) < 0)
        TEST_ERROR;

    PASSED();

    /*----------------------------------------------------------------------
     * STEP 3: Try to read the data we just wrote.
     *----------------------------------------------------------------------
     */
    TESTING("    filters (read)");

    /* Read the dataset back */
    if (H5Dread(did, H5T_NATIVE_INT, H5S_ALL, H5S_ALL, dxpl_id, *read) < 0)
        TEST_ERROR;

    /* Check that the values read are the same as the values written */
    if (compare_2D_arrays(orig, read, sizes_g, &are_same) < 0)
        TEST_ERROR;
    if (false == are_same)
        TEST_ERROR;

    PASSED();

    /*----------------------------------------------------------------------
     * STEP 4: Write new data over the top of the old data.  The new data is
     * random thus not very compressible, and will cause the chunks to move
     * around as they grow.  We only change values for the left half of the
     * dataset although we rewrite the whole thing.
     *----------------------------------------------------------------------
     */
    TESTING("    filters (modify)");

    for (i = 0; i < r; i++)
        for (j = 0; j < c / 2; j++)
            orig[i][j] = (int)HDrandom() % RANDOM_LIMIT;

    if (H5Dwrite(did, H5T_NATIVE_INT, H5S_ALL, H5S_ALL, write_dxpl_id, *orig) < 0)
        TEST_ERROR;

    /* Read the dataset back and check it */
    if (H5Dread(did, H5T_NATIVE_INT, H5S_ALL, H5S_ALL, dxpl_id, *read) < 0)
        TEST_ERROR;

    /* Check that the values read are the same as the values written */
    if (compare_2D_arrays(orig, read, sizes_g, &are_same) < 0)
        TEST_ERROR;
    if (false == are_same)
        TEST_ERROR;

    PASSED();

    /*----------------------------------------------------------------------
     * STEP 5: Close the dataset and then open it and read it again.  This
     * insures that the filters message is picked up properly from the
     * object header.
     *----------------------------------------------------------------------
     */
    TESTING("    filters (re-open)");

    if (H5Dclose(did) < 0)
        TEST_ERROR;
    if ((did = H5Dopen2(fid, name, H5P_DEFAULT)) < 0)
        TEST_ERROR;

    if (H5Dread(did, H5T_NATIVE_INT, H5S_ALL, H5S_ALL, dxpl_id, *read) < 0)
        TEST_ERROR;

    /* Check that the values read are the same as the values written */
    if (compare_2D_arrays(orig, read, sizes_g, &are_same) < 0)
        TEST_ERROR;
    if (false == are_same)
        TEST_ERROR;

    PASSED();

    /*----------------------------------------------------------------------
     * STEP 6: Test partial I/O by writing to and then reading from a
     * hyperslab of the dataset.  The hyperslab does not line up on chunk
     * boundaries (we know that case already works from above tests).
     *----------------------------------------------------------------------
     */
    TESTING("    filters (partial I/O)");

    hs_r    = (size_t)hs_sizes_g[0];
    hs_c    = (size_t)hs_sizes_g[1];
    hs_offr = (size_t)hs_offsets_g[0];
    hs_offc = (size_t)hs_offsets_g[1];
    for (i = 0; i < hs_r; i++)
        for (j = 0; j < hs_c; j++)
            orig[hs_offr + i][hs_offc + j] = (int)HDrandom() % RANDOM_LIMIT;

    if (H5Sselect_hyperslab(sid, H5S_SELECT_SET, hs_offsets_g, NULL, hs_sizes_g, NULL) < 0)
        TEST_ERROR;

    /* Use the "read" DXPL because partial I/O on corrupted data test
     * needs to ignore errors during writing
     */
    if (H5Dwrite(did, H5T_NATIVE_INT, sid, sid, dxpl_id, *orig) < 0)
        TEST_ERROR;

    if (H5Dread(did, H5T_NATIVE_INT, sid, sid, dxpl_id, *read) < 0)
        TEST_ERROR;

    /* Check that the values read are the same as the values written */
    if (compare_2D_arrays(orig, read, sizes_g, &are_same) < 0)
        TEST_ERROR;
    if (false == are_same)
        TEST_ERROR;

    PASSED();

    /* Save the data written to the file for later comparison when the file
     * is reopened for read test.
     */
    if (!strcmp(name, DSET_DEFLATE_NAME))
        save_array = &orig_deflate_g;
    else if (!strcmp(name, DSET_FILTER1_NAME))
        save_array = &orig_dynlib1_g;
    else if (!strcmp(name, DSET_FILTER2_NAME))
        save_array = &orig_dynlib2_g;
    else if (!strcmp(name, DSET_FILTER3_NAME))
        save_array = &orig_dynlib4_g;
    else
        TEST_ERROR;
    if (allocate_and_init_2D_array(save_array, sizes_g, orig) < 0)
        TEST_ERROR;

    /* Clean up and exit */
    if (H5Dclose(did) < 0)
        TEST_ERROR;
    if (H5Sclose(sid) < 0)
        TEST_ERROR;
    if (H5Pclose(dxpl_id) < 0)
        TEST_ERROR;
    if (H5Pclose(write_dxpl_id) < 0)
        TEST_ERROR;

    free_2D_array(&orig);
    free_2D_array(&read);

    free(tconv_buf);

    return SUCCEED;

error:
    /* Clean up objects used for this test */
    H5E_BEGIN_TRY
    {
        H5Dclose(did);
        H5Sclose(sid);
        H5Pclose(dxpl_id);
        H5Pclose(write_dxpl_id);
    }
    H5E_END_TRY

    /* NULLs are okay here */
    free_2D_array(&orig);
    free_2D_array(&read);

    if (tconv_buf)
        free(tconv_buf);

    return FAIL;
} /* end ensure_filter_works() */

/*-------------------------------------------------------------------------
 * Function:  test_dataset_write_with_filters
 *
 * Purpose:   Tests creating datasets and writing data with dynamically loaded filters
 *
 * Return:    SUCCEED/FAIL
 *
 *-------------------------------------------------------------------------
 */
static herr_t
test_dataset_write_with_filters(hid_t fid)
{
    hid_t        dcpl_id = H5I_INVALID_HID; /* Dataset creation property list ID        */
    unsigned int filter1_data;              /* Data used by filter 1                    */
    unsigned int libver_values[4];          /* Used w/ the filter that makes HDF5 calls */
#ifdef H5_HAVE_FILTER_DEFLATE
    unsigned int compress_level; /* Deflate compression level */
#endif

    /*----------------------------------------------------------
     * STEP 1: Test deflation by itself.
     *----------------------------------------------------------
     */
    puts("Testing dataset writes with deflate filter");
#ifdef H5_HAVE_FILTER_DEFLATE
    if ((dcpl_id = H5Pcreate(H5P_DATASET_CREATE)) < 0)
        TEST_ERROR;
    if (H5Pset_chunk(dcpl_id, 2, chunk_sizes_g) < 0)
        TEST_ERROR;
    compress_level = 6;
    if (H5Pset_deflate(dcpl_id, compress_level) < 0)
        TEST_ERROR;

    /* Ensure the filter works */
    if (ensure_filter_works(fid, DSET_DEFLATE_NAME, dcpl_id) < 0)
        TEST_ERROR;

    /* Clean up objects used for this test */
    if (H5Pclose(dcpl_id) < 0)
        TEST_ERROR;
#else  /* H5_HAVE_FILTER_DEFLATE */
    SKIPPED();
    puts("    Deflate filter not enabled");
#endif /* H5_HAVE_FILTER_DEFLATE */

    /*----------------------------------------------------------
     * STEP 2: Test filter plugin 1 by itself.
     *----------------------------------------------------------
     */
    puts("    dataset writes with filter plugin 1");
    if ((dcpl_id = H5Pcreate(H5P_DATASET_CREATE)) < 0)
        TEST_ERROR;
    if (H5Pset_chunk(dcpl_id, 2, chunk_sizes_g) < 0)
        TEST_ERROR;

    /* Set up the filter, passing in the amount the filter will add and subtract
     * from each data element. Note that this value has an arbitrary max of 9.
     */
    filter1_data = 9;
    if (H5Pset_filter(dcpl_id, FILTER1_ID, H5Z_FLAG_MANDATORY, (size_t)1, &filter1_data) < 0)
        TEST_ERROR;

    /* Ensure the filter works */
    if (ensure_filter_works(fid, DSET_FILTER1_NAME, dcpl_id) < 0)
        TEST_ERROR;

    /* Clean up objects used for this test */
    if (H5Pclose(dcpl_id) < 0)
        TEST_ERROR;

    /* Unregister the dynamic filter for testing purpose. The next time when this test is run for
     * the new file format, the library's H5PL code has to search in the table of loaded plugin libraries
     * for this filter.
     */
    if (H5Zunregister(FILTER1_ID) < 0)
        TEST_ERROR;

    /*----------------------------------------------------------
     * STEP 3: Test filter plugin 2 by itself.
     *----------------------------------------------------------
     */
    puts("    dataset writes with filter plugin 2");
    if ((dcpl_id = H5Pcreate(H5P_DATASET_CREATE)) < 0)
        TEST_ERROR;
    if (H5Pset_chunk(dcpl_id, 2, chunk_sizes_g) < 0)
        TEST_ERROR;
    if (H5Pset_filter(dcpl_id, FILTER2_ID, H5Z_FLAG_MANDATORY, 0, NULL) < 0)
        TEST_ERROR;

    /* Ensure the filter works */
    if (ensure_filter_works(fid, DSET_FILTER2_NAME, dcpl_id) < 0)
        TEST_ERROR;

    /* Clean up objects used for this test */
    if (H5Pclose(dcpl_id) < 0)
        TEST_ERROR;

    /* Unregister the dynamic filter for testing purpose. The next time when this test is run for
     * the new file format, the library's H5PL code has to search in the table of loaded plugin libraries
     * for this filter.
     */
    if (H5Zunregister(FILTER2_ID) < 0)
        TEST_ERROR;

    /*----------------------------------------------------------
     * STEP 4: Test filter plugin 3 by itself.
     *         (This filter plugin makes HDF5 API calls)
     *----------------------------------------------------------
     */
    puts("    dataset writes with filter plugin 3");
    if ((dcpl_id = H5Pcreate(H5P_DATASET_CREATE)) < 0)
        TEST_ERROR;
    if (H5Pset_chunk(dcpl_id, 2, chunk_sizes_g) < 0)
        TEST_ERROR;

    /* Set the add/subtract value for the filter */
    libver_values[0] = 9;

    /* Get the library bounds and add to the filter data */
    if (H5get_libversion(&libver_values[1], &libver_values[2], &libver_values[3]) < 0)
        TEST_ERROR;
    if (H5Pset_filter(dcpl_id, FILTER3_ID, H5Z_FLAG_MANDATORY, (size_t)4, libver_values) < 0)
        TEST_ERROR;

    /* Ensure the filter works */
    if (ensure_filter_works(fid, DSET_FILTER3_NAME, dcpl_id) < 0)
        TEST_ERROR;

    /* Clean up objects used for this test */
    if (H5Pclose(dcpl_id) < 0)
        TEST_ERROR;

    /* Unregister the dynamic filter for testing purpose. The next time when this test is run for
     * the new file format, the library's H5PL code has to search in the table of loaded plugin libraries
     * for this filter.
     */
    if (H5Zunregister(FILTER3_ID) < 0)
        TEST_ERROR;

    return SUCCEED;

error:
    /* Clean up objects used for this test */
    H5E_BEGIN_TRY
    {
        H5Pclose(dcpl_id);
    }
    H5E_END_TRY

    return FAIL;
} /* end test_dataset_write_with_filters() */

/*-------------------------------------------------------------------------
 * Function:  test_read_data
 *
 * Purpose:   Tests reading data and compares values
 *
 * Return:    SUCCEED/FAIL
 *
 *-------------------------------------------------------------------------
 */
static herr_t
test_read_data(hid_t did, int *origin_data)
{
    int  **check  = NULL;
    int   *data_p = origin_data;
    size_t i, j; /* Local index variables */

    if (allocate_and_init_2D_array(&check, sizes_g, NULL) < 0)
        TEST_ERROR;

    /* Read the dataset back */
    if (H5Dread(did, H5T_NATIVE_INT, H5S_ALL, H5S_ALL, H5P_DEFAULT, *check) < 0)
        TEST_ERROR;

    /* Check that the values read are the same as the values written */
    for (i = 0; i < sizes_g[0]; i++)
        for (j = 0; j < sizes_g[1]; j++) {
            if (*data_p != check[i][j])
                TEST_ERROR;
            data_p++;
        }

    free_2D_array(&check);

    PASSED();

    return SUCCEED;

error:
    free_2D_array(&check);

    return FAIL;
} /* end test_read_data() */

/*-------------------------------------------------------------------------
 * Function:  test_dataset_read_with_filters
 *
 * Purpose:   Tests reading datasets created with dynamically-loaded
 *            filter plugins.
 *
 * Return:    SUCCEED/FAIL
 *
 *-------------------------------------------------------------------------
 */
static herr_t
test_dataset_read_with_filters(hid_t fid)
{
    hid_t did = H5I_INVALID_HID; /* Dataset ID */

    /*----------------------------------------------------------
     * STEP 1: Test deflation by itself.
     *----------------------------------------------------------
     */
    TESTING("dataset read I/O with deflate filter");

#ifdef H5_HAVE_FILTER_DEFLATE
    if (H5Zfilter_avail(H5Z_FILTER_DEFLATE) != true)
        TEST_ERROR;

    if ((did = H5Dopen2(fid, DSET_DEFLATE_NAME, H5P_DEFAULT)) < 0)
        TEST_ERROR;

    if (test_read_data(did, orig_deflate_g[0]) < 0)
        TEST_ERROR;

    if (H5Dclose(did) < 0)
        TEST_ERROR;

#else  /* H5_HAVE_FILTER_DEFLATE */
    SKIPPED();
    puts("    Deflate filter not enabled");
#endif /* H5_HAVE_FILTER_DEFLATE */

    /*----------------------------------------------------------
     * STEP 2: Test filter plugin 1 by itself.
     *----------------------------------------------------------
     */
    TESTING("    dataset reads with filter plugin 1");

    if ((did = H5Dopen2(fid, DSET_FILTER1_NAME, H5P_DEFAULT)) < 0)
        TEST_ERROR;

    if (test_read_data(did, orig_dynlib1_g[0]) < 0)
        TEST_ERROR;

    if (H5Dclose(did) < 0)
        TEST_ERROR;

    /*----------------------------------------------------------
     * STEP 3: Test filter plugin 2 by itself.
     *----------------------------------------------------------
     */
    TESTING("    dataset reads with filter plugin 2");

    if ((did = H5Dopen2(fid, DSET_FILTER2_NAME, H5P_DEFAULT)) < 0)
        TEST_ERROR;

    if (test_read_data(did, orig_dynlib2_g[0]) < 0)
        TEST_ERROR;

    if (H5Dclose(did) < 0)
        TEST_ERROR;

    /*----------------------------------------------------------
     * STEP 4: Test filter plugin 3 by itself.
     *----------------------------------------------------------
     */
    TESTING("    dataset reads with filter plugin 3");

    if ((did = H5Dopen2(fid, DSET_FILTER3_NAME, H5P_DEFAULT)) < 0)
        TEST_ERROR;

    if (test_read_data(did, orig_dynlib4_g[0]) < 0)
        TEST_ERROR;

    if (H5Dclose(did) < 0)
        TEST_ERROR;

    return SUCCEED;

error:
    /* Clean up objects used for this test */
    H5E_BEGIN_TRY
    {
        H5Dclose(did);
    }
    H5E_END_TRY

    return FAIL;
} /* end test_dataset_read_with_filters() */

/*-------------------------------------------------------------------------
 * Function:  ensure_data_read_fails
 *
 * Purpose:   Tests not reading data
 *
 * Return:    SUCCEED/FAIL
 *
 *-------------------------------------------------------------------------
 */
static herr_t
ensure_data_read_fails(hid_t did)
{
    int  **check = NULL;
    herr_t ret   = FAIL;

    if (allocate_and_init_2D_array(&check, sizes_g, NULL) < 0)
        TEST_ERROR;

    /* Read the dataset back (should fail) */
    H5E_BEGIN_TRY
    {
        ret = H5Dread(did, H5T_NATIVE_INT, H5S_ALL, H5S_ALL, H5P_DEFAULT, *check);
    }
    H5E_END_TRY
    if (ret >= 0)
        TEST_ERROR;

    free_2D_array(&check);

    PASSED();

    return SUCCEED;

error:
    free_2D_array(&check);
    return FAIL;
} /* end ensure_data_read_fails() */

/*-------------------------------------------------------------------------
 * Function:  test_no_read_when_plugins_disabled
 *
 * Purpose:   Ensures we can't read data from a dataset that requires a
 *            filter located in a plugin.
 *
 * Return:    SUCCEED/FAIL
 *
 *-------------------------------------------------------------------------
 */
static herr_t
test_no_read_when_plugins_disabled(hid_t fid)
{
    hid_t    did = H5I_INVALID_HID; /* Dataset ID */
    unsigned plugin_flags;          /* Plugin access flags */

    TESTING("filter plugin 1 with filter plugins disabled");

    /* Get the existing plugin flags */
    if (H5PLget_loading_state(&plugin_flags) < 0)
        TEST_ERROR;

    /* Disable filter plugins and use the new flags */
    plugin_flags &= (unsigned)(~H5PL_FILTER_PLUGIN);
    if (H5PLset_loading_state(plugin_flags) < 0)
        TEST_ERROR;

    /* Open a dataset that requires a filter plugin to read the data */
    if ((did = H5Dopen2(fid, DSET_FILTER1_NAME, H5P_DEFAULT)) < 0)
        TEST_ERROR;

    /* Make sure we can't read the data */
    if (ensure_data_read_fails(did) < 0)
        TEST_ERROR;

    /* Close down */
    if (H5Dclose(did) < 0)
        TEST_ERROR;

    /* Re-enable filter plugins */
    plugin_flags |= (unsigned)H5PL_FILTER_PLUGIN;
    if (H5PLset_loading_state(plugin_flags) < 0)
        TEST_ERROR;

    return SUCCEED;

error:
    /* Clean up objects used for this test */
    H5E_BEGIN_TRY
    {
        plugin_flags |= (unsigned)H5PL_FILTER_PLUGIN;
        H5PLset_loading_state(plugin_flags);
        H5Dclose(did);
    }
    H5E_END_TRY

    return FAIL;
} /* end test_no_read_when_plugins_disabled() */

/*-------------------------------------------------------------------------
 * Function:  test_creating_groups_using_plugins
 *
 * Purpose:   Tests creating group with dynamically loaded filters
 *
 * Return:    SUCCEED/FAIL
 *
 *-------------------------------------------------------------------------
 */
static herr_t
test_creating_groups_using_plugins(hid_t fid)
{
    hid_t gcpl_id = H5I_INVALID_HID;
    hid_t gid     = H5I_INVALID_HID;
    hid_t sub_gid = H5I_INVALID_HID;
    int   i;
    char  subgroup_name[256];

    TESTING("creating groups with filter plugin 4");

    if ((gcpl_id = H5Pcreate(H5P_GROUP_CREATE)) < 0)
        TEST_ERROR;

    /* Use a filter plugin for creating groups */
    if (H5Pset_filter(gcpl_id, FILTER4_ID, H5Z_FLAG_MANDATORY, (size_t)0, NULL) < 0)
        TEST_ERROR;

    /* Create a group using this filter */
    if ((gid = H5Gcreate2(fid, TOP_LEVEL_GROUP_NAME, H5P_DEFAULT, gcpl_id, H5P_DEFAULT)) < 0)
        TEST_ERROR;

    /* Create multiple groups under the top-level group */
    for (i = 0; i < N_SUBGROUPS; i++) {
        snprintf(subgroup_name, sizeof(subgroup_name), SUBGROUP_PREFIX "%d", i);

        if ((sub_gid = H5Gcreate2(gid, subgroup_name, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT)) < 0)
            TEST_ERROR;
        if (H5Gclose(sub_gid) < 0)
            TEST_ERROR;
    }

    /* Close everything */
    if (H5Gclose(gid) < 0)
        TEST_ERROR;
    if (H5Pclose(gcpl_id) < 0)
        TEST_ERROR;

    PASSED();

    return SUCCEED;

error:
    /* Clean up objects used for this test */
    H5E_BEGIN_TRY
    {
        H5Gclose(sub_gid);
        H5Gclose(gid);
        H5Pclose(gcpl_id);
    }
    H5E_END_TRY

    return FAIL;
} /* end test_creating_groups_using_plugins() */

/*-------------------------------------------------------------------------
 * Function:    test_opening_groups_using_plugins
 *
 * Purpose:     Tests opening group with dynamically loaded filters
 *
 * Return:      SUCCEED/FAIL
 *
 *-------------------------------------------------------------------------
 */
static herr_t
test_opening_groups_using_plugins(hid_t fid)
{
    hid_t gid     = H5I_INVALID_HID;
    hid_t sub_gid = H5I_INVALID_HID;
    int   i;
    char  subgroup_name[256];

    TESTING("opening groups with filter plugin 4");

    /* Open the top group */
    if ((gid = H5Gopen2(fid, TOP_LEVEL_GROUP_NAME, H5P_DEFAULT)) < 0)
        TEST_ERROR;

    /* Open all the sub-groups under the top-level group */
    for (i = 0; i < N_SUBGROUPS; i++) {
        snprintf(subgroup_name, sizeof(subgroup_name), SUBGROUP_PREFIX "%d", i);

        if ((sub_gid = H5Gopen2(gid, subgroup_name, H5P_DEFAULT)) < 0)
            TEST_ERROR;
        if (H5Gclose(sub_gid) < 0)
            TEST_ERROR;
    }

    /* Close the top-level group */
    if (H5Gclose(gid) < 0)
        TEST_ERROR;

    PASSED();

    return SUCCEED;

error:
    /* Clean up objects used for this test */
    H5E_BEGIN_TRY
    {
        H5Gclose(gid);
        H5Gclose(sub_gid);
    }
    H5E_END_TRY

    return FAIL;
} /* end test_opening_groups_using_plugins() */

/*-------------------------------------------------------------------------
 * Function:  test_path_api_calls
 *
 * Purpose:   Tests the H5PL API calls that manipulate the plugin search
 *            paths.
 *
 * Return:    SUCCEED/FAIL
 *
 *-------------------------------------------------------------------------
 */
static herr_t
test_path_api_calls(void)
{
    unsigned int n_starting_paths;
    unsigned int u;
    unsigned int n_paths;
    herr_t       ret;
    ssize_t      path_len = -1;
    char         path[256];
    char         temp_name[256];

    puts("Testing access to the filter path table");

    if (H5Zfilter_avail(FILTER1_ID) != true)
        TEST_ERROR;

    /* Set the number of paths to create for this test.
     *
     * This should be set high enough to ensure that at least one array
     * expansion will take place. See H5PLpath.c for details.
     */
    n_starting_paths = 42;

    /****************/
    /* H5PLremove() */
    /****************/

    /* Remove all the current paths */
    TESTING("    remove");

    /* Get the current size */
    if (H5PLsize(&n_paths) < 0)
        TEST_ERROR;

    /* Remove all existing paths */
    for (u = n_paths; u > 0; u--)
        if (H5PLremove(u - 1) < 0) {
            fprintf(stderr, "    at %u: %s\n", u, path);
            TEST_ERROR;
        }

    /* Verify the table is empty */
    if (H5PLsize(&n_paths) < 0)
        TEST_ERROR;
    if (n_paths > 0)
        TEST_ERROR;

    PASSED();

    TESTING("    remove (index 0 in empty table)");

    /* Try to remove index zero in an empty list (SHOULD FAIL) */
    H5E_BEGIN_TRY
    {
        ret = H5PLremove(0);
    }
    H5E_END_TRY
    if (ret >= 0)
        TEST_ERROR;

    PASSED();

    /****************/
    /* H5PLappend() */
    /****************/

    TESTING("    append");

    /* Add a bunch of paths to the path table */
    for (u = 0; u < n_starting_paths; u++) {
        snprintf(path, sizeof(path), "a_path_%u", u);
        if (H5PLappend(path) < 0) {
            fprintf(stderr, "    at %u: %s\n", u, path);
            TEST_ERROR;
        }
    }

    PASSED();

    /**********************/
    /* H5PLremove() again */
    /**********************/

    TESTING("    remove (index too high)");

    /* Try to remove a path where the index is beyond the table capacity (SHOULD FAIL) */
    H5E_BEGIN_TRY
    {
        ret = H5PLremove(n_starting_paths);
    }
    H5E_END_TRY

    if (ret >= 0)
        TEST_ERROR;

    PASSED();

    /*************/
    /* H5PLget() */
    /*************/

    TESTING("    get (path name)");

    /* Get the path length by passing in NULL */
    if ((path_len = H5PLget(0, NULL, 0)) <= 0) {
        fprintf(stderr, "    get path 0 length failed\n");
        TEST_ERROR;
    }
    if (path_len != 8)
        TEST_ERROR;

    /* Get the path */
    if ((path_len = H5PLget(0, path, 256)) <= 0) {
        fprintf(stderr, "    get 0 len: %zd : %s\n", path_len, path);
        TEST_ERROR;
    }
    if (strcmp(path, "a_path_0") != 0) {
        fprintf(stderr, "    get 0: %s\n", path);
        TEST_ERROR;
    }

    PASSED();

    TESTING("    get (high and low indices)");

    /* Get path at index 1 */
    if ((path_len = H5PLget(1, path, 256)) <= 0)
        TEST_ERROR;
    if (strcmp(path, "a_path_1") != 0) {
        fprintf(stderr, "    get 1: %s\n", path);
        TEST_ERROR;
    }

    /* Get path at the last index */
    if ((path_len = H5PLget(n_starting_paths - 1, path, 256)) <= 0)
        TEST_ERROR;
    snprintf(temp_name, sizeof(temp_name), "a_path_%u", n_starting_paths - 1);
    if (strcmp(path, temp_name) != 0) {
        fprintf(stderr, "    get %u: %s\n", n_starting_paths - 1, path);
        TEST_ERROR;
    }

    PASSED();

    TESTING("    get (index too high)");

    /* Get path at the last + 1 index (SHOULD FAIL) */
    H5E_BEGIN_TRY
    {
        path_len = H5PLget(n_starting_paths, NULL, 0);
    }
    H5E_END_TRY
    if (path_len > 0)
        TEST_ERROR;

    PASSED();

    /*****************/
    /* H5PLprepend() */
    /*****************/

    /* We'll remove a path at an arbitrary index and then
     * prepend a new path.
     */

    TESTING("    remove (arbitrary index 1)");

    /* Remove one path */
    if (H5PLremove(8) < 0)
        TEST_ERROR;

    /* Verify that the entries were moved */
    if ((path_len = H5PLget(8, path, 256)) <= 0)
        TEST_ERROR;
    if (strcmp(path, "a_path_9") != 0) {
        fprintf(stderr, "    get 8: %s\n", path);
        TEST_ERROR;
    }

    /* Verify the table shrank */
    if (H5PLsize(&n_paths) < 0)
        TEST_ERROR;
    if (n_paths != n_starting_paths - 1)
        TEST_ERROR;

    PASSED();

    TESTING("    prepend");

    /* Prepend one path */
    snprintf(path, sizeof(path), "a_path_%d", n_starting_paths + 1);
    if (H5PLprepend(path) < 0) {
        fprintf(stderr, "    prepend %u: %s\n", n_starting_paths + 1, path);
        TEST_ERROR;
    }

    /* Verify the table increased */
    if (H5PLsize(&n_paths) < 0)
        TEST_ERROR;
    if (n_paths != n_starting_paths)
        TEST_ERROR;

    /* Verify that the entries were moved */
    if (H5PLget(8, path, 256) <= 0)
        TEST_ERROR;
    if (strcmp(path, "a_path_7") != 0) {
        fprintf(stderr, "    get 8: %s\n", path);
        TEST_ERROR;
    }

    /* Verify that the path was inserted at index zero */
    if (H5PLget(0, path, 256) <= 0)
        TEST_ERROR;
    snprintf(temp_name, sizeof(temp_name), "a_path_%d", n_starting_paths + 1);
    if (strcmp(path, temp_name) != 0) {
        fprintf(stderr, "    get 0: %s\n", path);
        TEST_ERROR;
    }

    PASSED();

    /*****************/
    /* H5PLreplace() */
    /*****************/

    TESTING("    replace");

    /* Replace one path at index 1 */
    snprintf(path, sizeof(path), "a_path_%u", n_starting_paths + 4);
    if (H5PLreplace(path, 1) < 0) {
        fprintf(stderr, "    replace 1: %s\n", path);
        TEST_ERROR;
    }

    /* Verify the table size remained the same */
    if (H5PLsize(&n_paths) < 0)
        TEST_ERROR;
    if (n_paths != n_starting_paths)
        TEST_ERROR;

    /* Verify that the entries were not moved by
     * inspecting the paths at indices +/- 1.
     */

    /* Check path at index 0 */
    if (H5PLget(0, path, 256) <= 0)
        TEST_ERROR;
    snprintf(temp_name, sizeof(temp_name), "a_path_%u", n_starting_paths + 1);
    if (strcmp(path, temp_name) != 0) {
        fprintf(stderr, "    get 0: %s\n", path);
        TEST_ERROR;
    }

    /* Check path at index 2 */
    if (H5PLget(2, path, 256) <= 0)
        TEST_ERROR;
    if (strcmp(path, "a_path_1") != 0) {
        fprintf(stderr, "    get 2: %s\n", path);
        TEST_ERROR;
    }

    PASSED();

    /****************/
    /* H5PLinsert() */
    /****************/

    /* We'll remove a path at an arbitrary index and then
     * insert a new path.
     */

    TESTING("    remove (arbitrary index 2)");

    /* Remove one path */
    if (H5PLremove(4) < 0)
        TEST_ERROR;

    /* Verify that the entries were moved */
    if (H5PLget(4, path, 256) <= 0)
        TEST_ERROR;
    if (strcmp(path, "a_path_4") != 0) {
        fprintf(stderr, "    get 4: %s\n", path);
        TEST_ERROR;
    }

    /* Verify the table size */
    if (H5PLsize(&n_paths) < 0)
        TEST_ERROR;
    if (n_paths != n_starting_paths - 1)
        TEST_ERROR;
    PASSED();

    TESTING("    insert");

    /* Insert one path at index 3*/
    snprintf(path, sizeof(path), "a_path_%d", n_starting_paths + 5);
    if (H5PLinsert(path, 3) < 0) {
        fprintf(stderr, "    insert 3: %s\n", path);
        TEST_ERROR;
    }

    /* Verify that the entries were moved */
    if (H5PLget(4, path, 256) <= 0)
        TEST_ERROR;
    if (strcmp(path, "a_path_2") != 0) {
        fprintf(stderr, "    get 4: %s\n", path);
        TEST_ERROR;
    }

    /* Verify the table size increased */
    if (H5PLsize(&n_paths) < 0)
        TEST_ERROR;
    if (n_paths != n_starting_paths)
        TEST_ERROR;

    PASSED();

    /****************/
    /* H5PLremove() */
    /****************/

    /* Remove all the current paths */
    TESTING("    remove (all)");

    /* Get the current size */
    if (H5PLsize(&n_paths) < 0)
        TEST_ERROR;

    /* Remove all existing paths */
    for (u = n_paths; u > 0; u--)
        if (H5PLremove(u - 1) < 0) {
            fprintf(stderr, "    at %u: %s\n", u, path);
            TEST_ERROR;
        }

    /* Verify the table is empty */
    if (H5PLsize(&n_paths) < 0)
        TEST_ERROR;
    if (n_paths > 0)
        TEST_ERROR;

    PASSED();

    return SUCCEED;

error:
    return FAIL;
} /* end test_path_api_calls() */

/*-------------------------------------------------------------------------
 * Function:  test_filter_numbers
 *
 * Purpose:   Tests the filter numbers are handled correctly
 *
 * Return:    SUCCEED/FAIL
 *
 *-------------------------------------------------------------------------
 */
static herr_t
test_filter_numbers(void)
{
    hid_t        dcpl_id = H5I_INVALID_HID;
    H5Z_filter_t id;
    herr_t       status = SUCCEED;
    size_t       nelmts = 0;
    unsigned int flags;
    unsigned int filter_config;

    puts("Testing filter number handling");

    /* Check that out-of-range filter numbers are handled correctly */
    TESTING("    Filter # out of range");

    /* Create property list */
    if ((dcpl_id = H5Pcreate(H5P_DATASET_CREATE)) < 0)
        TEST_ERROR;

    nelmts = 0;

    /* Test id > H5Z_FILTER_MAX and < 0, current version */

    H5E_BEGIN_TRY
    {
        id     = H5Z_FILTER_MAX + 1;
        status = H5Pget_filter_by_id2(dcpl_id, id, &flags, &nelmts, NULL, 0, NULL, &filter_config);
    }
    H5E_END_TRY

    /* Should fail */
    if (status != FAIL)
        TEST_ERROR;

    H5E_BEGIN_TRY
    {
        id     = -1;
        status = H5Pget_filter_by_id2(dcpl_id, id, &flags, &nelmts, NULL, 0, NULL, &filter_config);
    }
    H5E_END_TRY

    /* Should fail */
    if (status != FAIL)
        TEST_ERROR;

        /* Test id > H5Z_FILTER_MAX and < 0, deprecated version */

#ifndef H5_NO_DEPRECATED_SYMBOLS
    H5E_BEGIN_TRY
    {
        id     = H5Z_FILTER_MAX + 1;
        status = H5Pget_filter_by_id1(dcpl_id, id, &flags, &nelmts, NULL, 0, NULL);
    }
    H5E_END_TRY

    /* Should fail */
    if (status != FAIL)
        TEST_ERROR;

    H5E_BEGIN_TRY
    {
        id     = -1;
        status = H5Pget_filter_by_id1(dcpl_id, id, &flags, &nelmts, NULL, 0, NULL);
    }
    H5E_END_TRY

    /* Should fail */
    if (status != FAIL)
        TEST_ERROR;
#endif

    if (H5Pclose(dcpl_id) < 0)
        TEST_ERROR;

    PASSED();

    return SUCCEED;

error:
    H5E_BEGIN_TRY
    {
        H5Pclose(dcpl_id);
    }
    H5E_END_TRY
    return FAIL;
} /* end test_filter_numbers() */

/*-------------------------------------------------------------------------
 * Function:  disable_chunk_cache
 *
 * Purpose:   Turns the chunk cache off
 *
 * Return:    SUCCEED/FAIL
 *
 *-------------------------------------------------------------------------
 */
static herr_t
disable_chunk_cache(hid_t fapl_id)
{
    int    mdc_nelmts;
    size_t rdcc_nelmts;
    size_t rdcc_nbytes;
    double rdcc_w0;

    if (H5Pget_cache(fapl_id, &mdc_nelmts, &rdcc_nelmts, &rdcc_nbytes, &rdcc_w0) < 0)
        TEST_ERROR;
    rdcc_nbytes = 0;
    if (H5Pset_cache(fapl_id, mdc_nelmts, rdcc_nelmts, rdcc_nbytes, rdcc_w0) < 0)
        TEST_ERROR;

    return SUCCEED;
error:
    return FAIL;
} /* end disable_chunk_cache() */

/*-------------------------------------------------------------------------
 * Function:    main
 *
 * Purpose:     Tests the plugin module (H5PL)
 *
 * Return:      EXIT_SUCCESS/EXIT_FAILURE
 *
 *-------------------------------------------------------------------------
 */
int
main(void)
{
    char     filename[FILENAME_BUF_SIZE];
    hid_t    fid            = H5I_INVALID_HID;
    hid_t    old_ff_fapl_id = H5I_INVALID_HID;
    hid_t    new_ff_fapl_id = H5I_INVALID_HID;
    unsigned new_format;
    int      nerrors = 0;

    /*******************************************************************/
    /* ENSURE THAT WRITING TO DATASETS AND CREATING GROUPS WORKS       */
    /*******************************************************************/
    /* Test with old & new format groups */
    for (new_format = false; new_format <= true; new_format++) {
        hid_t my_fapl_id;

        /* Testing setup */
        h5_test_init();

        /* Get a VFD-dependent filename */
        if ((old_ff_fapl_id = h5_fileaccess()) < 0)
            TEST_ERROR;

        /* Turn off the chunk cache, so all the chunks are immediately written to disk */
        if (disable_chunk_cache(old_ff_fapl_id) < 0)
            TEST_ERROR;

        /* Fix up the filename for the VFD */
        h5_fixname(FILENAME[0], old_ff_fapl_id, filename, sizeof(filename));

        /* Set the FAPL for the type of format */
        if (new_format) {
            puts("\nTesting with new file format:");
            /* Copy the file access property list and set the latest file format on it */
            if ((new_ff_fapl_id = H5Pcopy(old_ff_fapl_id)) < 0)
                TEST_ERROR;
            if (H5Pset_libver_bounds(new_ff_fapl_id, H5F_LIBVER_LATEST, H5F_LIBVER_LATEST) < 0)
                TEST_ERROR;

            my_fapl_id = new_ff_fapl_id;
        }
        else {
            puts("Testing with old file format:");
            my_fapl_id = old_ff_fapl_id;
        }

        /* Create the file for this test */
        if ((fid = H5Fcreate(filename, H5F_ACC_TRUNC, H5P_DEFAULT, my_fapl_id)) < 0)
            TEST_ERROR;

        /* Test creating datasets and writing to them using plugin filters */
        nerrors += (test_dataset_write_with_filters(fid) < 0 ? 1 : 0);

        /* Test creating groups using dynamically-loaded plugin filters */
        nerrors += (test_creating_groups_using_plugins(fid) < 0 ? 1 : 0);

        if (H5Fclose(fid) < 0)
            TEST_ERROR;

        /* Close FAPLs */
        if (H5Pclose(old_ff_fapl_id) < 0)
            TEST_ERROR;
        if (new_format) {
            if (H5Pclose(new_ff_fapl_id) < 0)
                TEST_ERROR;
        }

        /* Restore the default error handler (set in h5_test_init()) */
        h5_restore_err();

        /*******************************************************************/
        /* ENSURE THAT READING FROM DATASETS AND OPENING GROUPS WORKS      */
        /*******************************************************************/

        puts("\nTesting reading data with with dynamic plugin filters:");

        /* Close the library so that all loaded plugin libraries are unloaded */
        h5_test_init();
        if ((old_ff_fapl_id = h5_fileaccess()) < 0)
            TEST_ERROR;

        /* Set the FAPL for the type of format */
        if (new_format) {
            /* Copy the file access property list and set the latest file format on it */
            if ((new_ff_fapl_id = H5Pcopy(old_ff_fapl_id)) < 0)
                TEST_ERROR;
            if (H5Pset_libver_bounds(new_ff_fapl_id, H5F_LIBVER_LATEST, H5F_LIBVER_LATEST) < 0)
                TEST_ERROR;

            my_fapl_id = new_ff_fapl_id;
        }
        else
            my_fapl_id = old_ff_fapl_id;

        /* Add extra path to check for correct error process */
        if (H5PLprepend("bogus") < 0) {
            fprintf(stderr, "Could not prepend path:bogus\n");
            TEST_ERROR;
        }

        /* Reopen the file for testing data reading */
        if ((fid = H5Fopen(filename, H5F_ACC_RDONLY, my_fapl_id)) < 0)
            TEST_ERROR;

        /* Read the data with filters */
        nerrors += (test_dataset_read_with_filters(fid) < 0 ? 1 : 0);

        /* Test creating groups using dynamically-loaded plugin filters */
        nerrors += (test_opening_groups_using_plugins(fid) < 0 ? 1 : 0);

        /* Close FAPLs */
        if (H5Pclose(old_ff_fapl_id) < 0)
            TEST_ERROR;
        if (new_format) {
            if (H5Pclose(new_ff_fapl_id) < 0)
                TEST_ERROR;
        }

        /* Restore the default error handler (set in h5_test_init()) */
        h5_restore_err();

        /*******************************************************************/
        /* ENSURE THAT DISABLING FILTER PLUGINS VIA THE FILTER FLAGS WORKS */
        /*******************************************************************/

        /* Close the library so that all loaded plugin libraries are unloaded */
        h5_test_init();
        if ((old_ff_fapl_id = h5_fileaccess()) < 0)
            TEST_ERROR;

        /* Set the FAPL for the type of format */
        if (new_format) {
            /* Copy the file access property list and set the latest file format on it */
            if ((new_ff_fapl_id = H5Pcopy(old_ff_fapl_id)) < 0)
                TEST_ERROR;
            if (H5Pset_libver_bounds(new_ff_fapl_id, H5F_LIBVER_LATEST, H5F_LIBVER_LATEST) < 0)
                TEST_ERROR;

            my_fapl_id = new_ff_fapl_id;
        }
        else
            my_fapl_id = old_ff_fapl_id;

        /* Reopen the file for testing data reading */
        if ((fid = H5Fopen(filename, H5F_ACC_RDONLY, my_fapl_id)) < 0)
            TEST_ERROR;

        /* When filters are disabled, make sure we can't read data from a
         * dataset that requires a filter plugin.
         */
        nerrors += (test_no_read_when_plugins_disabled(fid) < 0 ? 1 : 0);

        if (H5Fclose(fid) < 0)
            TEST_ERROR;

        /*********************/
        /* CLEAN UP          */
        /*********************/
        /* Close FAPLs */
        if (new_format) {
            if (H5Pclose(new_ff_fapl_id) < 0)
                TEST_ERROR;
        }
        else {
            /* Restore the default error handler (set in h5_test_init()) */
            h5_restore_err();

            if (H5Pclose(old_ff_fapl_id) < 0)
                TEST_ERROR;
        }

        /* Free up saved arrays */
        free_2D_array(&orig_deflate_g);
        free_2D_array(&orig_dynlib1_g);
        free_2D_array(&orig_dynlib2_g);
        free_2D_array(&orig_dynlib4_g);
    } /* end for */

    h5_cleanup(FILENAME, old_ff_fapl_id);

    /************************************/
    /* TEST THE FILTER PLUGIN API CALLS */
    /************************************/

    /* Test the APIs for access to the filter plugin path table */
    nerrors += (test_path_api_calls() < 0 ? 1 : 0);

    /* Test filter numbers */
    nerrors += (test_filter_numbers() < 0 ? 1 : 0);

    if (nerrors)
        TEST_ERROR;

    printf("All plugin tests passed.\n");

    exit(EXIT_SUCCESS);

error:
    H5E_BEGIN_TRY
    {
        H5Fclose(fid);
        H5Pclose(old_ff_fapl_id);
        H5Pclose(new_ff_fapl_id);
    }
    H5E_END_TRY

    /* Free up saved arrays (NULLs okay) */
    free_2D_array(&orig_deflate_g);
    free_2D_array(&orig_dynlib1_g);
    free_2D_array(&orig_dynlib2_g);
    free_2D_array(&orig_dynlib4_g);

    nerrors = MAX(1, nerrors);
    printf("***** %d PLUGIN TEST%s FAILED! *****\n", nerrors, 1 == nerrors ? "" : "S");
    exit(EXIT_FAILURE);
} /* end main() */
