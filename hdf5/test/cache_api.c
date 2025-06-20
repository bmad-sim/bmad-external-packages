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
 *        This file contains tests for the API calls associated
 *        with the cache implemented in H5C.c
 */

#include "cache_common.h"

/* extern declarations */

/* global variable declarations: */

static const char *FILENAME[] = {"cache_api_test", NULL};

/* macro definitions */

/* private function declarations: */

static bool                 check_fapl_mdc_api_calls(unsigned paged, hid_t fcpl_id);
static bool                 check_file_mdc_api_calls(unsigned paged, hid_t fcpl_id);
static bool                 mdc_api_call_smoke_check(int express_test, unsigned paged, hid_t fcpl_id);
static H5AC_cache_config_t *init_invalid_configs(void);
static bool                 check_fapl_mdc_api_errs(void);
static bool                 check_file_mdc_api_errs(unsigned paged, hid_t fcpl_id);

/**************************************************************************/
/**************************************************************************/
/********************************* tests: *********************************/
/**************************************************************************/
/**************************************************************************/

/*-------------------------------------------------------------------------
 * Function:    check_fapl_mdc_api_calls()
 *
 * Purpose:     Verify that the file access property list related
 *              metadata cache related API calls are functioning
 *              correctly.
 *
 *              Since we have tested the H5C code elsewhere, it should
 *              be sufficient to verify that the desired configuration
 *              data is getting to the cache.
 *
 * Return:      Test pass status (true/false)
 *
 *-------------------------------------------------------------------------
 */
static bool
check_fapl_mdc_api_calls(unsigned paged, hid_t fcpl_id)
{
    char                filename[512];
    herr_t              result;
    hid_t               fapl_id        = H5I_INVALID_HID;
    hid_t               test_fapl_id   = H5I_INVALID_HID;
    hid_t               file_id        = H5I_INVALID_HID;
    H5F_t              *file_ptr       = NULL;
    H5C_t              *cache_ptr      = NULL;
    H5AC_cache_config_t default_config = H5AC__DEFAULT_CACHE_CONFIG;
    H5AC_cache_config_t mod_config     = {
        /* int         version                = */ H5AC__CURR_CACHE_CONFIG_VERSION,
        /* bool     rpt_fcn_enabled        = */ false,
        /* bool     open_trace_file        = */ false,
        /* bool     close_trace_file       = */ false,
        /* char        trace_file_name[]      = */ "",
        /* bool     evictions_enabled      = */ true,
        /* bool     set_initial_size       = */ true,
        /* size_t      initial_size           = */ (1 * 1024 * 1024 + 1),
        /* double      min_clean_fraction     = */ 0.2,
        /* size_t      max_size               = */ (16 * 1024 * 1024 + 1),
        /* size_t      min_size               = */ (1 * 1024 * 1024 + 1),
        /* long int    epoch_length           = */ 50001,
        /* enum H5C_cache_incr_mode incr_mode = */ H5C_incr__threshold,
        /* double      lower_hr_threshold     = */ 0.91,
        /* double      increment              = */ 2.1,
        /* bool     apply_max_increment    = */ true,
        /* size_t      max_increment          = */ (4 * 1024 * 1024 + 1),
        /* enum H5C_cache_flash_incr_mode       */
        /*                    flash_incr_mode = */ H5C_flash_incr__off,
        /* double      flash_multiple         = */ 2.0,
        /* double      flash_threshold        = */ 0.5,
        /* enum H5C_cache_decr_mode decr_mode = */ H5C_decr__age_out,
        /* double      upper_hr_threshold     = */ 0.998,
        /* double      decrement              = */ 0.91,
        /* bool     apply_max_decrement    = */ true,
        /* size_t      max_decrement          = */ (1 * 1024 * 1024 - 1),
        /* int         epochs_before_eviction = */ 4,
        /* bool     apply_empty_reserve    = */ true,
        /* double      empty_reserve          = */ 0.05,
        /* int         dirty_bytes_threshold  = */ (256 * 1024),
        /* int        metadata_write_strategy = */
        H5AC__DEFAULT_METADATA_WRITE_STRATEGY};
    H5AC_cache_config_t scratch;
    H5C_auto_size_ctl_t default_auto_size_ctl;
    H5C_auto_size_ctl_t mod_auto_size_ctl;

    if (paged)
        TESTING("MDC/FAPL related API calls for paged aggregation strategy");
    else
        TESTING("MDC/FAPL related API calls");

    pass = true;

    XLATE_EXT_TO_INT_MDC_CONFIG(default_auto_size_ctl, default_config)
    XLATE_EXT_TO_INT_MDC_CONFIG(mod_auto_size_ctl, mod_config)

    /* Create a FAPL and verify that it contains the default
     * initial mdc configuration
     */

    if (pass) {

        fapl_id = H5Pcreate(H5P_FILE_ACCESS);

        if (fapl_id < 0) {

            pass         = false;
            failure_mssg = "H5Pcreate(H5P_FILE_ACCESS) failed.\n";
        }
    }

    if (pass) {

        scratch.version = H5C__CURR_AUTO_SIZE_CTL_VER;

        result = H5Pget_mdc_config(fapl_id, &scratch);

        if (result < 0) {

            pass         = false;
            failure_mssg = "H5Pget_mdc_config() failed.\n";
        }
        else if (!CACHE_CONFIGS_EQUAL(default_config, scratch, true, true)) {

            pass         = false;
            failure_mssg = "retrieved config doesn't match default.";
        }
    }

    /* Modify the initial mdc configuration in a FAPL, and verify that
     * the changes can be read back
     */

    if (pass) {

        result = H5Pset_mdc_config(fapl_id, &mod_config);

        if (result < 0) {

            pass         = false;
            failure_mssg = "H5Pset_mdc_config() failed.\n";
        }
    }

    if (pass) {

        scratch.version = H5C__CURR_AUTO_SIZE_CTL_VER;

        result = H5Pget_mdc_config(fapl_id, &scratch);

        if (result < 0) {

            pass         = false;
            failure_mssg = "H5Pget_mdc_config() failed.\n";
        }
        else if (!CACHE_CONFIGS_EQUAL(mod_config, scratch, true, true)) {

            pass         = false;
            failure_mssg = "retrieved config doesn't match mod config.";
        }
    }

    if (pass) {

        if (H5Pclose(fapl_id) < 0) {

            pass         = false;
            failure_mssg = "H5Pclose() failed.\n";
        }
    }

    /* Open a file using the default FAPL.  Verify that the resulting
     * metadata cache uses the default configuration as well.  Get a
     * copy of the FAPL from the file, and verify that it contains the
     * default initial meta data cache configuration.  Close and delete
     * the file.
     */

    /* setup the file name */
    if (pass) {

        if (h5_fixname(FILENAME[0], H5P_DEFAULT, filename, sizeof(filename)) == NULL) {

            pass         = false;
            failure_mssg = "h5_fixname() failed.\n";
        }
    }

    /* create the file using the default FAPL */
    if (pass) {

        file_id = H5Fcreate(filename, H5F_ACC_TRUNC, fcpl_id, H5P_DEFAULT);

        if (file_id < 0) {

            pass         = false;
            failure_mssg = "H5Fcreate() failed.\n";
        }
    }

    /* get a pointer to the files internal data structure */
    if (pass) {

        file_ptr = (H5F_t *)H5VL_object_verify(file_id, H5I_FILE);

        if (file_ptr == NULL) {

            pass         = false;
            failure_mssg = "Can't get file_ptr.\n";
        }
        else {

            cache_ptr = file_ptr->shared->cache;
        }
    }

    /* verify that we can access the internal version of the cache config */
    if (pass) {

        if (cache_ptr == NULL || cache_ptr->resize_ctl.version != H5C__CURR_AUTO_SIZE_CTL_VER) {

            pass         = false;
            failure_mssg = "Can't access cache resize_ctl.\n";
        }
    }

    /* compare the cache's internal configuration with the expected value */
    if (pass) {

        if (!resize_configs_are_equal(&default_auto_size_ctl, &cache_ptr->resize_ctl, true)) {

            pass         = false;
            failure_mssg = "Unexpected value(s) in cache resize_ctl 1.\n";
        }
    }

    /* get a copy of the files FAPL */
    if (pass) {

        fapl_id = H5Fget_access_plist(file_id);

        if (fapl_id < 0) {

            pass         = false;
            failure_mssg = "H5Fget_access_plist() failed.\n";
        }
    }

    /* compare the initial cache config from the copy of the file's FAPL
     * to the expected value.  If all goes well, close the copy of the FAPL.
     */
    if (pass) {

        scratch.version = H5C__CURR_AUTO_SIZE_CTL_VER;

        result = H5Pget_mdc_config(fapl_id, &scratch);

        if (result < 0) {

            pass         = false;
            failure_mssg = "H5Pget_mdc_config() failed.\n";
        }
        else if (!CACHE_CONFIGS_EQUAL(default_config, scratch, true, true)) {

            pass         = false;
            failure_mssg = "config retrieved from file doesn't match default.";
        }
        else if (H5Pclose(fapl_id) < 0) {

            pass         = false;
            failure_mssg = "H5Pclose() failed.\n";
        }
    }

    /* close the file and delete it */
    if (pass) {

        if (H5Fclose(file_id) < 0) {

            pass         = false;
            failure_mssg = "H5Fclose() failed.\n";
        }
        else if (H5Fdelete(filename, H5P_DEFAULT) < 0) {

            pass         = false;
            failure_mssg = "H5Fdelete() failed.\n";
        }
    }

    /* Open a file using a FAPL with a modified initial metadata cache
     * configuration.  Verify that the resulting metadata cache uses the
     * modified configuration as well.  Get a copy of the FAPL from the
     * file, and verify that it contains the modified initial meta data
     * cache configuration.  Close and delete the file.
     */

    /* Create a FAPL */
    if (pass) {

        fapl_id = H5Pcreate(H5P_FILE_ACCESS);

        if (fapl_id < 0) {

            pass         = false;
            failure_mssg = "H5Pcreate(H5P_FILE_ACCESS) failed.\n";
        }
    }

    /* Modify the initial mdc configuration in the FAPL. */

    if (pass) {

        result = H5Pset_mdc_config(fapl_id, &mod_config);

        if (result < 0) {

            pass         = false;
            failure_mssg = "H5Pset_mdc_config() failed.\n";
        }
    }

    /* setup the file name */
    if (pass) {

        if (h5_fixname(FILENAME[0], H5P_DEFAULT, filename, sizeof(filename)) == NULL) {

            pass         = false;
            failure_mssg = "h5_fixname() failed.\n";
        }
    }

    /* create the file using the modified FAPL */
    if (pass) {

        file_id = H5Fcreate(filename, H5F_ACC_TRUNC, fcpl_id, fapl_id);

        if (file_id < 0) {

            pass         = false;
            failure_mssg = "H5Fcreate() failed.\n";
        }
    }

    /* get a pointer to the files internal data structure */
    if (pass) {

        file_ptr = (H5F_t *)H5VL_object_verify(file_id, H5I_FILE);

        if (file_ptr == NULL) {

            pass         = false;
            failure_mssg = "Can't get file_ptr.\n";
        }
        else {

            cache_ptr = file_ptr->shared->cache;
        }
    }

    /* verify that we can access the internal version of the cache config */
    if (pass) {

        if (cache_ptr == NULL || cache_ptr->resize_ctl.version != H5C__CURR_AUTO_SIZE_CTL_VER) {

            pass         = false;
            failure_mssg = "Can't access cache resize_ctl.\n";
        }
    }

    /* compare the cache's internal configuration with the expected value */
    if (pass) {

        if (!resize_configs_are_equal(&mod_auto_size_ctl, &cache_ptr->resize_ctl, true)) {

            pass         = false;
            failure_mssg = "Unexpected value(s) in cache resize_ctl 2.\n";
        }
    }

    /* get a copy of the files FAPL */
    if (pass) {

        test_fapl_id = H5Fget_access_plist(file_id);

        if (test_fapl_id < 0) {

            pass         = false;
            failure_mssg = "H5Fget_access_plist() failed.\n";
        }
    }

    /* compare the initial cache config from the copy of the file's FAPL
     * to the expected value.  If all goes well, close the copy of the FAPL.
     */
    if (pass) {

        scratch.version = H5C__CURR_AUTO_SIZE_CTL_VER;

        result = H5Pget_mdc_config(test_fapl_id, &scratch);

        if (result < 0) {

            pass         = false;
            failure_mssg = "H5Pget_mdc_config() failed.\n";
        }
        else if (!CACHE_CONFIGS_EQUAL(mod_config, scratch, true, true)) {

            pass         = false;
            failure_mssg = "config retrieved from file doesn't match.";
        }
        else if (H5Pclose(test_fapl_id) < 0) {

            pass         = false;
            failure_mssg = "H5Pclose() failed.\n";
        }
    }

    /* close the file and delete it */
    if (pass) {

        if (H5Fclose(file_id) < 0) {

            pass         = false;
            failure_mssg = "H5Fclose() failed.\n";
        }
        else if (H5Fdelete(filename, fapl_id) < 0) {

            pass         = false;
            failure_mssg = "H5Fdelete() failed.\n";
        }
    }

    /* close the fapl used to create the file */
    if (pass) {

        if (H5Pclose(fapl_id) < 0) {

            pass         = false;
            failure_mssg = "H5Pclose() failed.\n";
        }
    }

    if (pass) {

        PASSED();
    }
    else {

        H5_FAILED();
    }

    if (!pass) {

        fprintf(stdout, "%s: failure_mssg = \"%s\".\n", __func__, failure_mssg);
    }

    return pass;

} /* check_fapl_mdc_api_calls() */

