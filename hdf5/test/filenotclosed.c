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
 * Purpose:	Test to verify that the assertion/abort failure is fixed when the
 *          application does not close the file. (See HDFFV-10160)
 */

#include "h5test.h"

#define FILENAME     "filenotclosed"
#define DATASET_NAME "dset"

/*-------------------------------------------------------------------------
 * Function:    catch_signal
 *
 * Purpose:     The signal handler to catch the SIGABRT signal.
 *
 * Return:      No return
 *
 *-------------------------------------------------------------------------
 */
static void
catch_signal(int H5_ATTR_UNUSED signo)
{
    exit(EXIT_FAILURE);
} /* catch_signal() */

/*-------------------------------------------------------------------------
 * Function:	main
 *
 * Purpose:	Test to verify the following problem described in HDFFV-10160 is fixed:
 *          "a.out: H5Fint.c:1679: H5F_close: Assertion `f->file_id > 0' failed."
 *
 * Return:	Success:	exit(EXIT_SUCCESS)
 *		    Failure:	exit(EXIT_FAILURE)
 *
 *-------------------------------------------------------------------------
 */
int
main(void)
{
    hid_t       fapl         = H5I_INVALID_HID; /* File access property lists */
    hid_t       fid          = H5I_INVALID_HID; /* File ID */
    hid_t       did          = H5I_INVALID_HID; /* Dataset ID */
    hid_t       dcpl         = H5I_INVALID_HID; /* Dataset creation property list */
    hid_t       sid          = H5I_INVALID_HID; /* Dataspace ID */
    hsize_t     cur_dim[1]   = {5};             /* Current dimension sizes */
    hsize_t     max_dim[1]   = {H5S_UNLIMITED}; /* Maximum dimension sizes */
    hsize_t     chunk_dim[1] = {10};            /* Chunk dimension sizes */
    int         buf[5]       = {1, 2, 3, 4, 5}; /* The data to be written to the dataset */
    char        filename[100];                  /* File name */
    const char *driver_name;                    /* File Driver value from environment */
    bool        contig_addr_vfd;                /* Contiguous address vfd */

    /* Get the VFD to use */
    driver_name = h5_get_test_driver_name();

    /* Skip test when using VFDs that has different address spaces for each
     * type of metadata allocation.
     * Further investigation is needed to resolve the test failure with the
     * split/multi driver.  Please see HDFFV-10160.
     */
    contig_addr_vfd = (bool)(strcmp(driver_name, "split") != 0 && strcmp(driver_name, "multi") != 0);
    if (!contig_addr_vfd) {
        SKIPPED();
        puts("    Temporary skipped for a spilt/multi driver");
        exit(EXIT_SUCCESS);
    }

    h5_test_init();

    /* To exit from the file for SIGABRT signal */
    if (signal(SIGABRT, catch_signal) == SIG_ERR)
        TEST_ERROR;

    fapl = h5_fileaccess();
    h5_fixname(FILENAME, fapl, filename, sizeof(filename));

    /* Set to latest format */
    if (H5Pset_libver_bounds(fapl, H5F_LIBVER_LATEST, H5F_LIBVER_LATEST) < 0)
        TEST_ERROR;

    /* Create the file  */
    if ((fid = H5Fcreate(filename, H5F_ACC_TRUNC, H5P_DEFAULT, fapl)) < 0)
        TEST_ERROR;

    /* Create the dcpl and set the chunk size */
    if ((dcpl = H5Pcreate(H5P_DATASET_CREATE)) < 0)
        TEST_ERROR;

    if (H5Pset_chunk(dcpl, 1, chunk_dim) < 0)
        TEST_ERROR;

    /* Create the dataspace */
    if ((sid = H5Screate_simple(1, cur_dim, max_dim)) < 0)
        TEST_ERROR;

    /* Create the dataset */
    if ((did = H5Dcreate2(fid, DATASET_NAME, H5T_NATIVE_INT, sid, H5P_DEFAULT, dcpl, H5P_DEFAULT)) < 0)
        TEST_ERROR;

    /* Write to the dataset */
    if (H5Dwrite(did, H5T_NATIVE_INT, H5S_ALL, H5S_ALL, H5P_DEFAULT, buf) < 0)
        TEST_ERROR;

    /* Close the dataset */
    if (H5Dclose(did) < 0)
        TEST_ERROR;

    /* Close the dataspace */
    if (H5Sclose(sid) < 0)
        TEST_ERROR;

    /* Close the property lists */
    if (H5Pclose(dcpl) < 0)
        TEST_ERROR;
    if (H5Pclose(fapl) < 0)
        TEST_ERROR;

    /* The file is not closed. */
    /* The library will call H5_term_library to shut down the library. */

    exit(EXIT_SUCCESS);

error:
    puts("*** TEST FAILED ***");
    exit(EXIT_FAILURE);
}
