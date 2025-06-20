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
 * Purpose:    Test local heaps used by symbol tables (groups).
 */
#include "h5test.h"
#include "H5srcdir.h"
#include "H5ACprivate.h"
#include "H5CXprivate.h" /* API Contexts                         */
#include "H5HLprivate.h"
#include "H5Iprivate.h"
#include "H5VLprivate.h" /* Virtual Object Layer                     */

static const char *FILENAME[] = {"lheap", NULL};

#define TESTFILE "tsizeslheap.h5"

#define NOBJS 40

/*-------------------------------------------------------------------------
 * Function:    main
 *
 * Purpose:     Create a file, create a local heap, write data into the local
 *              heap, close the file, open the file, read data out of the
 *              local heap, close the file.
 *
 * Return:      EXIT_SUCCESS/EXIT_FAILURE
 *
 *-------------------------------------------------------------------------
 */
int
main(void)
{
    hid_t       fapl = H5P_DEFAULT;     /* file access properties   */
    hid_t       file = H5I_INVALID_HID; /* hdf5 file                */
    H5F_t      *f    = NULL;            /* hdf5 file pointer        */
    char        filename[1024];         /* file name                */
    haddr_t     heap_addr;              /* local heap address       */
    H5HL_t     *heap = NULL;            /* local heap               */
    size_t      obj[NOBJS];             /* offsets within the heap  */
    int         i, j;                   /* miscellaneous counters   */
    char        buf[1024];              /* the value to store       */
    const char *s;                      /* value to read            */
    bool        api_ctx_pushed = false; /* Whether API context pushed */
    bool        driver_is_default_compatible;

    /* Reset library */
    h5_test_init();
    fapl = h5_fileaccess();

    /* Push API context */
    if (H5CX_push() < 0)
        FAIL_STACK_ERROR;
    api_ctx_pushed = true;

    /*
     * Test writing to the heap...
     */
    TESTING("local heap write");
    h5_fixname(FILENAME[0], fapl, filename, sizeof filename);
    if (FAIL == (file = H5Fcreate(filename, H5F_ACC_TRUNC, H5P_DEFAULT, fapl)))
        goto error;
    if (NULL == (f = (H5F_t *)H5VL_object(file))) {
        H5_FAILED();
        H5Eprint2(H5E_DEFAULT, stdout);
        goto error;
    }
    if (FAIL == H5AC_ignore_tags(f)) {
        H5_FAILED();
        H5Eprint2(H5E_DEFAULT, stdout);
        goto error;
    }
    if (FAIL == H5HL_create(f, (size_t)0, &heap_addr /*out*/)) {
        H5_FAILED();
        H5Eprint2(H5E_DEFAULT, stdout);
        goto error;
    }
    if (NULL == (heap = H5HL_protect(f, heap_addr, H5AC__NO_FLAGS_SET))) {
        H5_FAILED();
        H5Eprint2(H5E_DEFAULT, stdout);
        goto error;
    }
    for (i = 0; i < NOBJS; i++) {
        snprintf(buf, sizeof(buf), "%03d-", i);
        for (j = 4; j < i; j++)
            buf[j] = (char)('0' + j % 10);
        if (j > 4)
            buf[j] = '\0';

        if (H5HL_insert(f, heap, strlen(buf) + 1, buf, &obj[i]) < 0) {
            H5_FAILED();
            H5Eprint2(H5E_DEFAULT, stdout);
            goto error;
        }
    }
    if (FAIL == H5HL_unprotect(heap)) {
        H5_FAILED();
        H5Eprint2(H5E_DEFAULT, stdout);
        goto error;
    }
    if (FAIL == H5Fclose(file))
        goto error;
    PASSED();

    /*
     * Test reading from the heap...
     */

    TESTING("local heap read");
    h5_fixname(FILENAME[0], fapl, filename, sizeof filename);
    if (FAIL == (file = H5Fopen(filename, H5F_ACC_RDONLY, fapl)))
        goto error;
    if (NULL == (f = (H5F_t *)H5VL_object(file))) {
        H5_FAILED();
        H5Eprint2(H5E_DEFAULT, stdout);
        goto error;
    }
    if (FAIL == H5AC_ignore_tags(f)) {
        H5_FAILED();
        H5Eprint2(H5E_DEFAULT, stdout);
        goto error;
    }
    for (i = 0; i < NOBJS; i++) {
        snprintf(buf, sizeof(buf), "%03d-", i);
        for (j = 4; j < i; j++)
            buf[j] = (char)('0' + j % 10);
        if (j > 4)
            buf[j] = '\0';

        if (NULL == (heap = H5HL_protect(f, heap_addr, H5AC__READ_ONLY_FLAG))) {
            H5_FAILED();
            H5Eprint2(H5E_DEFAULT, stdout);
            goto error;
        }

        if (NULL == (s = (const char *)H5HL_offset_into(heap, obj[i]))) {
            H5_FAILED();
            H5Eprint2(H5E_DEFAULT, stdout);
            goto error;
        }

        if (strcmp(s, buf) != 0) {
            H5_FAILED();
            printf("    i=%d, heap offset=%lu\n", i, (unsigned long)(obj[i]));
            printf("    got: \"%s\"\n", s);
            printf("    ans: \"%s\"\n", buf);
            goto error;
        }

        if (FAIL == H5HL_unprotect(heap)) {
            H5_FAILED();
            H5Eprint2(H5E_DEFAULT, stdout);
            goto error;
        }
    }

    if (FAIL == H5Fclose(file))
        goto error;
    PASSED();

    if (h5_driver_is_default_vfd_compatible(H5P_DEFAULT, &driver_is_default_compatible) < 0)
        TEST_ERROR;

    if (driver_is_default_compatible) {
        /* Check opening existing file non-default sizes of lengths and addresses */
        TESTING("opening pre-created file with non-default sizes");
        {
            const char *testfile = H5_get_srcdir_filename(TESTFILE); /* Corrected test file name */
            hid_t       dset     = H5I_INVALID_HID;
            file                 = H5Fopen(testfile, H5F_ACC_RDONLY, H5P_DEFAULT);
            if (file >= 0) {
                if ((dset = H5Dopen2(file, "/Dataset1", H5P_DEFAULT)) < 0)
                    TEST_ERROR;
                if (H5Dclose(dset) < 0)
                    TEST_ERROR;
                if (H5Fclose(file) < 0)
                    TEST_ERROR;
            }
            else {
                H5_FAILED();
                printf("***cannot open the pre-created non-default sizes test file (%s)\n", testfile);
                goto error;
            } /* end else */
        }
        PASSED();
    }

    /* Verify symbol table messages are cached */
    if (h5_verify_cached_stabs(FILENAME, fapl) < 0)
        TEST_ERROR;

    /* Pop API context */
    if (api_ctx_pushed && H5CX_pop(false) < 0)
        FAIL_STACK_ERROR;
    api_ctx_pushed = false;

    puts("All local heap tests passed.");
    h5_cleanup(FILENAME, fapl);

    return EXIT_SUCCESS;

error:
    puts("*** TESTS FAILED ***");
    H5E_BEGIN_TRY
    {
        H5Fclose(file);
    }
    H5E_END_TRY

    if (api_ctx_pushed)
        H5CX_pop(false);

    return EXIT_FAILURE;
}