/*-------------------------------------------------------------------------
 * Function:    check_file_mdc_api_calls()
 *
 * Purpose:     Verify that the file related metadata cache API calls are
 *              functioning correctly.
 *
 *              Since we have tested the H5C code elsewhere, it should
 *              be sufficient to verify that the desired configuration
 *              data is getting in and out of the cache.  Similarly,
 *              we need only verify that the cache monitoring calls
 *              return the data that the cache thinks they should return.
 *              We shouldn't need to verify data correctness beyond that
 *              point.
 *
 * Return:      Test pass status (true/false)
 *
 *-------------------------------------------------------------------------
 */
static bool
check_file_mdc_api_calls(unsigned paged, hid_t fcpl_id)
{
    char                filename[512];
    hid_t               file_id = H5I_INVALID_HID;
    size_t              max_size;
    size_t              min_clean_size;
    size_t              cur_size;
    int                 cur_num_entries;
    double              hit_rate;
    H5AC_cache_config_t default_config = H5AC__DEFAULT_CACHE_CONFIG;
    H5AC_cache_config_t mod_config_1   = {
        /* int         version                = */ H5C__CURR_AUTO_SIZE_CTL_VER,
        /* bool     rpt_fcn_enabled        = */ false,
        /* bool     open_trace_file        = */ false,
        /* bool     close_trace_file       = */ false,
        /* char        trace_file_name[]      = */ "",
        /* bool     evictions_enabled      = */ true,
        /* bool     set_initial_size       = */ true,
        /* size_t      initial_size           = */ (1 * 1024 * 1024 + 1),
        /* double      min_clean_fraction     = */ 0.2,
        /* size_t      max_size               = */ (16 * 1024 * 1024 + 1),
        /* size_t      min_size               = */ (1 * 1024 * 1024 + 1),
        /* long int    epoch_length           = */ 50001,
        /* enum H5C_cache_incr_mode incr_mode = */ H5C_incr__threshold,
        /* double      lower_hr_threshold     = */ 0.91,
        /* double      increment              = */ 2.1,
        /* bool     apply_max_increment    = */ true,
        /* size_t      max_increment          = */ (4 * 1024 * 1024 + 1),
        /* enum H5C_cache_flash_incr_mode       */
        /*                    flash_incr_mode = */ H5C_flash_incr__off,
        /* double      flash_multiple         = */ 2.0,
        /* double      flash_threshold        = */ 0.5,
        /* enum H5C_cache_decr_mode decr_mode = */ H5C_decr__age_out,
        /* double      upper_hr_threshold     = */ 0.998,
        /* double      decrement              = */ 0.91,
        /* bool     apply_max_decrement    = */ true,
        /* size_t      max_decrement          = */ (1 * 1024 * 1024 - 1),
        /* int         epochs_before_eviction = */ 4,
        /* bool     apply_empty_reserve    = */ true,
        /* double      empty_reserve          = */ 0.05,
        /* int         dirty_bytes_threshold  = */ (256 * 1024),
        /* int        metadata_write_strategy = */
        H5AC__DEFAULT_METADATA_WRITE_STRATEGY};
    H5AC_cache_config_t mod_config_2 = {
        /* int         version                = */ H5C__CURR_AUTO_SIZE_CTL_VER,
        /* bool     rpt_fcn_enabled        = */ true,
        /* bool     open_trace_file        = */ false,
        /* bool     close_trace_file       = */ false,
        /* char        trace_file_name[]      = */ "",
        /* bool     evictions_enabled      = */ true,
        /* bool     set_initial_size       = */ true,
        /* size_t      initial_size           = */ (512 * 1024),
        /* double      min_clean_fraction     = */ 0.1,
        /* size_t      max_size               = */ (8 * 1024 * 1024),
        /* size_t      min_size               = */ (512 * 1024),
        /* long int    epoch_length           = */ 25000,
        /* enum H5C_cache_incr_mode incr_mode = */ H5C_incr__threshold,
        /* double      lower_hr_threshold     = */ 0.9,
        /* double      increment              = */ 2.0,
        /* bool     apply_max_increment    = */ true,
        /* size_t      max_increment          = */ (2 * 1024 * 1024),
        /* enum H5C_cache_flash_incr_mode       */
        /*                    flash_incr_mode = */ H5C_flash_incr__off,
        /* double      flash_multiple         = */ 1.5,
        /* double      flash_threshold        = */ 0.4,
        /* enum H5C_cache_decr_mode decr_mode = */ H5C_decr__threshold,
        /* double      upper_hr_threshold     = */ 0.9995,
        /* double      decrement              = */ 0.95,
        /* bool     apply_max_decrement    = */ true,
        /* size_t      max_decrement          = */ (512 * 1024),
        /* int         epochs_before_eviction = */ 4,
        /* bool     apply_empty_reserve    = */ true,
        /* double      empty_reserve          = */ 0.05,
        /* int         dirty_bytes_threshold  = */ (256 * 1024),
        /* int        metadata_write_strategy = */
        H5AC__DEFAULT_METADATA_WRITE_STRATEGY};
    H5AC_cache_config_t mod_config_3 = {
        /* int         version                = */ H5C__CURR_AUTO_SIZE_CTL_VER,
        /* bool     rpt_fcn_enabled        = */ false,
        /* bool     open_trace_file        = */ false,
        /* bool     close_trace_file       = */ false,
        /* char        trace_file_name[]      = */ "",
        /* bool     evictions_enabled      = */ true,
        /* bool     set_initial_size       = */ true,
        /* size_t      initial_size           = */ (1 * 1024 * 1024),
        /* double      min_clean_fraction     = */ 0.2,
        /* size_t      max_size               = */ (16 * 1024 * 1024),
        /* size_t      min_size               = */ (1 * 1024 * 1024),
        /* long int    epoch_length           = */ 50000,
        /* enum H5C_cache_incr_mode incr_mode = */ H5C_incr__off,
        /* double      lower_hr_threshold     = */ 0.90,
        /* double      increment              = */ 2.0,
        /* bool     apply_max_increment    = */ true,
        /* size_t      max_increment          = */ (4 * 1024 * 1024),
        /* enum H5C_cache_flash_incr_mode       */
        /*                    flash_incr_mode = */ H5C_flash_incr__off,
        /* double      flash_multiple         = */ 2.1,
        /* double      flash_threshold        = */ 0.6,
        /* enum H5C_cache_decr_mode decr_mode = */ H5C_decr__off,
        /* double      upper_hr_threshold     = */ 0.999,
        /* double      decrement              = */ 0.9,
        /* bool     apply_max_decrement    = */ false,
        /* size_t      max_decrement          = */ (1 * 1024 * 1024 - 1),
        /* int         epochs_before_eviction = */ 3,
        /* bool     apply_empty_reserve    = */ false,
        /* double      empty_reserve          = */ 0.05,
        /* int         dirty_bytes_threshold  = */ (256 * 1024),
        /* int        metadata_write_strategy = */
        H5AC__DEFAULT_METADATA_WRITE_STRATEGY};
    H5AC_cache_config_t mod_config_4 = {
        /* int         version                = */ H5C__CURR_AUTO_SIZE_CTL_VER,
        /* bool     rpt_fcn_enabled        = */ false,
        /* bool     open_trace_file        = */ false,
        /* bool     close_trace_file       = */ false,
        /* char        trace_file_name[]      = */ "",
        /* bool     evictions_enabled      = */ true,
        /* bool     set_initial_size       = */ true,
        /* size_t      initial_size           = */ (1 * 1024 * 1024),
        /* double      min_clean_fraction     = */ 0.15,
        /* size_t      max_size               = */ (20 * 1024 * 1024),
        /* size_t      min_size               = */ (1 * 1024 * 1024),
        /* long int    epoch_length           = */ 75000,
        /* enum H5C_cache_incr_mode incr_mode = */ H5C_incr__threshold,
        /* double      lower_hr_threshold     = */ 0.9,
        /* double      increment              = */ 2.0,
        /* bool     apply_max_increment    = */ true,
        /* size_t      max_increment          = */ (2 * 1024 * 1024),
        /* enum H5C_cache_flash_incr_mode       */
        /*                    flash_incr_mode = */ H5C_flash_incr__off,
        /* double      flash_multiple         = */ 1.1,
        /* double      flash_threshold        = */ 0.3,
        /* enum H5C_cache_decr_mode decr_mode = */
        H5C_decr__age_out_with_threshold,
        /* double      upper_hr_threshold     = */ 0.999,
        /* double      decrement              = */ 0.9,
        /* bool     apply_max_decrement    = */ true,
        /* size_t      max_decrement          = */ (1 * 1024 * 1024),
        /* int         epochs_before_eviction = */ 3,
        /* bool     apply_empty_reserve    = */ true,
        /* double      empty_reserve          = */ 0.1,
        /* int         dirty_bytes_threshold  = */ (256 * 1024),
        /* int        metadata_write_strategy = */
        H5AC__DEFAULT_METADATA_WRITE_STRATEGY};

    if (paged)
        TESTING("MDC/FILE related API calls for paged aggregation strategy");
    else
        TESTING("MDC/FILE related API calls");

    pass = true;

    /* Open a file with the default FAPL.  Verify that the cache is
     * configured as per the default both by looking at its internal
     * configuration, and via the H5Fget_mdc_config() call.
     *
     * Then set several different configurations, and verify that
     * they took as per above.
     */

    /* setup the file name */
    if (pass) {

        if (h5_fixname(FILENAME[0], H5P_DEFAULT, filename, sizeof(filename)) == NULL) {

            pass         = false;
            failure_mssg = "h5_fixname() failed.\n";
        }
    }

    /* create the file using the default FAPL */
    if (pass) {

        file_id = H5Fcreate(filename, H5F_ACC_TRUNC, fcpl_id, H5P_DEFAULT);

        if (file_id < 0) {

            pass         = false;
            failure_mssg = "H5Fcreate() failed.\n";
        }
    }

    /* verify that the cache is set to the default config */
    validate_mdc_config(file_id, &default_config, true, 1);

    /* set alternate config 1 */
    if (pass) {

        if (H5Fset_mdc_config(file_id, &mod_config_1) < 0) {

            pass         = false;
            failure_mssg = "H5Fset_mdc_config() failed 1.\n";
        }
    }

    /* verify that the cache is now set to the alternate config */
    validate_mdc_config(file_id, &mod_config_1, true, 2);

    /* set alternate config 2 */
    if (pass) {

        if (H5Fset_mdc_config(file_id, &mod_config_2) < 0) {

            pass         = false;
            failure_mssg = "H5Fset_mdc_config() failed 2.\n";
        }
    }

    /* verify that the cache is now set to the alternate config */
    validate_mdc_config(file_id, &mod_config_2, true, 3);

    /* set alternate config 3 */
    if (pass) {

        if (H5Fset_mdc_config(file_id, &mod_config_3) < 0) {

            pass         = false;
            failure_mssg = "H5Fset_mdc_config() failed 3.\n";
        }
    }

    /* verify that the cache is now set to the alternate config */
    validate_mdc_config(file_id, &mod_config_3, true, 4);

    /* set alternate config 4 */
    if (pass) {

        if (H5Fset_mdc_config(file_id, &mod_config_4) < 0) {

            pass         = false;
            failure_mssg = "H5Fset_mdc_config() failed 4.\n";
        }
    }

    /* verify that the cache is now set to the alternate config */
    validate_mdc_config(file_id, &mod_config_4, true, 5);

    /* Run some quick smoke checks on the cache status monitoring
     * calls -- no interesting data as the cache hasn't had a
     * chance to do much yet.
     */

    if (pass) {

        if (H5Fget_mdc_hit_rate(file_id, &hit_rate) < 0) {

            pass         = false;
            failure_mssg = "H5Fget_mdc_hit_rate() failed 1.\n";
        }
        else if (!H5_DBL_ABS_EQUAL(hit_rate, 0.0)) {

            pass         = false;
            failure_mssg = "H5Fget_mdc_hit_rate() returned unexpected hit rate.\n";
        }
    }

    if (pass) {

        if (H5Fget_mdc_size(file_id, &max_size, &min_clean_size, &cur_size, &cur_num_entries) < 0) {

            pass         = false;
            failure_mssg = "H5Fget_mdc_size() failed 1.\n";
        }
        else if ((mod_config_4.initial_size != max_size) ||
                 (min_clean_size != (size_t)((double)max_size * mod_config_4.min_clean_fraction))) {

            pass         = false;
            failure_mssg = "H5Fget_mdc_size() returned unexpected value(s).\n";
        }
    }

    /* close the file and delete it */
    if (pass) {

        if (H5Fclose(file_id) < 0) {

            pass         = false;
            failure_mssg = "H5Fclose() failed.\n";
        }
        else if (H5Fdelete(filename, H5P_DEFAULT) < 0) {

            pass         = false;
            failure_mssg = "H5Fdelete() failed.\n";
        }
    }

    if (pass) {

        PASSED();
    }
    else {

        H5_FAILED();
    }

    if (!pass) {

        fprintf(stdout, "%s: failure_mssg = \"%s\".\n", __func__, failure_mssg);
    }

    return pass;

} /* check_file_mdc_api_calls() */

/*-------------------------------------------------------------------------
 * Function:    mdc_api_call_smoke_check()
 *
 * Purpose:     Initial basic functional test to see if there are problems
 *              with the cache API calls.
 *
 *              NOTE: This test takes some time to run and checks the
 *                    testing express level value.
 *
 * Return:      Test pass status (true/false)
 *
 *-------------------------------------------------------------------------
 */

#define CHUNK_SIZE          2
#define DSET_SIZE           (200 * CHUNK_SIZE)
#define NUM_DSETS           6
#define NUM_RANDOM_ACCESSES 200000

static bool
mdc_api_call_smoke_check(int express_test, unsigned paged, hid_t fcpl_id)
{
    char                filename[512];
    bool                valid_chunk;
    bool                dump_hit_rate   = false;
    int64_t             min_accesses    = 1000;
    double              min_hit_rate    = 0.90;
    bool                dump_cache_size = false;
    hid_t               file_id         = H5I_INVALID_HID;
    hid_t               dataspace_id    = H5I_INVALID_HID;
    hid_t               filespace_ids[NUM_DSETS];
    hid_t               memspace_id = H5I_INVALID_HID;
    hid_t               dataset_ids[NUM_DSETS];
    hid_t               properties = H5I_INVALID_HID;
    char                dset_name[64];
    int                 i, j, k, l, m, n;
    herr_t              status;
    hsize_t             dims[2];
    hsize_t             a_size[2];
    hsize_t             offset[2];
    hsize_t             chunk_size[2];
    int                 data_chunk[CHUNK_SIZE][CHUNK_SIZE];
    H5AC_cache_config_t default_config = H5AC__DEFAULT_CACHE_CONFIG;
    H5AC_cache_config_t mod_config_1   = {
        /* int         version                = */ H5C__CURR_AUTO_SIZE_CTL_VER,
        /* bool     rpt_fcn_enabled        = */ false,
        /* bool     open_trace_file        = */ false,
        /* bool     close_trace_file       = */ false,
        /* char        trace_file_name[]      = */ "",
        /* bool     evictions_enabled      = */ true,
        /* bool     set_initial_size       = */ true,
        /* size_t      initial_size           = */ 500000,
        /* double      min_clean_fraction     = */ 0.1,
        /* size_t      max_size               = */ 16000000,
        /* size_t      min_size               = */ 250000,
        /* long int    epoch_length           = */ 50000,
        /* enum H5C_cache_incr_mode incr_mode = */ H5C_incr__off,
        /* double      lower_hr_threshold     = */ 0.95,
        /* double      increment              = */ 2.0,
        /* bool     apply_max_increment    = */ false,
        /* size_t      max_increment          = */ 4000000,
        /* enum H5C_cache_flash_incr_mode       */
        /*                    flash_incr_mode = */ H5C_flash_incr__off,
        /* double      flash_multiple         = */ 2.0,
        /* double      flash_threshold        = */ 0.5,
        /* enum H5C_cache_decr_mode decr_mode = */ H5C_decr__off,
        /* double      upper_hr_threshold     = */ 0.999,
        /* double      decrement              = */ 0.9,
        /* bool     apply_max_decrement    = */ false,
        /* size_t      max_decrement          = */ 1000000,
        /* int         epochs_before_eviction = */ 2,
        /* bool     apply_empty_reserve    = */ true,
        /* double      empty_reserve          = */ 0.05,
        /* int         dirty_bytes_threshold  = */ (256 * 1024),
        /* int        metadata_write_strategy = */
        H5AC__DEFAULT_METADATA_WRITE_STRATEGY};
    H5AC_cache_config_t mod_config_2 = {
        /* int         version                = */ H5C__CURR_AUTO_SIZE_CTL_VER,
        /* bool     rpt_fcn_enabled        = */ false,
        /* bool     open_trace_file        = */ false,
        /* bool     close_trace_file       = */ false,
        /* char        trace_file_name[]      = */ "",
        /* bool     evictions_enabled      = */ true,
        /* bool     set_initial_size       = */ true,
        /* size_t      initial_size           = */ 12000000,
        /* double      min_clean_fraction     = */ 0.1,
        /* size_t      max_size               = */ 16000000,
        /* size_t      min_size               = */ 250000,
        /* long int    epoch_length           = */ 50000,
        /* enum H5C_cache_incr_mode incr_mode = */ H5C_incr__off,
        /* double      lower_hr_threshold     = */ 0.95,
        /* double      increment              = */ 2.0,
        /* bool     apply_max_increment    = */ false,
        /* size_t      max_increment          = */ 4000000,
        /* enum H5C_cache_flash_incr_mode       */
        /*                    flash_incr_mode = */ H5C_flash_incr__off,
        /* double      flash_multiple         = */ 2.0,
        /* double      flash_threshold        = */ 0.5,
        /* enum H5C_cache_decr_mode decr_mode = */ H5C_decr__off,
        /* double      upper_hr_threshold     = */ 0.999,
        /* double      decrement              = */ 0.9,
        /* bool     apply_max_decrement    = */ false,
        /* size_t      max_decrement          = */ 1000000,
        /* int         epochs_before_eviction = */ 2,
        /* bool     apply_empty_reserve    = */ true,
        /* double      empty_reserve          = */ 0.05,
        /* int         dirty_bytes_threshold  = */ (256 * 1024),
        /* int        metadata_write_strategy = */
        H5AC__DEFAULT_METADATA_WRITE_STRATEGY};
    H5AC_cache_config_t mod_config_3 = {
        /* int         version                = */ H5C__CURR_AUTO_SIZE_CTL_VER,
        /* bool     rpt_fcn_enabled        = */ false,
        /* bool     open_trace_file        = */ false,
        /* bool     close_trace_file       = */ false,
        /* char        trace_file_name[]      = */ "",
        /* bool     evictions_enabled      = */ true,
        /* bool     set_initial_size       = */ true,
        /* size_t      initial_size           = */ 2000000,
        /* double      min_clean_fraction     = */ 0.1,
        /* size_t      max_size               = */ 16000000,
        /* size_t      min_size               = */ 250000,
        /* long int    epoch_length           = */ 50000,
        /* enum H5C_cache_incr_mode incr_mode = */ H5C_incr__off,
        /* double      lower_hr_threshold     = */ 0.95,
        /* double      increment              = */ 2.0,
        /* bool     apply_max_increment    = */ false,
        /* size_t      max_increment          = */ 4000000,
        /* enum H5C_cache_flash_incr_mode       */
        /*                    flash_incr_mode = */ H5C_flash_incr__off,
        /* double      flash_multiple         = */ 2.0,
        /* double      flash_threshold        = */ 0.5,
        /* enum H5C_cache_decr_mode decr_mode = */ H5C_decr__off,
        /* double      upper_hr_threshold     = */ 0.999,
        /* double      decrement              = */ 0.9,
        /* bool     apply_max_decrement    = */ false,
        /* size_t      max_decrement          = */ 1000000,
        /* int         epochs_before_eviction = */ 2,
        /* bool     apply_empty_reserve    = */ true,
        /* double      empty_reserve          = */ 0.05,
        /* int         dirty_bytes_threshold  = */ (256 * 1024),
        /* int        metadata_write_strategy = */
        H5AC__DEFAULT_METADATA_WRITE_STRATEGY};

    if (paged)
        TESTING("MDC API smoke check for paged aggregation strategy");
    else
        TESTING("MDC API smoke check");

    pass = true;

    if (express_test > 0) {

        SKIPPED();

        fprintf(stdout, "     Long tests disabled.\n");

        return pass;
    }

    /* Open a file with the default FAPL.  Verify that the cache is
     * configured as per the default both by looking at its internal
     * configuration, and via the H5Fget_mdc_config() call.
     *
     * Then set the cache to mod_config_1, which fixes cache size at
     * 500000 bytes, and turns off automatic cache resize.
     */

    /* setup the file name */
    if (pass) {

        if (h5_fixname(FILENAME[0], H5P_DEFAULT, filename, sizeof(filename)) == NULL) {

            pass         = false;
            failure_mssg = "h5_fixname() failed.\n";
        }
    }

    /* create the file using the default FAPL */
    if (pass) {

        file_id = H5Fcreate(filename, H5F_ACC_TRUNC, fcpl_id, H5P_DEFAULT);

        if (file_id < 0) {

            pass         = false;
            failure_mssg = "H5Fcreate() failed.\n";
        }
    }

    /* verify that the cache is set to the default config */
    validate_mdc_config(file_id, &default_config, true, 1);

    /* set alternate config 1 */
    if (pass) {

        if (H5Fset_mdc_config(file_id, &mod_config_1) < 0) {

            pass         = false;
            failure_mssg = "H5Fset_mdc_config() failed 1.\n";
        }
    }

    /* verify that the cache is now set to the alternate config */
    validate_mdc_config(file_id, &mod_config_1, true, 2);

    /* create the datasets */
    if (pass) {

        i = 0;

        while ((pass) && (i < NUM_DSETS)) {
            /* create a dataspace for the chunked dataset */
            dims[0]      = DSET_SIZE;
            dims[1]      = DSET_SIZE;
            dataspace_id = H5Screate_simple(2, dims, NULL);

            if (dataspace_id < 0) {

                pass         = false;
                failure_mssg = "H5Screate_simple() failed.";
            }

            /* set the dataset creation plist to specify that the raw data is
             * to be partitioned into 10X10 element chunks.
             */

            if (pass) {

                chunk_size[0] = CHUNK_SIZE;
                chunk_size[1] = CHUNK_SIZE;
                properties    = H5Pcreate(H5P_DATASET_CREATE);

                if (properties < 0) {

                    pass         = false;
                    failure_mssg = "H5Pcreate() failed.";
                }
            }

            if (pass) {

                if (H5Pset_chunk(properties, 2, chunk_size) < 0) {

                    pass         = false;
                    failure_mssg = "H5Pset_chunk() failed.";
                }
            }

            /* create the dataset */
            if (pass) {

                snprintf(dset_name, sizeof(dset_name), "/dset%03d", i);
                dataset_ids[i] = H5Dcreate2(file_id, dset_name, H5T_STD_I32BE, dataspace_id, H5P_DEFAULT,
                                            properties, H5P_DEFAULT);

                if (dataset_ids[i] < 0) {

                    pass         = false;
                    failure_mssg = "H5Dcreate2() failed.";
                }
            }

            /* get the file space ID */
            if (pass) {

                filespace_ids[i] = H5Dget_space(dataset_ids[i]);

                if (filespace_ids[i] < 0) {

                    pass         = false;
                    failure_mssg = "H5Dget_space() failed.";
                }
            }

            i++;
        }
    }

    /* create the mem space to be used to read and write chunks */
    if (pass) {

        dims[0]     = CHUNK_SIZE;
        dims[1]     = CHUNK_SIZE;
        memspace_id = H5Screate_simple(2, dims, NULL);

        if (memspace_id < 0) {

            pass         = false;
            failure_mssg = "H5Screate_simple() failed.";
        }
    }

    /* select in memory hyperslab */
    if (pass) {

        offset[0] = 0; /*offset of hyperslab in memory*/
        offset[1] = 0;
        a_size[0] = CHUNK_SIZE; /*size of hyperslab*/
        a_size[1] = CHUNK_SIZE;
        status    = H5Sselect_hyperslab(memspace_id, H5S_SELECT_SET, offset, NULL, a_size, NULL);

        if (status < 0) {

            pass         = false;
            failure_mssg = "H5Sselect_hyperslab() failed.";
        }
    }

    /* initialize all datasets on a round robin basis */
    i = 0;

    while ((pass) && (i < DSET_SIZE)) {
        j = 0;
        while ((pass) && (j < DSET_SIZE)) {
            m = 0;
            while ((pass) && (m < NUM_DSETS)) {
                /* initialize the slab */
                for (k = 0; k < CHUNK_SIZE; k++) {
                    for (l = 0; l < CHUNK_SIZE; l++) {
                        data_chunk[k][l] = (DSET_SIZE * DSET_SIZE * m) + (DSET_SIZE * (i + k)) + j + l;
                    }
                }

                /* select on disk hyperslab */
                offset[0] = (hsize_t)i; /*offset of hyperslab in file*/
                offset[1] = (hsize_t)j;
                a_size[0] = CHUNK_SIZE; /*size of hyperslab*/
                a_size[1] = CHUNK_SIZE;
                status    = H5Sselect_hyperslab(filespace_ids[m], H5S_SELECT_SET, offset, NULL, a_size, NULL);

                if (status < 0) {

                    pass         = false;
                    failure_mssg = "disk H5Sselect_hyperslab() failed.";
                }

                /* write the chunk to file */
                status = H5Dwrite(dataset_ids[m], H5T_NATIVE_INT, memspace_id, filespace_ids[m], H5P_DEFAULT,
                                  data_chunk);

                if (status < 0) {

                    pass         = false;
                    failure_mssg = "H5Dwrite() failed.";
                }
                m++;
            }
            j += CHUNK_SIZE;
        }

        /* check the cache hit rate, and reset the counters.
         * Hit rate should be just about unity here, so we will just
         * get the data and (possibly) print it without checking it
         * beyond ensuring that it agrees with the cache internal
         * data structures.
         *
         * similarly, check cache size.
         */

        if ((pass) && (i % (DSET_SIZE / 4) == 0)) {

            check_and_validate_cache_hit_rate(file_id, NULL, dump_hit_rate, min_accesses, min_hit_rate);

            check_and_validate_cache_size(file_id, NULL, NULL, NULL, NULL, dump_cache_size);
        }

        i += CHUNK_SIZE;
    }

    /* set alternate config 2 */
    if (pass) {

        if (H5Fset_mdc_config(file_id, &mod_config_2) < 0) {

            pass         = false;
            failure_mssg = "H5Fset_mdc_config() failed 2.\n";
        }
    }

    /* verify that the cache is now set to the alternate config */
    validate_mdc_config(file_id, &mod_config_2, true, 3);

    /* do random reads on all datasets */
    n = 0;
    while ((pass) && (n < NUM_RANDOM_ACCESSES)) {
        m = HDrand() % NUM_DSETS;
        i = (HDrand() % (DSET_SIZE / CHUNK_SIZE)) * CHUNK_SIZE;
        j = (HDrand() % (DSET_SIZE / CHUNK_SIZE)) * CHUNK_SIZE;

        /* select on disk hyperslab */
        offset[0] = (hsize_t)i; /*offset of hyperslab in file*/
        offset[1] = (hsize_t)j;
        a_size[0] = CHUNK_SIZE; /*size of hyperslab*/
        a_size[1] = CHUNK_SIZE;
        status    = H5Sselect_hyperslab(filespace_ids[m], H5S_SELECT_SET, offset, NULL, a_size, NULL);

        if (status < 0) {

            pass         = false;
            failure_mssg = "disk hyperslab create failed.";
        }

        /* read the chunk from file */
        if (pass) {

            status = H5Dread(dataset_ids[m], H5T_NATIVE_INT, memspace_id, filespace_ids[m], H5P_DEFAULT,
                             data_chunk);

            if (status < 0) {

                pass         = false;
                failure_mssg = "disk hyperslab create failed.";
            }
        }

        /* validate the slab */
        if (pass) {

            valid_chunk = true;
            for (k = 0; k < CHUNK_SIZE; k++) {
                for (l = 0; l < CHUNK_SIZE; l++) {
                    if (data_chunk[k][l] != ((DSET_SIZE * DSET_SIZE * m) + (DSET_SIZE * (i + k)) + j + l)) {

                        valid_chunk = false;
                    }
                }
            }

            if (!valid_chunk) {
                pass         = false;
                failure_mssg = "slab validation failed.";
            }
        }

        if ((pass) && (n % (NUM_RANDOM_ACCESSES / 4) == 0)) {

            check_and_validate_cache_hit_rate(file_id, NULL, dump_hit_rate, min_accesses, min_hit_rate);

            check_and_validate_cache_size(file_id, NULL, NULL, NULL, NULL, dump_cache_size);
        }

        n++;
    }

    /* close the file spaces we are done with */
    i = 1;
    while ((pass) && (i < NUM_DSETS)) {
        if (H5Sclose(filespace_ids[i]) < 0) {

            pass         = false;
            failure_mssg = "H5Sclose() failed.";
        }
        i++;
    }

    /* close the datasets we are done with */
    i = 1;
    while ((pass) && (i < NUM_DSETS)) {
        if (H5Dclose(dataset_ids[i]) < 0) {

            pass         = false;
            failure_mssg = "H5Dclose() failed.";
        }
        i++;
    }

    /* set alternate config 3 */
    if (pass) {

        if (H5Fset_mdc_config(file_id, &mod_config_3) < 0) {

            pass         = false;
            failure_mssg = "H5Fset_mdc_config() failed 3.\n";
        }
    }

    /* verify that the cache is now set to the alternate config */
    validate_mdc_config(file_id, &mod_config_3, true, 4);

    /* do random reads on data set 0 only */
    m = 0;
    n = 0;
    while ((pass) && (n < NUM_RANDOM_ACCESSES)) {
        i = (HDrand() % (DSET_SIZE / CHUNK_SIZE)) * CHUNK_SIZE;
        j = (HDrand() % (DSET_SIZE / CHUNK_SIZE)) * CHUNK_SIZE;

        /* select on disk hyperslab */
        offset[0] = (hsize_t)i; /*offset of hyperslab in file*/
        offset[1] = (hsize_t)j;
        a_size[0] = CHUNK_SIZE; /*size of hyperslab*/
        a_size[1] = CHUNK_SIZE;
        status    = H5Sselect_hyperslab(filespace_ids[m], H5S_SELECT_SET, offset, NULL, a_size, NULL);

        if (status < 0) {

            pass         = false;
            failure_mssg = "disk hyperslab create failed.";
        }

        /* read the chunk from file */
        if (pass) {

            status = H5Dread(dataset_ids[m], H5T_NATIVE_INT, memspace_id, filespace_ids[m], H5P_DEFAULT,
                             data_chunk);

            if (status < 0) {

                pass         = false;
                failure_mssg = "disk hyperslab create failed.";
            }
        }

        /* validate the slab */
        if (pass) {

            valid_chunk = true;
            for (k = 0; k < CHUNK_SIZE; k++) {
                for (l = 0; l < CHUNK_SIZE; l++) {
                    if (data_chunk[k][l] != ((DSET_SIZE * DSET_SIZE * m) + (DSET_SIZE * (i + k)) + j + l)) {

                        valid_chunk = false;
                    }
                }
            }

            if (!valid_chunk) {

                pass         = false;
                failure_mssg = "slab validation failed.";
            }
        }

        if ((pass) && (n % (NUM_RANDOM_ACCESSES / 4) == 0)) {

            check_and_validate_cache_hit_rate(file_id, NULL, dump_hit_rate, min_accesses, min_hit_rate);

            check_and_validate_cache_size(file_id, NULL, NULL, NULL, NULL, dump_cache_size);
        }

        n++;
    }

    /* close file space 0 */
    if (pass) {

        if (H5Sclose(filespace_ids[0]) < 0) {

            pass         = false;
            failure_mssg = "H5Sclose(filespace_ids[0]) failed.";
        }
    }

    /* close the data space */
    if (pass) {

        if (H5Sclose(dataspace_id) < 0) {

            pass         = false;
            failure_mssg = "H5Sclose(dataspace) failed.";
        }
    }

    /* close the mem space */
    if (pass) {

        if (H5Sclose(memspace_id) < 0) {

            pass         = false;
            failure_mssg = "H5Sclose(memspace_id) failed.";
        }
    }

    /* close dataset 0 */
    if (pass) {

        if (H5Dclose(dataset_ids[0]) < 0) {

            pass         = false;
            failure_mssg = "H5Dclose(dataset_ids[0]) failed.";
        }
    }

    /* close the file and delete it */
    if (pass) {

        if (H5Fclose(file_id) < 0) {

            pass         = false;
            failure_mssg = "H5Fclose() failed.\n";
        }
        else if (H5Fdelete(filename, H5P_DEFAULT) < 0) {

            pass         = false;
            failure_mssg = "H5Fdelete() failed.\n";
        }
    }

    if (pass) {

        PASSED();
    }
    else {

        H5_FAILED();
    }

    if (!pass) {

        fprintf(stdout, "%s: failure_mssg = \"%s\".\n", __func__, failure_mssg);
    }

    return pass;

} /* mdc_api_call_smoke_check() */

/*-------------------------------------------------------------------------
 * Function:    init_invalid_configs()
 *
 * Purpose:     Initialize an array of invalid external MDC configurations
 *              that will be used to test error rejection in the MDC-
 *              related API calls.
 *
 *              Note: It is assumed that boolean parameters are only set
 *                    to true/false.
 *
 * Return:      Success:    Pointer to an array of cache configurations.
 *              Failure:    NULL
 *
 *-------------------------------------------------------------------------
 */

#define NUM_INVALID_CONFIGS 36
static H5AC_cache_config_t *invalid_configs = NULL;

static H5AC_cache_config_t *
init_invalid_configs(void)
{

    int                  i;
    H5AC_cache_config_t *configs = NULL;

    /* Allocate memory */
    if (NULL == (configs = (H5AC_cache_config_t *)calloc(NUM_INVALID_CONFIGS, sizeof(H5AC_cache_config_t)))) {

        return NULL;
    }

    /* Set defaults for all configs */
    for (i = 0; i < NUM_INVALID_CONFIGS; i++) {

        configs[i].version          = H5C__CURR_AUTO_SIZE_CTL_VER;
        configs[i].rpt_fcn_enabled  = false;
        configs[i].open_trace_file  = false;
        configs[i].close_trace_file = false;
        /* trace file name set to all ASCII NUL by calloc() */
        configs[i].evictions_enabled       = true;
        configs[i].set_initial_size        = true;
        configs[i].initial_size            = (1 * 1024 * 1024);
        configs[i].min_clean_fraction      = 0.25;
        configs[i].max_size                = (16 * 1024 * 1024);
        configs[i].min_size                = (1 * 1024 * 1024);
        configs[i].epoch_length            = 50000;
        configs[i].incr_mode               = H5C_incr__threshold;
        configs[i].lower_hr_threshold      = 0.9;
        configs[i].increment               = 2.0;
        configs[i].apply_max_increment     = true;
        configs[i].max_increment           = (4 * 1024 * 1024);
        configs[i].flash_incr_mode         = H5C_flash_incr__off;
        configs[i].flash_multiple          = 2.0;
        configs[i].flash_threshold         = 0.5;
        configs[i].decr_mode               = H5C_decr__age_out_with_threshold;
        configs[i].upper_hr_threshold      = 0.999;
        configs[i].decrement               = 0.9;
        configs[i].apply_max_decrement     = true;
        configs[i].max_decrement           = (1 * 1024 * 1024);
        configs[i].epochs_before_eviction  = 3;
        configs[i].apply_empty_reserve     = true;
        configs[i].empty_reserve           = 0.1;
        configs[i].dirty_bytes_threshold   = (256 * 1024);
        configs[i].metadata_write_strategy = H5AC__DEFAULT_METADATA_WRITE_STRATEGY;
    }

    /* Set badness for each config */

    /* 0 -- bad version */
    configs[0].version = -1;

    /* 1 -- open_trace_file == true and empty trace_file_name */
    configs[1].open_trace_file = true;
    /* trace file name set to all ASCII NUL by calloc() */

    /* 2 -- max_size too big */
    configs[2].max_size = H5C__MAX_MAX_CACHE_SIZE + 1;

    /* 3 -- min_size too small */
    configs[3].min_size = H5C__MIN_MAX_CACHE_SIZE - 1;

    /* 4 -- min_size > max_size */
    configs[4].max_size = (16 * 1024 * 1024);
    configs[4].min_size = (16 * 1024 * 1024 + 1);

    /* 5 -- initial size out of range (too big) */
    configs[5].initial_size = (16 * 1024 * 1024 + 1);

    /* 6 -- initial_size out of range (too small) */
    configs[6].initial_size = (1 * 1024 * 1024 - 1);

    /* 7 -- min_clean_fraction too big */
    configs[7].min_clean_fraction = 1.000001;

    /* 8 -- min_clean_fraction too small */
    configs[8].min_clean_fraction = -0.00000001;

    /* 9 -- epoch_length too small */
    configs[9].epoch_length = H5C__MIN_AR_EPOCH_LENGTH - 1;

    /* 10 -- epoch_length too big */
    configs[10].epoch_length = H5C__MAX_AR_EPOCH_LENGTH + 1;

    /* 11 -- invalid incr_mode */
    configs[11].incr_mode = (enum H5C_cache_incr_mode) - 1;

    /* 12 -- lower_hr_threshold too small */
    configs[12].lower_hr_threshold = -0.000001;

    /* 13 -- lower_hr_threshold too big */
    configs[13].lower_hr_threshold = 1.00000001;

    /* 14 -- increment too small */
    configs[14].increment = 0.999999999999;

    /* 15 -- invalid flash_incr_mode */
    configs[15].flash_incr_mode = (enum H5C_cache_flash_incr_mode) - 1;

    /* 16 -- flash_multiple too small */
    configs[16].flash_incr_mode = H5C_flash_incr__add_space;
    configs[16].flash_multiple  = 0.09;

    /* 17 -- flash_multiple too big */
    configs[17].flash_incr_mode = H5C_flash_incr__add_space;
    configs[17].flash_multiple  = 10.001;

    /* 18 -- flash_threshold too small */
    configs[18].flash_incr_mode = H5C_flash_incr__add_space;
    configs[18].flash_threshold = 0.099;

    /* 19 -- flash_threshold too big */
    configs[19].flash_incr_mode = H5C_flash_incr__add_space;
    configs[19].flash_threshold = 1.001;

    /* 20 -- bad decr_mode */
    configs[20].decr_mode = (enum H5C_cache_decr_mode) - 1;

    /* 21 -- upper_hr_threshold too big */
    configs[21].upper_hr_threshold = 1.00001;

    /* 22 -- decrement too small */
    configs[22].decr_mode = H5C_decr__threshold;
    configs[22].decrement = -0.0000000001;

    /* 23 -- decrement too big */
    configs[23].decr_mode = H5C_decr__threshold;
    configs[23].decrement = 1.0000000001;

    /* 24 -- epochs_before_eviction too small */
    configs[24].epochs_before_eviction = 0;

    /* 25 -- epochs_before_eviction too big */
    configs[25].epochs_before_eviction = H5C__MAX_EPOCH_MARKERS + 1;

    /* 26 -- empty_reserve too small */
    configs[26].empty_reserve = -0.0000000001;

    /* 27 -- empty_reserve too big */
    configs[27].empty_reserve = 1.00000000001;

    /* 28 -- upper_hr_threshold too small */
    configs[28].upper_hr_threshold = -0.000000001;

    /* 29 -- upper_hr_threshold too big */
    configs[29].upper_hr_threshold = 1.00000001;

    /* 30 -- upper_hr_threshold <= lower_hr_threshold */
    configs[30].lower_hr_threshold = 0.9;
    configs[30].upper_hr_threshold = 0.9;

    /* 31 -- dirty_bytes_threshold too small */
    configs[31].dirty_bytes_threshold = (H5C__MIN_MAX_CACHE_SIZE / 2) - 1;

    /* 32 -- dirty_bytes_threshold too big */
    configs[32].dirty_bytes_threshold = (H5C__MAX_MAX_CACHE_SIZE / 4) + 1;

    /* 33 -- attempt to disable evictions when auto incr enabled */
    configs[33].evictions_enabled = false;
    configs[33].decr_mode         = H5C_decr__off;

    /* 34 -- attempt to disable evictions when auto decr enabled */
    configs[34].evictions_enabled = false;
    configs[34].decr_mode         = H5C_decr__age_out;

    /* 35 -- unknown metadata write strategy */
    configs[35].metadata_write_strategy = -1;

    return configs;

} /* initialize_invalid_configs() */

/*-------------------------------------------------------------------------
 * Function:    check_fapl_mdc_api_errs()
 *
 * Purpose:     Verify that the FAPL related MDC API calls reject input
 *              errors gracefully.
 *
 * Return:      Test pass status (true/false)
 *
 *-------------------------------------------------------------------------
 */
static bool
check_fapl_mdc_api_errs(void)
{
    static char         msg[128];
    int                 i;
    herr_t              result;
    hid_t               fapl_id        = H5I_INVALID_HID;
    H5AC_cache_config_t default_config = H5AC__DEFAULT_CACHE_CONFIG;
    H5AC_cache_config_t scratch;

    TESTING("MDC/FAPL related API input errors");

    pass = true;

    /* first test H5Pget_mdc_config().
     */

    scratch.version = H5C__CURR_AUTO_SIZE_CTL_VER;
    if (pass) {

        H5E_BEGIN_TRY
        {
            result = H5Pget_mdc_config((hid_t)H5I_INVALID_HID, &scratch);
        }
        H5E_END_TRY

        if (result >= 0) {

            pass         = false;
            failure_mssg = "H5Pget_mdc_config() accepted invalid plist_id.";
        }
    }

    /* Create a FAPL for test purposes, and verify that it contains the
     * default MDC configuration.
     */

    if (pass) {

        fapl_id = H5Pcreate(H5P_FILE_ACCESS);

        if (fapl_id < 0) {

            pass         = false;
            failure_mssg = "H5Pcreate(H5P_FILE_ACCESS) failed.\n";
        }
    }

    scratch.version = H5C__CURR_AUTO_SIZE_CTL_VER;
    if ((pass) && ((H5Pget_mdc_config(fapl_id, &scratch) < 0) ||
                   (!CACHE_CONFIGS_EQUAL(default_config, scratch, true, true)))) {

        pass         = false;
        failure_mssg = "New FAPL has unexpected metadata cache config?!?!?.\n";
    }

    if (pass) {

        H5E_BEGIN_TRY
        {
            result = H5Pget_mdc_config(fapl_id, NULL);
        }
        H5E_END_TRY

        if (result >= 0) {

            pass         = false;
            failure_mssg = "H5Pget_mdc_config() accepted NULL config_ptr.";
        }
    }

    /* one last test for H5Pget_mdc_config() */

    scratch.version = -1; /* a convenient, invalid value */
    if (pass) {

        H5E_BEGIN_TRY
        {
            result = H5Pget_mdc_config(fapl_id, &scratch);
        }
        H5E_END_TRY

        if (result >= 0) {

            pass         = false;
            failure_mssg = "H5Pget_mdc_config() accepted bad config version.";
        }
    }

    /* now test H5Pset_mdc_config()
     */

    scratch.version = H5C__CURR_AUTO_SIZE_CTL_VER;
    if (pass) {

        H5E_BEGIN_TRY
        {
            result = H5Pset_mdc_config((hid_t)H5I_INVALID_HID, &default_config);
        }
        H5E_END_TRY

        if (result >= 0) {

            pass         = false;
            failure_mssg = "H5Pset_mdc_config() accepted bad invalid plist_id.";
        }
    }

    if (pass) {

        H5E_BEGIN_TRY
        {
            result = H5Pset_mdc_config(fapl_id, NULL);
        }
        H5E_END_TRY

        if (result >= 0) {

            pass         = false;
            failure_mssg = "H5Pset_mdc_config() accepted NULL config_ptr.";
        }
    }

    i = 0;
    while ((pass) && (i < NUM_INVALID_CONFIGS)) {
        H5E_BEGIN_TRY
        {
            result = H5Pset_mdc_config(fapl_id, &(invalid_configs[i]));
        }
        H5E_END_TRY

        if (result >= 0) {

            pass = false;
            snprintf(msg, (size_t)128, "H5Pset_mdc_config() accepted invalid_configs[%d].", i);
            failure_mssg = msg;
        }
        i++;
    }

    /* verify that none of the above calls to H5Pset_mdc_config() changed
     * the configuration in the FAPL.
     */
    scratch.version = H5C__CURR_AUTO_SIZE_CTL_VER;
    if ((pass) && ((H5Pget_mdc_config(fapl_id, &scratch) < 0) ||
                   (!CACHE_CONFIGS_EQUAL(default_config, scratch, true, true)))) {

        pass         = false;
        failure_mssg = "FAPL metadata cache config changed???.\n";
    }

    if (pass) {

        PASSED();
    }
    else {

        H5_FAILED();
    }

    if (!pass) {

        fprintf(stdout, "%s: failure_mssg = \"%s\".\n", __func__, failure_mssg);
    }

    return pass;

} /* check_fapl_mdc_api_errs() */

/*-------------------------------------------------------------------------
 * Function:    check_file_mdc_api_errs()
 *
 * Purpose:     Verify that the file related MDC API calls reject input
 *              errors gracefully.
 *
 * Return:      Test pass status (true/false)
 *
 *-------------------------------------------------------------------------
 */
static bool
check_file_mdc_api_errs(unsigned paged, hid_t fcpl_id)
{
    char                filename[512];
    static char         msg[128];
    bool                show_progress = false;
    int                 i;
    herr_t              result;
    hid_t               file_id = H5I_INVALID_HID;
    size_t              max_size;
    size_t              min_clean_size;
    size_t              cur_size;
    int                 cur_num_entries;
    double              hit_rate;
    H5AC_cache_config_t default_config = H5AC__DEFAULT_CACHE_CONFIG;
    H5AC_cache_config_t scratch;

    if (paged)
        TESTING("MDC/FILE related API input errors for paged aggregation strategy");
    else
        TESTING("MDC/FILE related API input errors");

    pass = true;

    /* Create a file for test purposes, and verify that its metadata cache
     * set to the default MDC configuration.
     */

    /* setup the file name */
    if (pass) {

        if (show_progress) {

            fprintf(stdout, "%s: calling h5_fixname().\n", __func__);
        }

        if (h5_fixname(FILENAME[0], H5P_DEFAULT, filename, sizeof(filename)) == NULL) {

            pass         = false;
            failure_mssg = "h5_fixname() failed.\n";
        }
    }

    if (pass) {

        if (show_progress) {

            fprintf(stdout, "%s: calling H5Fcreate().\n", __func__);
        }

        file_id = H5Fcreate(filename, H5F_ACC_TRUNC, fcpl_id, H5P_DEFAULT);

        if (file_id < 0) {

            pass         = false;
            failure_mssg = "H5Fcreate() failed.\n";
        }
    }

    validate_mdc_config(file_id, &default_config, true, 1);

    /* test H5Fget_mdc_config().  */

    /* Create an ID to use in the H5Fset_mdc_config/H5Fget_mdc_config tests */
    hid_t dtype_id = H5Tcopy(H5T_NATIVE_INT);

    if (dtype_id < 0) {

        pass         = false;
        failure_mssg = "H5Tcopy() failed.\n";
    }

    scratch.version = H5C__CURR_AUTO_SIZE_CTL_VER;
    if (pass) {

        if (show_progress) {

            fprintf(stdout, "%s: testing H5Fget_mdc_config() 1.\n", __func__);
        }

        H5E_BEGIN_TRY
        {
            result = H5Fget_mdc_config((hid_t)H5I_INVALID_HID, &scratch);
        }
        H5E_END_TRY

        if (result >= 0) {

            pass         = false;
            failure_mssg = "H5Fget_mdc_config() accepted invalid file_id.";
        }

        H5E_BEGIN_TRY
        {
            result = H5Fget_mdc_config(dtype_id, &scratch); /* not a file ID */
        }
        H5E_END_TRY

        if (result >= 0) {

            pass         = false;
            failure_mssg = "H5Fget_mdc_config() accepted an ID that is not a file ID.";
        }
    }

    if (pass) {

        if (show_progress) {

            fprintf(stdout, "%s: testing H5Fget_mdc_config() 2.\n", __func__);
        }

        H5E_BEGIN_TRY
        {
            result = H5Fget_mdc_config(file_id, NULL);
        }
        H5E_END_TRY

        if (result >= 0) {

            pass         = false;
            failure_mssg = "H5Fget_mdc_config() accepted NULL config_ptr.";
        }
    }

    scratch.version = -1; /* a convenient, invalid value */
    if (pass) {

        if (show_progress) {

            fprintf(stdout, "%s: testing H5Fget_mdc_config() 3.\n", __func__);
        }

        H5E_BEGIN_TRY
        {
            result = H5Fget_mdc_config(file_id, &scratch);
        }
        H5E_END_TRY

        if (result >= 0) {

            pass         = false;
            failure_mssg = "H5Fget_mdc_config() accepted bad config version.";
        }
    }

    /* test H5Fset_mdc_config() */

    scratch.version = H5C__CURR_AUTO_SIZE_CTL_VER;
    if (pass) {

        if (show_progress) {

            fprintf(stdout, "%s: testing H5Fset_mdc_config() 1.\n", __func__);
        }

        H5E_BEGIN_TRY
        {
            result = H5Fset_mdc_config((hid_t)H5I_INVALID_HID, &default_config);
        }
        H5E_END_TRY

        if (result >= 0) {

            pass         = false;
            failure_mssg = "H5Fset_mdc_config() accepted bad invalid file_id.";
        }

        H5E_BEGIN_TRY
        {
            result = H5Fset_mdc_config(dtype_id, &default_config);
        }
        H5E_END_TRY

        if (result >= 0) {

            pass         = false;
            failure_mssg = "H5Fset_mdc_config() accepted an ID that is not a file ID.";
        }
    }

    /* Close the temporary datatype */
    result = H5Tclose(dtype_id);

    if (result < 0) {

        pass         = false;
        failure_mssg = "H5Tclose() failed.\n";
    }

    if (pass) {

        if (show_progress) {

            fprintf(stdout, "%s: testing H5Fset_mdc_config() 2.\n", __func__);
        }

        H5E_BEGIN_TRY
        {
            result = H5Fset_mdc_config(file_id, NULL);
        }
        H5E_END_TRY

        if (result >= 0) {

            pass         = false;
            failure_mssg = "H5Fset_mdc_config() accepted NULL config_ptr.";
        }
    }

    i = 0;
    while ((pass) && (i < NUM_INVALID_CONFIGS)) {
        if (show_progress) {

            fprintf(stdout, "%s: testing H5Fset_mdc_config() with invalid config %d.\n", __func__, i);
        }

        H5E_BEGIN_TRY
        {
            result = H5Fset_mdc_config(file_id, &(invalid_configs[i]));
        }
        H5E_END_TRY

        if (result >= 0) {

            pass = false;
            snprintf(msg, (size_t)128, "H5Fset_mdc_config() accepted invalid_configs[%d].", i);
            failure_mssg = msg;
        }
        i++;
    }

    /* verify that none of the above calls to H5Fset_mdc_config() changed
     * the configuration in the FAPL.
     */
    validate_mdc_config(file_id, &default_config, true, 2);

    /* test H5Fget_mdc_hit_rate() */
    if (pass) {

        if (show_progress) {

            fprintf(stdout, "%s: testing H5Fget_mdc_hit_rate() 1.\n", __func__);
        }

        H5E_BEGIN_TRY
        {
            result = H5Fget_mdc_hit_rate((hid_t)H5I_INVALID_HID, &hit_rate);
        }
        H5E_END_TRY

        if (result >= 0) {

            pass         = false;
            failure_mssg = "H5Fget_mdc_hit_rate() accepted bad file_id.";
        }
    }

    if (pass) {

        if (show_progress) {

            fprintf(stdout, "%s: testing H5Fget_mdc_hit_rate() 2.\n", __func__);
        }

        H5E_BEGIN_TRY
        {
            result = H5Fget_mdc_hit_rate(file_id, NULL);
        }
        H5E_END_TRY

        if (result >= 0) {

            pass         = false;
            failure_mssg = "H5Fget_mdc_hit_rate() accepted NULL hit_rate_ptr.";
        }
    }

    /* test H5Freset_mdc_hit_rate_stats() */
    if (pass) {

        if (show_progress) {

            fprintf(stdout, "%s: testing H5Freset_mdc_hit_rate_stats().\n", __func__);
        }

        H5E_BEGIN_TRY
        {
            result = H5Freset_mdc_hit_rate_stats((hid_t)H5I_INVALID_HID);
        }
        H5E_END_TRY

        if (result >= 0) {

            pass         = false;
            failure_mssg = "H5Freset_mdc_hit_rate_stats() accepted bad file_id.";
        }

        /* Create an ID to use in the next test */
        hid_t scalarsp_id = H5Screate(H5S_SCALAR);

        if (scalarsp_id < 0) {

            pass         = false;
            failure_mssg = "H5Screate() failed.\n";
        }

        /* Try to call H5Freset_mdc_hit_rate_stats with an inappropriate ID */
        H5E_BEGIN_TRY
        {
            result = H5Freset_mdc_hit_rate_stats(scalarsp_id);
        }
        H5E_END_TRY

        if (result >= 0) {

            pass         = false;
            failure_mssg = "H5Freset_mdc_hit_rate_stats() accepted an ID that is not a file_id.";
        }

        /* Close the temporary dataspace */
        result = H5Sclose(scalarsp_id);

        if (result < 0) {

            pass         = false;
            failure_mssg = "H5Sclose() failed.\n";
        }
    }

    /* test H5Fget_mdc_size() */
    if (pass) {

        if (show_progress) {

            fprintf(stdout, "%s: testing H5Fget_mdc_size() 1.\n", __func__);
        }

        H5E_BEGIN_TRY
        {
            result = H5Fget_mdc_size((hid_t)H5I_INVALID_HID, &max_size, &min_clean_size, &cur_size,
                                     &cur_num_entries);
        }
        H5E_END_TRY

        if (result >= 0) {

            pass         = false;
            failure_mssg = "H5Fget_mdc_size() accepted bad file_id.";
        }
    }

    if (pass) {

        if (show_progress) {

            fprintf(stdout, "%s: testing H5Fget_mdc_size() 2.\n", __func__);
        }

        if ((H5Fget_mdc_size(file_id, &max_size, NULL, NULL, NULL) < 0) ||
            (H5Fget_mdc_size(file_id, NULL, &min_clean_size, NULL, NULL) < 0) ||
            (H5Fget_mdc_size(file_id, NULL, NULL, &cur_size, NULL) < 0) ||
            (H5Fget_mdc_size(file_id, NULL, NULL, NULL, &cur_num_entries) < 0) ||
            (H5Fget_mdc_size(file_id, NULL, NULL, NULL, NULL) < 0)) {

            pass         = false;
            failure_mssg = "H5Fget_mdc_size() failed to handle NULL params.";
        }
    }

    /* close the file and delete it */
    if (pass) {

        if (show_progress) {

            fprintf(stdout, "%s: cleaning up from tests.\n", __func__);
        }

        if (H5Fclose(file_id) < 0) {

            pass         = false;
            failure_mssg = "H5Fclose() failed.\n";
        }
        else if (H5Fdelete(filename, H5P_DEFAULT) < 0) {

            pass         = false;
            failure_mssg = "H5Fdelete() failed.\n";
        }
    }

    if (pass) {

        PASSED();
    }
    else {

        H5_FAILED();
    }

    if (!pass) {

        fprintf(stdout, "%s: failure_mssg = \"%s\".\n", __func__, failure_mssg);
    }

    return pass;

} /* check_file_mdc_api_errs() */

/*-------------------------------------------------------------------------
 * Function:    main
 *
 * Purpose:     Run tests on the cache code contained in H5C.c
 *
 * Return:      EXIT_SUCCESS/EXIT_FAILURE
 *
 *-------------------------------------------------------------------------
 */
int
main(void)
{
    unsigned nerrs = 0;
    int      express_test;
    hid_t    fcpl_id  = H5I_INVALID_HID;
    hid_t    fcpl2_id = H5I_INVALID_HID;
    unsigned paged;

    H5open();

    express_test = GetTestExpress();

    printf("===================================\n");
    printf("Cache API tests\n");
    printf("        express_test = %d\n", express_test);
    printf("===================================\n");

    /* Initialize invalid configurations.
     */
    invalid_configs = init_invalid_configs();
    if (NULL == invalid_configs) {
        failure_mssg = "Unable to allocate memory for invalid configs.";
        fprintf(stdout, "%s: failure_mssg = \"%s\".\n", __func__, failure_mssg);
        return EXIT_FAILURE;
    } /* end if */

    if ((fcpl_id = H5Pcreate(H5P_FILE_CREATE)) < 0) {
        failure_mssg = "H5Pcreate(H5P_FILE_CREATE) failed.\n";
        fprintf(stdout, "%s: failure_mssg = \"%s\".\n", __func__, failure_mssg);
        return EXIT_FAILURE;
    } /* end if */

    /* Set file space strategy to default or paged aggregation strategy */
    if ((fcpl2_id = H5Pcopy(fcpl_id)) < 0) {
        failure_mssg = "H5Pcreate(H5P_FILE_CREATE) failed.\n";
        fprintf(stdout, "%s: failure_mssg = \"%s\".\n", __func__, failure_mssg);
        return EXIT_FAILURE;
    } /* end if */

    if (H5Pset_file_space_strategy(fcpl2_id, H5F_FSPACE_STRATEGY_PAGE, 1, (hsize_t)1) < 0) {
        failure_mssg = "H5Pset_file_space_strategy() failed.\n";
        fprintf(stdout, "%s: failure_mssg = \"%s\".\n", __func__, failure_mssg);
        return EXIT_FAILURE;
    } /* end if */

    /* Test with paged aggregation enabled or not */
    /* The "my_fcpl" passed to each test has the paged or non-paged strategy set up accordingly */
    for (paged = false; paged <= true; paged++) {
        hid_t my_fcpl = fcpl_id;

        if (paged) {
            /* Only run paged aggregation tests with sec2/default driver */
            if (!h5_using_default_driver(NULL))
                continue;
            else
                my_fcpl = fcpl2_id;
        }

        if (!check_fapl_mdc_api_calls(paged, my_fcpl))
            nerrs += 1;

        if (!check_file_mdc_api_calls(paged, my_fcpl))
            nerrs += 1;

        if (!mdc_api_call_smoke_check(express_test, paged, my_fcpl))
            nerrs += 1;

        if (!check_file_mdc_api_errs(paged, my_fcpl))
            nerrs += 1;
    } /* end for paged */

    if (!check_fapl_mdc_api_errs())
        nerrs += 1;

    if (invalid_configs)
        free(invalid_configs);

    if (H5Pclose(fcpl_id) < 0) {
        failure_mssg = "H5Pclose() failed.\n";
        fprintf(stdout, "%s: failure_mssg = \"%s\".\n", __func__, failure_mssg);
        return EXIT_FAILURE;
    } /* end if */

    if (nerrs > 0)
        return EXIT_FAILURE;
    else
        return EXIT_SUCCESS;
} /* main() */
