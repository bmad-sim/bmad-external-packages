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

/***********************************************************
 *
 * Test program:   tmisc
 *
 * Test miscellaneous features not tested elsewhere.  Generally
 *       regression tests for bugs that are reported and don't
 *       have an existing test to add them to.
 *
 *************************************************************/

#define H5D_FRIEND /*suppress error about including H5Dpkg      */
#define H5T_FRIEND /*suppress error about including H5Tpkg      */

/* Define this macro to indicate that the testing APIs should be available */
#define H5D_TESTING

#include "testhdf5.h"
#include "H5srcdir.h"
#include "H5Dpkg.h"      /* Datasets                 */
#include "H5Tpkg.h"      /* Datatypes                */
#include "H5MMprivate.h" /* Memory                   */

/* Definitions for misc. test #1 */
#define MISC1_FILE      "tmisc1.h5"
#define MISC1_VAL       (13417386) /* 0xccbbaa */
#define MISC1_VAL2      (15654348) /* 0xeeddcc */
#define MISC1_DSET_NAME "/scalar_set"

/* Definitions for misc. test #2 */
#define MISC2_FILE_1     "tmisc2a.h5"
#define MISC2_FILE_2     "tmisc2b.h5"
#define MISC2_ATT_NAME_1 "scalar_att_1"
#define MISC2_ATT_NAME_2 "scalar_att_2"

typedef struct {
    char *string;
} misc2_struct;

/* Definitions for misc. test #3 */
#define MISC3_FILE       "tmisc3.h5"
#define MISC3_RANK       2
#define MISC3_DIM1       6
#define MISC3_DIM2       6
#define MISC3_CHUNK_DIM1 2
#define MISC3_CHUNK_DIM2 2
#define MISC3_FILL_VALUE 2
#define MISC3_DSET_NAME  "/chunked"

/* Definitions for misc. test #4 */
#define MISC4_FILE_1  "tmisc4a.h5"
#define MISC4_FILE_2  "tmisc4b.h5"
#define MISC4_GROUP_1 "/Group1"
#define MISC4_GROUP_2 "/Group2"

/* Definitions for misc. test #5 */
#define MISC5_FILE       "tmisc5.h5"
#define MISC5_DSETNAME   "dset1"
#define MISC5_DSETRANK   1
#define MISC5_NELMTOPLVL 1
#define MISC5_DBGNELM1   2
#define MISC5_DBGNELM2   1
#define MISC5_DBGNELM3   1
#define MISC5_DBGELVAL1  999999999
#define MISC5_DBGELVAL2  888888888
#define MISC5_DBGELVAL3  777777777

typedef struct {
    int   st1_el1;
    hvl_t st1_el2;
} misc5_struct1;

typedef struct {
    int   st2_el1;
    hvl_t st2_el2;
} misc5_struct2;

typedef struct {
    int st3_el1;
} misc5_struct3;

typedef struct {
    hid_t st3h_base;
    hid_t st3h_id;
} misc5_struct3_hndl;

typedef struct {
    hid_t               st2h_base;
    hid_t               st2h_id;
    misc5_struct3_hndl *st2h_st3hndl;
} misc5_struct2_hndl;

typedef struct {
    hid_t               st1h_base;
    hid_t               st1h_id;
    misc5_struct2_hndl *st1h_st2hndl;
} misc5_struct1_hndl;

/* Definitions for misc. test #6 */
#define MISC6_FILE      "tmisc6.h5"
#define MISC6_DSETNAME1 "dset1"
#define MISC6_DSETNAME2 "dset2"
#define MISC6_NUMATTR   16

/* Definitions for misc. test #7 */
#define MISC7_FILE      "tmisc7.h5"
#define MISC7_DSETNAME1 "Dataset1"
#define MISC7_DSETNAME2 "Dataset2"
#define MISC7_TYPENAME1 "Datatype1"
#define MISC7_TYPENAME2 "Datatype2"

/* Definitions for misc. test #8 */
#define MISC8_FILE      "tmisc8.h5"
#define MISC8_DSETNAME1 "Dataset1"
#define MISC8_DSETNAME4 "Dataset4"
#define MISC8_DSETNAME5 "Dataset5"
#define MISC8_DSETNAME8 "Dataset8"

#ifndef H5_HAVE_PARALLEL
#define MISC8_DSETNAME2  "Dataset2"
#define MISC8_DSETNAME3  "Dataset3"
#define MISC8_DSETNAME6  "Dataset6"
#define MISC8_DSETNAME7  "Dataset7"
#define MISC8_DSETNAME9  "Dataset9"
#define MISC8_DSETNAME10 "Dataset10"
#endif

#define MISC8_RANK       2
#define MISC8_DIM0       50
#define MISC8_DIM1       50
#define MISC8_CHUNK_DIM0 10
#define MISC8_CHUNK_DIM1 10

/* Definitions for misc. test #9 */
#define MISC9_FILE "tmisc9.h5"

/* Definitions for misc. test #10 */
#define MISC10_FILE_OLD "tmtimeo.h5"
#define MISC10_FILE_NEW "tmisc10.h5"
#define MISC10_DSETNAME "Dataset1"

/* Definitions for misc. test #11 */
#define MISC11_FILE       "tmisc11.h5"
#define MISC11_USERBLOCK  1024
#define MISC11_SIZEOF_OFF 4
#define MISC11_SIZEOF_LEN 4
#define MISC11_SYM_LK     8
#define MISC11_SYM_IK     32
#define MISC11_ISTORE_IK  64
#define MISC11_NINDEXES   1

/* Definitions for misc. test #12 */
#define MISC12_FILE        "tmisc12.h5"
#define MISC12_DSET_NAME   "Dataset"
#define MISC12_SPACE1_RANK 1
#define MISC12_SPACE1_DIM1 4
#define MISC12_CHUNK_SIZE  2
#define MISC12_APPEND_SIZE 5

/* Definitions for misc. test #13 */
#define MISC13_FILE_1         "tmisc13a.h5"
#define MISC13_FILE_2         "tmisc13b.h5"
#define MISC13_DSET1_NAME     "Dataset1"
#define MISC13_DSET2_NAME     "Dataset2"
#define MISC13_DSET3_NAME     "Dataset3"
#define MISC13_GROUP1_NAME    "Group1"
#define MISC13_GROUP2_NAME    "Group2"
#define MISC13_DTYPE_NAME     "Datatype"
#define MISC13_RANK           1
#define MISC13_DIM1           600
#define MISC13_CHUNK_DIM1     10
#define MISC13_USERBLOCK_SIZE 512
#define MISC13_COPY_BUF_SIZE  4096

/* Definitions for misc. test #14 */
#define MISC14_FILE          "tmisc14.h5"
#define MISC14_DSET1_NAME    "Dataset1"
#define MISC14_DSET2_NAME    "Dataset2"
#define MISC14_DSET3_NAME    "Dataset3"
#define MISC14_METADATA_SIZE 4096

/* Definitions for misc. test #15 */
#define MISC15_FILE     "tmisc15.h5"
#define MISC15_BUF_SIZE 1024

/* Definitions for misc. test #16 */
#define MISC16_FILE       "tmisc16.h5"
#define MISC16_SPACE_DIM  4
#define MISC16_SPACE_RANK 1
#define MISC16_STR_SIZE   8
#define MISC16_DSET_NAME  "Dataset"

/* Definitions for misc. test #17 */
#define MISC17_FILE       "tmisc17.h5"
#define MISC17_SPACE_RANK 2
#define MISC17_SPACE_DIM1 4
#define MISC17_SPACE_DIM2 8
#define MISC17_DSET_NAME  "Dataset"

/* Definitions for misc. test #18 */
#define MISC18_FILE       "tmisc18.h5"
#define MISC18_DSET1_NAME "Dataset1"
#define MISC18_DSET2_NAME "Dataset2"

/* Definitions for misc. test #19 */
#define MISC19_FILE       "tmisc19.h5"
#define MISC19_DSET_NAME  "Dataset"
#define MISC19_ATTR_NAME  "Attribute"
#define MISC19_GROUP_NAME "Group"

/* Definitions for misc. test #20 */
#define MISC20_FILE       "tmisc20.h5"
#define MISC20_FILE_OLD   "tlayouto.h5"
#define MISC20_DSET_NAME  "Dataset"
#define MISC20_DSET2_NAME "Dataset2"
#define MISC20_SPACE_RANK 2
/* Make sure the product of the following 2 does not get too close to */
/* 64 bits, risking an overflow. */
#define MISC20_SPACE_DIM0  (8 * 1024 * 1024 * (uint64_t)1024)
#define MISC20_SPACE_DIM1  ((256 * 1024 * (uint64_t)1024) + 1)
#define MISC20_SPACE2_DIM0 8
#define MISC20_SPACE2_DIM1 4

#ifdef H5_HAVE_FILTER_SZIP
/* Definitions for misc. test #21 */
#define MISC21_FILE       "tmisc21.h5"
#define MISC21_DSET_NAME  "Dataset"
#define MISC21_SPACE_RANK 2
#define MISC21_SPACE_DIM0 7639
#define MISC21_SPACE_DIM1 6308
#define MISC21_CHUNK_DIM0 2048
#define MISC21_CHUNK_DIM1 2048

/* Definitions for misc. test #22 */
#define MISC22_FILE       "tmisc22.h5"
#define MISC22_DSET_NAME  "Dataset"
#define MISC22_SPACE_RANK 2
#define MISC22_CHUNK_DIM0 512
#define MISC22_CHUNK_DIM1 512
#define MISC22_SPACE_DIM0 639
#define MISC22_SPACE_DIM1 1308
#endif /* H5_HAVE_FILTER_SZIP */

/* Definitions for misc. test #23 */
#define MISC23_FILE          "tmisc23.h5"
#define MISC23_NAME_BUF_SIZE 40

/* Definitions for misc. test #24 */
#define MISC24_FILE          "tmisc24.h5"
#define MISC24_GROUP_NAME    "group"
#define MISC24_GROUP_LINK    "group_link"
#define MISC24_DATASET_NAME  "dataset"
#define MISC24_DATASET_LINK  "dataset_link"
#define MISC24_DATATYPE_NAME "datatype"
#define MISC24_DATATYPE_LINK "datatype_link"

/* Definitions for misc. test #25 'a', 'b' & 'c' */
#define MISC25A_FILE        "foo.h5"
#define MISC25A_GROUP0_NAME "grp0"
#define MISC25A_GROUP1_NAME "/grp0/grp1"
#define MISC25A_GROUP2_NAME "/grp0/grp2"
#define MISC25A_GROUP3_NAME "/grp0/grp3"
#define MISC25A_ATTR1_NAME  "_long attribute_"
#define MISC25A_ATTR1_LEN   11
#define MISC25A_ATTR2_NAME  "_short attr__"
#define MISC25A_ATTR2_LEN   11
#define MISC25A_ATTR3_NAME  "_short attr__"
#define MISC25A_ATTR3_LEN   1
#define MISC25B_FILE        "mergemsg.h5"
#define MISC25B_GROUP       "grp1"
#define MISC25C_FILE        "nc4_rename.h5"
#define MISC25C_DSETNAME    "da"
#define MISC25C_DSETNAME2   "dz"
#define MISC25C_DSETGRPNAME "ga"
#define MISC25C_GRPNAME     "gb"
#define MISC25C_GRPNAME2    "gc"
#define MISC25C_ATTRNAME    "aa"
#define MISC25C_ATTRNAME2   "ab"

/* Definitions for misc. test #26 */
#define MISC26_FILE "dcpl_file.h5"

/* Definitions for misc. test #27 */
/* (Note that this test file is generated by the "gen_bad_ohdr.c" code) */
#define MISC27_FILE  "tbad_msg_count.h5"
#define MISC27_GROUP "Group"

/* Definitions for misc. test #28 */
#define MISC28_FILE   "tmisc28.h5"
#define MISC28_SIZE   10
#define MISC28_NSLOTS 10000

/* Definitions for misc. test #29 */
#define MISC29_ORIG_FILE "specmetaread.h5"
#define MISC29_COPY_FILE "tmisc29.h5"
#define MISC29_DSETNAME  "dset2"

/* Definitions for misc. test #30 */
#define MISC30_FILE "tmisc30.h5"

#ifndef H5_NO_DEPRECATED_SYMBOLS
/* Definitions for misc. test #31 */
#define MISC31_FILE      "tmisc31.h5"
#define MISC31_DSETNAME  "dset"
#define MISC31_ATTRNAME1 "attr1"
#define MISC31_ATTRNAME2 "attr2"
#define MISC31_GROUPNAME "group"
#define MISC31_PROPNAME  "misc31_prop"
#define MISC31_DTYPENAME "dtype"
#endif /* H5_NO_DEPRECATED_SYMBOLS */

/* Definitions for misc. test #33 */
/* Note that this test file is generated by "gen_bad_offset.c" */
/* and bad offset values are written to that file for testing */
#define MISC33_FILE "bad_offset.h5"

/* Definitions for misc. test #35 */
#define MISC35_SPACE_RANK 3
#define MISC35_SPACE_DIM1 3
#define MISC35_SPACE_DIM2 15
#define MISC35_SPACE_DIM3 13
#define MISC35_NPOINTS    10

/* Definitions for misc. test #37 */
/* The test file is formerly named h5_nrefs_POC.
   See https://nvd.nist.gov/vuln/detail/CVE-2020-10812 */
#define CVE_2020_10812_FILENAME "cve_2020_10812.h5"

/* Definitions for misc. test #38 */
#define MISC38A_FILE     "tmisc38a.h5"
#define MISC38A_DSETNAME "Fletcher_float_data_be"
#define MISC38B_FILE     "tmisc38b.h5"
#define MISC38B_DSETNAME "unusual_datatype"
#define MISC38C_FILE     "tmisc38c.h5"
#define MISC38C_DSETNAME "dset_unusual_datatype"
#define MISC38C_TYPENAME "type_unusual_datatype"
#define MISC38C_ATTRNAME "attr_unusual_datatype"

#define MISC39_FILE "type_conversion_path_table_issue.h5"
#define MISC40_FILE "set_est_link_info.h5"
#define MISC41_FILE "obj_props_intermediate.h5"

/****************************************************************
**
**  test_misc1(): test unlinking a dataset from a group and immediately
**                      reusing the dataset name
**
****************************************************************/
static void
test_misc1(void)
{
    int    i;
    int    i_check;
    hid_t  file, dataspace, dataset;
    herr_t ret;

    /* Output message about test being performed */
    MESSAGE(5, ("Testing Unlinking Dataset and Re-creating It\n"));

    file = H5Fcreate(MISC1_FILE, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
    CHECK(file, FAIL, "H5Fcreate");

    dataspace = H5Screate(H5S_SCALAR);
    CHECK(dataspace, FAIL, "H5Screate");

    /* Write the dataset the first time. */
    dataset =
        H5Dcreate2(file, MISC1_DSET_NAME, H5T_NATIVE_INT, dataspace, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
    CHECK(dataset, FAIL, "H5Dcreate2");

    i   = MISC1_VAL;
    ret = H5Dwrite(dataset, H5T_NATIVE_INT, H5S_ALL, H5S_ALL, H5P_DEFAULT, &i);
    CHECK(ret, FAIL, "H5Dwrite");

    ret = H5Dclose(dataset);
    CHECK(ret, FAIL, "H5Dclose");

    /* Remove the dataset. */
    ret = H5Ldelete(file, MISC1_DSET_NAME, H5P_DEFAULT);
    CHECK(ret, FAIL, "H5Ldelete");

    /* Write the dataset for the second time with a different value. */
    dataset =
        H5Dcreate2(file, MISC1_DSET_NAME, H5T_NATIVE_INT, dataspace, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
    CHECK(dataset, FAIL, "H5Dcreate2");

    i   = MISC1_VAL2;
    ret = H5Dwrite(dataset, H5T_NATIVE_INT, H5S_ALL, H5S_ALL, H5P_DEFAULT, &i);
    CHECK(ret, FAIL, "H5Dwrite");

    ret = H5Dclose(dataset);
    CHECK(ret, FAIL, "H5Dclose");

    ret = H5Sclose(dataspace);
    CHECK(ret, FAIL, "H5Sclose");

    ret = H5Fclose(file);
    CHECK(ret, FAIL, "H5Fclose");

    /* Now, check the value written to the dataset, after it was re-created */
    file = H5Fopen(MISC1_FILE, H5F_ACC_RDONLY, H5P_DEFAULT);
    CHECK(file, FAIL, "H5Fopen");

    dataspace = H5Screate(H5S_SCALAR);
    CHECK(dataspace, FAIL, "H5Screate");

    dataset = H5Dopen2(file, MISC1_DSET_NAME, H5P_DEFAULT);
    CHECK(dataset, FAIL, "H5Dopen2");

    ret = H5Dread(dataset, H5T_NATIVE_INT, H5S_ALL, H5S_ALL, H5P_DEFAULT, &i_check);
    CHECK(ret, FAIL, "H5Dread");
    VERIFY(i_check, MISC1_VAL2, "H5Dread");

    ret = H5Sclose(dataspace);
    CHECK(ret, FAIL, "H5Sclose");

    ret = H5Dclose(dataset);
    CHECK(ret, FAIL, "H5Dclose");

    ret = H5Fclose(file);
    CHECK(ret, FAIL, "H5Fclose");

} /* end test_misc1() */

static hid_t
misc2_create_type(void)
{
    hid_t  type, type_tmp;
    herr_t ret;

    type_tmp = H5Tcopy(H5T_C_S1);
    CHECK(type_tmp, FAIL, "H5Tcopy");

    ret = H5Tset_size(type_tmp, H5T_VARIABLE);
    CHECK(ret, FAIL, "H5Tset_size");

    type = H5Tcreate(H5T_COMPOUND, sizeof(misc2_struct));
    CHECK(type, FAIL, "H5Tcreate");

    ret = H5Tinsert(type, "string", offsetof(misc2_struct, string), type_tmp);
    CHECK(ret, FAIL, "H5Tinsert");

    ret = H5Tclose(type_tmp);
    CHECK(ret, FAIL, "H5Tclose");

    return type;
}

static void
test_misc2_write_attribute(void)
{
    hid_t        file1, file2, root1, root2, dataspace, att1, att2;
    hid_t        type;
    herr_t       ret;
    misc2_struct data, data_check;
    char        *string_att1 = strdup("string attribute in file one");
    char        *string_att2 = strdup("string attribute in file two");

    memset(&data, 0, sizeof(data));
    memset(&data_check, 0, sizeof(data_check));

    type = misc2_create_type();

    dataspace = H5Screate(H5S_SCALAR);
    CHECK(dataspace, FAIL, "H5Screate");

    file2 = H5Fcreate(MISC2_FILE_2, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
    CHECK(file2, FAIL, "H5Fcreate");

    file1 = H5Fcreate(MISC2_FILE_1, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
    CHECK(file1, FAIL, "H5Fcreate");

    root1 = H5Gopen2(file1, "/", H5P_DEFAULT);
    CHECK(root1, FAIL, "H5Gopen2");

    att1 = H5Acreate2(root1, MISC2_ATT_NAME_1, type, dataspace, H5P_DEFAULT, H5P_DEFAULT);
    CHECK(att1, FAIL, "H5Acreate2");

    data.string = string_att1;

    ret = H5Awrite(att1, type, &data);
    CHECK(ret, FAIL, "H5Awrite");

    ret = H5Aread(att1, type, &data_check);
    CHECK(ret, FAIL, "H5Aread");

    ret = H5Treclaim(type, dataspace, H5P_DEFAULT, &data_check);
    CHECK(ret, FAIL, "H5Treclaim");

    ret = H5Aclose(att1);
    CHECK(ret, FAIL, "H5Aclose");

    ret = H5Gclose(root1);
    CHECK(ret, FAIL, "H5Gclose");

    ret = H5Fclose(file1);
    CHECK(ret, FAIL, "H5Fclose");

    root2 = H5Gopen2(file2, "/", H5P_DEFAULT);
    CHECK(root2, FAIL, "H5Gopen2");

    att2 = H5Acreate2(root2, MISC2_ATT_NAME_2, type, dataspace, H5P_DEFAULT, H5P_DEFAULT);
    CHECK(att2, FAIL, "H5Acreate2");

    data.string = string_att2;

    ret = H5Awrite(att2, type, &data);
    CHECK(ret, FAIL, "H5Awrite");

    ret = H5Aread(att2, type, &data_check);
    CHECK(ret, FAIL, "H5Aread");

    ret = H5Treclaim(type, dataspace, H5P_DEFAULT, &data_check);
    CHECK(ret, FAIL, "H5Treclaim");

    ret = H5Aclose(att2);
    CHECK(ret, FAIL, "H5Aclose");

    ret = H5Gclose(root2);
    CHECK(ret, FAIL, "H5Gclose");

    ret = H5Tclose(type);
    CHECK(ret, FAIL, "H5Tclose");

    ret = H5Sclose(dataspace);
    CHECK(ret, FAIL, "H5Sclose");

    ret = H5Fclose(file2);
    CHECK(ret, FAIL, "H5Fclose");

    free(string_att1);
    free(string_att2);
}

static void
test_misc2_read_attribute(const char *filename, const char *att_name)
{
    hid_t        file, root, att;
    hid_t        type;
    hid_t        space;
    herr_t       ret;
    misc2_struct data_check;

    type = misc2_create_type();

    file = H5Fopen(filename, H5F_ACC_RDONLY, H5P_DEFAULT);
    CHECK(file, FAIL, "H5Fopen");

    root = H5Gopen2(file, "/", H5P_DEFAULT);
    CHECK(root, FAIL, "H5Gopen2");

    att = H5Aopen(root, att_name, H5P_DEFAULT);
    CHECK(att, FAIL, "H5Aopen");

    space = H5Aget_space(att);
    CHECK(space, FAIL, "H5Aget_space");

    ret = H5Aread(att, type, &data_check);
    CHECK(ret, FAIL, "H5Aread");

    ret = H5Treclaim(type, space, H5P_DEFAULT, &data_check);
    CHECK(ret, FAIL, "H5Treclaim");

    ret = H5Sclose(space);
    CHECK(ret, FAIL, "H5Sclose");

    ret = H5Aclose(att);
    CHECK(ret, FAIL, "H5Aclose");

    ret = H5Tclose(type);
    CHECK(ret, FAIL, "H5Tclose");

    ret = H5Gclose(root);
    CHECK(ret, FAIL, "H5Gclose");

    ret = H5Fclose(file);
    CHECK(ret, FAIL, "H5Fclose");
}
/****************************************************************
**
**  test_misc2(): test using the same VL-derived datatype in two
**      different files, which was causing problems with the
**      datatype conversion functions
**
****************************************************************/
static void
test_misc2(void)
{
    /* Output message about test being performed */
    MESSAGE(5, ("Testing VL datatype in two different files\n"));

    test_misc2_write_attribute();
    test_misc2_read_attribute(MISC2_FILE_1, MISC2_ATT_NAME_1);
    test_misc2_read_attribute(MISC2_FILE_2, MISC2_ATT_NAME_2);
} /* end test_misc2() */

/****************************************************************
**
**  test_misc3(): Test reading from chunked dataset with non-zero
**      fill value
**
****************************************************************/
static void
test_misc3(void)
{
    hid_t   file, dataspace, dataset, dcpl;
    int     rank                   = MISC3_RANK;
    hsize_t dims[MISC3_RANK]       = {MISC3_DIM1, MISC3_DIM2};
    hsize_t chunk_dims[MISC3_RANK] = {MISC3_CHUNK_DIM1, MISC3_CHUNK_DIM2};
    int     fill                   = MISC3_FILL_VALUE;
    int     read_buf[MISC3_DIM1][MISC3_DIM2];
    int     i, j;
    herr_t  ret;

    /* Output message about test being performed */
    MESSAGE(5, ("Testing reading from chunked dataset with non-zero fill-value\n"));

    file = H5Fcreate(MISC3_FILE, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
    CHECK(file, FAIL, "H5Fcreate");

    /* Create a simple dataspace */
    dataspace = H5Screate_simple(rank, dims, NULL);
    CHECK(dataspace, FAIL, "H5Screate_simple");

    /* Create a dataset creation property list */
    dcpl = H5Pcreate(H5P_DATASET_CREATE);
    CHECK(dcpl, FAIL, "H5Pcreate");

    /* Set the chunk information */
    ret = H5Pset_chunk(dcpl, rank, chunk_dims);
    CHECK(dcpl, FAIL, "H5Pset_chunk");

    /* Set the fill-value information */
    ret = H5Pset_fill_value(dcpl, H5T_NATIVE_INT, &fill);
    CHECK(dcpl, FAIL, "H5Pset_fill_value");

    /* Create the dataset */
    dataset = H5Dcreate2(file, MISC3_DSET_NAME, H5T_NATIVE_INT, dataspace, H5P_DEFAULT, dcpl, H5P_DEFAULT);
    CHECK(dataset, FAIL, "H5Dcreate2");

    /* Read from the dataset (should be fill-values) */
    ret = H5Dread(dataset, H5T_NATIVE_INT, H5S_ALL, H5S_ALL, H5P_DEFAULT, &read_buf);
    CHECK(ret, FAIL, "H5Dread");

    for (i = 0; i < MISC3_DIM1; i++)
        for (j = 0; j < MISC3_DIM2; j++)
            VERIFY(read_buf[i][j], fill, "H5Dread");

    /* Release resources */
    ret = H5Pclose(dcpl);
    CHECK(ret, FAIL, "H5Pclose");

    ret = H5Sclose(dataspace);
    CHECK(ret, FAIL, "H5Sclose");

    ret = H5Dclose(dataset);
    CHECK(ret, FAIL, "H5Dclose");

    ret = H5Fclose(file);
    CHECK(ret, FAIL, "H5Fclose");
} /* end test_misc3() */

/****************************************************************
**
**  test_misc4(): Test the that 'fileno' field in H5O_info_t is
**      valid.
**
****************************************************************/
static void
test_misc4(void)
{
    hid_t       file1, file2, group1, group2, group3;
    H5O_info2_t oinfo1, oinfo2, oinfo3;
    herr_t      ret;

    /* Output message about test being performed */
    MESSAGE(5, ("Testing fileno working in H5O_info2_t\n"));

    file1 = H5Fcreate(MISC4_FILE_1, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
    CHECK(file1, FAIL, "H5Fcreate");

    /* Create the first group */
    group1 = H5Gcreate2(file1, MISC4_GROUP_1, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
    CHECK(group1, FAIL, "H5Gcreate2");

    /* Create the second group */
    group2 = H5Gcreate2(file1, MISC4_GROUP_2, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
    CHECK(group2, FAIL, "H5Gcreate2");

    file2 = H5Fcreate(MISC4_FILE_2, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
    CHECK(file2, FAIL, "H5Fcreate");

    /* Create the first group */
    group3 = H5Gcreate2(file2, MISC4_GROUP_1, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
    CHECK(group3, FAIL, "H5Gcreate2");

    /* Get the stat information for each group */
    ret = H5Oget_info_by_name3(file1, MISC4_GROUP_1, &oinfo1, H5O_INFO_BASIC, H5P_DEFAULT);
    CHECK(ret, FAIL, "H5Oget_info_by_name3");
    ret = H5Oget_info_by_name3(file1, MISC4_GROUP_2, &oinfo2, H5O_INFO_BASIC, H5P_DEFAULT);
    CHECK(ret, FAIL, "H5Oget_info_by_name3");
    ret = H5Oget_info_by_name3(file2, MISC4_GROUP_1, &oinfo3, H5O_INFO_BASIC, H5P_DEFAULT);
    CHECK(ret, FAIL, "H5Oget_info_by_name3");

    /* Verify that the fileno values are the same for groups from file1 */
    VERIFY(oinfo1.fileno, oinfo2.fileno, "H5Oget_info_by_name");

    /* Verify that the fileno values are not the same between file1 & file2 */
    if (oinfo1.fileno == oinfo3.fileno)
        TestErrPrintf("Error on line %d: oinfo1.fileno != oinfo3.fileno\n", __LINE__);
    if (oinfo2.fileno == oinfo3.fileno)
        TestErrPrintf("Error on line %d: oinfo2.fileno != oinfo3.fileno\n", __LINE__);

    /* Close the objects */
    ret = H5Gclose(group1);
    CHECK(ret, FAIL, "H5Gclose");

    ret = H5Gclose(group2);
    CHECK(ret, FAIL, "H5Gclose");

    ret = H5Gclose(group3);
    CHECK(ret, FAIL, "H5Gclose");

    ret = H5Fclose(file1);
    CHECK(ret, FAIL, "H5Fclose");

    ret = H5Fclose(file2);
    CHECK(ret, FAIL, "H5Fclose");
} /* end test_misc4() */

/****************************************************************
**
**  test_misc5(): Test several level deep nested compound & VL datatypes
**
****************************************************************/

/*********************** struct3 ***********************/

static misc5_struct3_hndl *
create_struct3(void)
{
    misc5_struct3_hndl *str3hndl; /* New 'struct3' created */
    herr_t              ret;      /* For error checking */

    str3hndl = (misc5_struct3_hndl *)malloc(sizeof(misc5_struct3_hndl));
    CHECK_PTR(str3hndl, "malloc");

    str3hndl->st3h_base = H5Tcreate(H5T_COMPOUND, sizeof(misc5_struct3));
    CHECK(str3hndl->st3h_base, FAIL, "H5Tcreate");

    ret = H5Tinsert(str3hndl->st3h_base, "st3_el1", HOFFSET(misc5_struct3, st3_el1), H5T_NATIVE_INT);
    CHECK(ret, FAIL, "H5Tinsert");

    str3hndl->st3h_id = H5Tvlen_create(str3hndl->st3h_base);
    CHECK(str3hndl->st3h_id, FAIL, "H5Tvlen_create");

    return str3hndl;
}

static void
delete_struct3(misc5_struct3_hndl *str3hndl)
{
    herr_t ret; /* For error checking */

    ret = H5Tclose(str3hndl->st3h_id);
    CHECK(ret, FAIL, "H5Tclose");

    ret = H5Tclose(str3hndl->st3h_base);
    CHECK(ret, FAIL, "H5Tclose");

    free(str3hndl);
}

static void
set_struct3(misc5_struct3 *buf)
{
    buf->st3_el1 = MISC5_DBGELVAL3;
}

/*********************** struct2 ***********************/

static misc5_struct2_hndl *
create_struct2(void)
{
    misc5_struct2_hndl *str2hndl; /* New 'struct2' created */
    herr_t              ret;      /* For error checking */

    str2hndl = (misc5_struct2_hndl *)malloc(sizeof(misc5_struct2_hndl));
    CHECK_PTR(str2hndl, "malloc");

    str2hndl->st2h_base = H5Tcreate(H5T_COMPOUND, sizeof(misc5_struct2));
    CHECK(str2hndl->st2h_base, FAIL, "H5Tcreate");

    ret = H5Tinsert(str2hndl->st2h_base, "st2_el1", HOFFSET(misc5_struct2, st2_el1), H5T_NATIVE_INT);
    CHECK(ret, FAIL, "H5Tinsert");

    str2hndl->st2h_st3hndl = create_struct3();
    CHECK_PTR(str2hndl->st2h_st3hndl, "create_struct3");

    ret = H5Tinsert(str2hndl->st2h_base, "st2_el2", HOFFSET(misc5_struct2, st2_el2),
                    str2hndl->st2h_st3hndl->st3h_id);
    CHECK(ret, FAIL, "H5Tinsert");

    str2hndl->st2h_id = H5Tvlen_create(str2hndl->st2h_base);
    CHECK(str2hndl->st2h_id, FAIL, "H5Tvlen_create");

    return str2hndl;
}

static void
delete_struct2(misc5_struct2_hndl *str2hndl)
{
    herr_t ret; /* For error checking */

    ret = H5Tclose(str2hndl->st2h_id);
    CHECK(ret, FAIL, "H5Tclose");

    delete_struct3(str2hndl->st2h_st3hndl);

    H5Tclose(str2hndl->st2h_base);
    CHECK(ret, FAIL, "H5Tclose");

    free(str2hndl);
}

static void
set_struct2(misc5_struct2 *buf)
{
    unsigned i; /* Local index variable */

    buf->st2_el1     = MISC5_DBGELVAL2;
    buf->st2_el2.len = MISC5_DBGNELM3;

    buf->st2_el2.p = malloc((buf->st2_el2.len) * sizeof(misc5_struct3));
    CHECK_PTR(buf->st2_el2.p, "malloc");

    for (i = 0; i < (buf->st2_el2.len); i++)
        set_struct3(&(((misc5_struct3 *)(buf->st2_el2.p))[i]));
}

static void
clear_struct2(misc5_struct2 *buf)
{
    free(buf->st2_el2.p);
}

/*********************** struct1 ***********************/

static misc5_struct1_hndl *
create_struct1(void)
{
    misc5_struct1_hndl *str1hndl; /* New 'struct1' created */
    herr_t              ret;      /* For error checking */

    str1hndl = (misc5_struct1_hndl *)malloc(sizeof(misc5_struct1_hndl));
    CHECK_PTR(str1hndl, "malloc");

    str1hndl->st1h_base = H5Tcreate(H5T_COMPOUND, sizeof(misc5_struct1));
    CHECK(str1hndl->st1h_base, FAIL, "H5Tcreate");

    ret = H5Tinsert(str1hndl->st1h_base, "st1_el1", HOFFSET(misc5_struct1, st1_el1), H5T_NATIVE_INT);
    CHECK(ret, FAIL, "H5Tinsert");

    str1hndl->st1h_st2hndl = create_struct2();
    CHECK_PTR(str1hndl->st1h_st2hndl, "create_struct2");

    ret = H5Tinsert(str1hndl->st1h_base, "st1_el2", HOFFSET(misc5_struct1, st1_el2),
                    str1hndl->st1h_st2hndl->st2h_id);
    CHECK(ret, FAIL, "H5Tinsert");

    str1hndl->st1h_id = H5Tvlen_create(str1hndl->st1h_base);
    CHECK(str1hndl->st1h_id, FAIL, "H5Tvlen_create");

    return str1hndl;
}

static void
delete_struct1(misc5_struct1_hndl *str1hndl)
{
    herr_t ret; /* For error checking */

    ret = H5Tclose(str1hndl->st1h_id);
    CHECK(ret, FAIL, "H5Tclose");

    delete_struct2(str1hndl->st1h_st2hndl);

    ret = H5Tclose(str1hndl->st1h_base);
    CHECK(ret, FAIL, "H5Tclose");

    free(str1hndl);
}

static void
set_struct1(misc5_struct1 *buf)
{
    unsigned i; /* Local index variable */

    buf->st1_el1     = MISC5_DBGELVAL1;
    buf->st1_el2.len = MISC5_DBGNELM2;

    buf->st1_el2.p = malloc((buf->st1_el2.len) * sizeof(misc5_struct2));
    CHECK_PTR(buf->st1_el2.p, "malloc");

    for (i = 0; i < (buf->st1_el2.len); i++)
        set_struct2(&(((misc5_struct2 *)(buf->st1_el2.p))[i]));
}

static void
clear_struct1(misc5_struct1 *buf)
{
    unsigned i;

    for (i = 0; i < buf->st1_el2.len; i++)
        clear_struct2(&(((misc5_struct2 *)(buf->st1_el2.p))[i]));
    free(buf->st1_el2.p);
}

static void
test_misc5(void)
{
    hid_t               loc_id, space_id, dataset_id;
    hid_t               mem_type_id;
    misc5_struct1_hndl *str1hndl;
    hsize_t             dims[MISC5_DSETRANK];
    hvl_t               buf;
    unsigned            i, j, k;
    herr_t              ret;

    /* Output message about test being performed */
    MESSAGE(5, ("Testing several level deep nested compound & VL datatypes \n"));

    /* Write the dataset out */
    loc_id = H5Fcreate(MISC5_FILE, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
    CHECK(loc_id, FAIL, "H5Fcreate");

    /* Create the memory structure to write */
    str1hndl = create_struct1();
    CHECK_PTR(str1hndl, "create_struct1");

    /* Create the dataspace */
    dims[0]  = MISC5_NELMTOPLVL;
    space_id = H5Screate_simple(MISC5_DSETRANK, dims, NULL);
    CHECK(space_id, FAIL, "H5Screate_simple");

    /* Create the dataset */
    dataset_id = H5Dcreate2(loc_id, MISC5_DSETNAME, str1hndl->st1h_id, space_id, H5P_DEFAULT, H5P_DEFAULT,
                            H5P_DEFAULT);
    CHECK(dataset_id, FAIL, "H5Dcreate2");

    /* Create the variable-length buffer */
    buf.len = MISC5_DBGNELM1;
    buf.p   = malloc((buf.len) * sizeof(misc5_struct1));
    CHECK_PTR(buf.p, "malloc");

    /* Create the top-level VL information */
    for (i = 0; i < MISC5_DBGNELM1; i++)
        set_struct1(&(((misc5_struct1 *)(buf.p))[i]));

    /* Write the data out */
    ret = H5Dwrite(dataset_id, str1hndl->st1h_id, H5S_ALL, H5S_ALL, H5P_DEFAULT, &buf);
    CHECK(ret, FAIL, "H5Dwrite");

    /* Release the top-level VL information */
    for (j = 0; j < MISC5_DBGNELM1; j++)
        clear_struct1(&(((misc5_struct1 *)(buf.p))[j]));

    /* Free the variable-length buffer */
    free(buf.p);

    /* Close dataset */
    ret = H5Dclose(dataset_id);
    CHECK(ret, FAIL, "H5Dclose");

    /* Close dataspace */
    ret = H5Sclose(space_id);
    CHECK(ret, FAIL, "H5Sclose");

    /* Delete memory structures */
    delete_struct1(str1hndl);

    /* Close file */
    ret = H5Fclose(loc_id);
    CHECK(ret, FAIL, "H5Fclose");

    /* Read the dataset back in & verify it */
    loc_id = H5Fopen(MISC5_FILE, H5F_ACC_RDONLY, H5P_DEFAULT);
    CHECK(loc_id, FAIL, "H5Fopen");

    /* Open dataset again */
    dataset_id = H5Dopen2(loc_id, MISC5_DSETNAME, H5P_DEFAULT);
    CHECK(dataset_id, FAIL, "H5Dopen2");

    /* Get the dataset's datatype */
    mem_type_id = H5Dget_type(dataset_id);
    CHECK(mem_type_id, FAIL, "H5Dget_type");

    /* Get the dataset's dataspace */
    space_id = H5Dget_space(dataset_id);
    CHECK(space_id, FAIL, "H5Dget_space");

    /* Read the data back in */
    ret = H5Dread(dataset_id, mem_type_id, H5S_ALL, H5S_ALL, H5P_DEFAULT, &buf);
    CHECK(ret, FAIL, "H5Dread");

    /* Verify the correct information was read in */
    for (i = 0; i < (buf.len); i++) {
        /* printf("[%d]=%d\n",i, ((misc5_struct1 *)(buf.p))[i].st1_el1); */
        VERIFY(((misc5_struct1 *)(buf.p))[i].st1_el1, MISC5_DBGELVAL1, "H5Dread");
        for (j = 0; j < (((misc5_struct1 *)(buf.p))[i].st1_el2.len); j++) {
            /* printf("   [%d]=%d\n",j, ((misc5_struct2 *)(((misc5_struct1 *)
             * (buf.p))[i].st1_el2.p))[j].st2_el1); */
            VERIFY(((misc5_struct2 *)(((misc5_struct1 *)(buf.p))[i].st1_el2.p))[j].st2_el1, MISC5_DBGELVAL2,
                   "H5Dread");
            for (k = 0; k < (((misc5_struct2 *)(((misc5_struct1 *)(buf.p))[i].st1_el2.p))[j].st2_el2.len);
                 k++) {
                /* printf("      [%d]=%d\n",k, ((misc5_struct3 *)(((misc5_struct2 *) (((misc5_struct1
                 * *)(buf.p))[i].  st1_el2.p))[j].st2_el2.p))[k].st3_el1); */
                VERIFY(((misc5_struct3 *)(((misc5_struct2 *)(((misc5_struct1 *)(buf.p))[i].st1_el2.p))[j]
                                              .st2_el2.p))[k]
                           .st3_el1,
                       MISC5_DBGELVAL3, "H5Dread");
            } /* end for */
        }
    }

    /* Reclaim the memory for the VL information */
    ret = H5Treclaim(mem_type_id, space_id, H5P_DEFAULT, &buf);
    CHECK(ret, FAIL, "H5Treclaim");

    /* Close dataspace */
    ret = H5Sclose(space_id);
    CHECK(ret, FAIL, "H5Sclose");

    /* Close dataset */
    ret = H5Tclose(mem_type_id);
    CHECK(ret, FAIL, "H5Tclose");

    /* Close dataset */
    ret = H5Dclose(dataset_id);
    CHECK(ret, FAIL, "H5Dclose");

    /* Close file */
    ret = H5Fclose(loc_id);
    CHECK(ret, FAIL, "H5Fclose");

} /* end test_misc5() */

/****************************************************************
**
**  test_misc6(): Test that object header continuation messages are
**      created correctly.
**
****************************************************************/
static void
test_misc6(void)
{
    hid_t    loc_id, space_id, dataset_id;
    hid_t    attr_id;
    char     attr_name[16];
    unsigned u;
    herr_t   ret;

    /* Output message about test being performed */
    MESSAGE(5, ("Testing object header continuation code \n"));

    /* Create the file */
    loc_id = H5Fcreate(MISC6_FILE, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
    CHECK(loc_id, FAIL, "H5Fcreate");

    /* Create the dataspace */
    space_id = H5Screate(H5S_SCALAR);
    CHECK(space_id, FAIL, "H5Screate");

    /* Create the first dataset */
    dataset_id =
        H5Dcreate2(loc_id, MISC6_DSETNAME1, H5T_NATIVE_INT, space_id, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
    CHECK(dataset_id, FAIL, "H5Dcreate2");

    /* Close dataset */
    ret = H5Dclose(dataset_id);
    CHECK(ret, FAIL, "H5Dclose");

    /* Create the second dataset */
    dataset_id =
        H5Dcreate2(loc_id, MISC6_DSETNAME2, H5T_NATIVE_INT, space_id, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
    CHECK(dataset_id, FAIL, "H5Dcreate2");

    /* Close dataset */
    ret = H5Dclose(dataset_id);
    CHECK(ret, FAIL, "H5Dclose");

    /* Close file */
    ret = H5Fclose(loc_id);
    CHECK(ret, FAIL, "H5Fclose");

    /* Loop through adding attributes to each dataset */
    for (u = 0; u < MISC6_NUMATTR; u++) {
        /* Create name for attribute */
        snprintf(attr_name, sizeof(attr_name), "Attr#%u", u);

        /* Open the file */
        loc_id = H5Fopen(MISC6_FILE, H5F_ACC_RDWR, H5P_DEFAULT);
        CHECK(loc_id, FAIL, "H5Fopen");

        /* Open first dataset */
        dataset_id = H5Dopen2(loc_id, MISC6_DSETNAME1, H5P_DEFAULT);
        CHECK(dataset_id, FAIL, "H5Dopen2");

        /* Add attribute to dataset */
        attr_id = H5Acreate2(dataset_id, attr_name, H5T_NATIVE_INT, space_id, H5P_DEFAULT, H5P_DEFAULT);
        CHECK(attr_id, FAIL, "H5Acreate2");

        /* Close attribute */
        ret = H5Aclose(attr_id);
        CHECK(ret, FAIL, "H5Aclose");

        /* Close dataset */
        ret = H5Dclose(dataset_id);
        CHECK(ret, FAIL, "H5Dclose");

        /* Open second dataset */
        dataset_id = H5Dopen2(loc_id, MISC6_DSETNAME2, H5P_DEFAULT);
        CHECK(dataset_id, FAIL, "H5Dopen2");

        /* Add attribute to dataset */
        attr_id = H5Acreate2(dataset_id, attr_name, H5T_NATIVE_INT, space_id, H5P_DEFAULT, H5P_DEFAULT);
        CHECK(attr_id, FAIL, "H5Acreate2");

        /* Close attribute */
        ret = H5Aclose(attr_id);
        CHECK(ret, FAIL, "H5Aclose");

        /* Close dataset */
        ret = H5Dclose(dataset_id);
        CHECK(ret, FAIL, "H5Dclose");

        /* Close file */
        ret = H5Fclose(loc_id);
        CHECK(ret, FAIL, "H5Fclose");
    } /* end for */

    /* Close dataspace */
    ret = H5Sclose(space_id);
    CHECK(ret, FAIL, "H5Sclose");

} /* end test_misc6() */

/****************************************************************
**
**  test_misc7(): Test that datatypes are sensible to store on
**      disk.  (i.e. not partially initialized)
**
****************************************************************/
static void
test_misc7(void)
{
    hid_t  fid, did, tid, sid;
    int    enum_value = 1;
    bool   vol_is_native;
    herr_t ret;

    /* Output message about test being performed */
    MESSAGE(5, ("Testing sensible datatype on disk code \n"));

    /* Attempt to commit a non-sensible datatype */

    /* Create the file */
    fid = H5Fcreate(MISC7_FILE, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
    CHECK(fid, FAIL, "H5Fcreate");

    /* Check if native VOL is being used */
    CHECK(h5_using_native_vol(H5P_DEFAULT, fid, &vol_is_native), FAIL, "h5_using_native_vol");
    if (!vol_is_native) {
        CHECK(H5Fclose(fid), FAIL, "H5Fclose");
        MESSAGE(5, (" -- SKIPPED --\n"));
        return;
    }

    /* Create the dataspace */
    sid = H5Screate(H5S_SCALAR);
    CHECK(sid, FAIL, "H5Screate");

    /* Create the compound datatype to commit*/
    tid = H5Tcreate(H5T_COMPOUND, (size_t)32);
    CHECK(tid, FAIL, "H5Tcreate");

    /* Attempt to commit an empty compound datatype */
    ret = H5Tcommit2(fid, MISC7_TYPENAME1, tid, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
    VERIFY(ret, FAIL, "H5Tcommit2");

    /* Attempt to use empty compound datatype to create dataset */
    did = H5Dcreate2(fid, MISC7_DSETNAME1, tid, sid, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
    VERIFY(ret, FAIL, "H5Dcreate2");

    /* Add a field to the compound datatype */
    ret = H5Tinsert(tid, "a", (size_t)0, H5T_NATIVE_INT);
    CHECK(ret, FAIL, "H5Tinsert");

    /* Attempt to commit the compound datatype now - should work */
    ret = H5Tcommit2(fid, MISC7_TYPENAME1, tid, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
    CHECK(ret, FAIL, "H5Tcommit2");

    /* Attempt to use compound datatype to create dataset now - should work */
    did = H5Dcreate2(fid, MISC7_DSETNAME1, tid, sid, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
    CHECK(did, FAIL, "H5Dcreate2");

    /* Close dataset */
    ret = H5Dclose(did);
    CHECK(ret, FAIL, "H5Dclose");

    /* Close compound datatype */
    ret = H5Tclose(tid);
    CHECK(ret, FAIL, "H5Tclose");

    /* Create the enum datatype to commit*/
    tid = H5Tenum_create(H5T_NATIVE_INT);
    CHECK(tid, FAIL, "H5Tenum_create");

    /* Attempt to commit an empty enum datatype */
    ret = H5Tcommit2(fid, MISC7_TYPENAME2, tid, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
    VERIFY(ret, FAIL, "H5Tcommit2");

    /* Attempt to use empty enum datatype to create dataset */
    did = H5Dcreate2(fid, MISC7_DSETNAME2, tid, sid, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
    VERIFY(did, FAIL, "H5Dcreate2");

    /* Add a member to the enum datatype */
    ret = H5Tenum_insert(tid, "a", &enum_value);
    CHECK(ret, FAIL, "H5Tenum_insert");

    /* Attempt to commit the enum datatype now - should work */
    ret = H5Tcommit2(fid, MISC7_TYPENAME2, tid, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
    CHECK(ret, FAIL, "H5Tcommit2");

    /* Attempt to use enum datatype to create dataset now - should work */
    did = H5Dcreate2(fid, MISC7_DSETNAME2, tid, sid, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
    CHECK(did, FAIL, "H5Dcreate2");

    /* Close dataset */
    ret = H5Dclose(did);
    CHECK(ret, FAIL, "H5Dclose");

    /* Close enum datatype */
    ret = H5Tclose(tid);
    CHECK(ret, FAIL, "H5Tclose");

    /* Close dataspace */
    ret = H5Sclose(sid);
    CHECK(ret, FAIL, "H5Sclose");

    /* Close file */
    ret = H5Fclose(fid);
    CHECK(ret, FAIL, "H5Fclose");

} /* end test_misc7() */

/****************************************************************
**
**  test_misc8(): Test storage size of various types of dataset
**      storage methods.
**
****************************************************************/
static void
test_misc8(void)
{
    hid_t   fid, did, sid;
    hid_t   fapl; /* File access property list */
    hid_t   dcpl; /* Dataset creation property list */
    int     rank                   = MISC8_RANK;
    hsize_t dims[MISC8_RANK]       = {MISC8_DIM0, MISC8_DIM1};
    hsize_t chunk_dims[MISC8_RANK] = {MISC8_CHUNK_DIM0, MISC8_CHUNK_DIM1};
    hsize_t storage_size; /* Number of bytes of raw data storage used */
    int    *wdata;        /* Data to write */
    int    *tdata;        /* Temporary pointer to data write */
#ifndef H5_HAVE_PARALLEL
    int *rdata;  /* Data to read */
    int *tdata2; /* Temporary pointer to data to read */
#endif
    unsigned u, v;              /* Local index variables */
    int      mdc_nelmts;        /* Metadata number of elements */
    size_t   rdcc_nelmts;       /* Raw data number of elements */
    size_t   rdcc_nbytes;       /* Raw data number of bytes */
    double   rdcc_w0;           /* Raw data write percentage */
    hsize_t  start[MISC8_RANK]; /* Hyperslab start */
    hsize_t  count[MISC8_RANK]; /* Hyperslab block count */
    bool     vol_is_native;
    herr_t   ret;

    /* Output message about test being performed */
    MESSAGE(5, ("Testing dataset storage sizes\n"));

    /* Create a file access property list */
    fapl = H5Pcreate(H5P_FILE_ACCESS);
    CHECK(fapl, FAIL, "H5Pcreate");

    /* Get the default file access properties for caching */
    ret = H5Pget_cache(fapl, &mdc_nelmts, &rdcc_nelmts, &rdcc_nbytes, &rdcc_w0);
    CHECK(ret, FAIL, "H5Pget_cache");

    /* Decrease the size of the raw data cache */
    rdcc_nbytes = 0;

    /* Set the file access properties for caching */
    ret = H5Pset_cache(fapl, mdc_nelmts, rdcc_nelmts, rdcc_nbytes, rdcc_w0);
    CHECK(ret, FAIL, "H5Pset_cache");

    /* Create the file */
    fid = H5Fcreate(MISC8_FILE, H5F_ACC_TRUNC, H5P_DEFAULT, fapl);
    CHECK(fid, FAIL, "H5Fcreate");

    /* Close file access property list */
    ret = H5Pclose(fapl);
    CHECK(ret, FAIL, "H5Pclose");

    /* Check if native VOL is being used */
    CHECK(h5_using_native_vol(H5I_INVALID_HID, fid, &vol_is_native), FAIL, "h5_using_native_vol");
    if (!vol_is_native) {
        CHECK(H5Fclose(fid), FAIL, "H5Fclose");
        MESSAGE(5, (" -- SKIPPED --\n"));
        return;
    }

    /* Allocate space for the data to write & read */
    wdata = (int *)malloc(sizeof(int) * MISC8_DIM0 * MISC8_DIM1);
    CHECK_PTR(wdata, "malloc");
#ifndef H5_HAVE_PARALLEL
    rdata = (int *)malloc(sizeof(int) * MISC8_DIM0 * MISC8_DIM1);
    CHECK_PTR(rdata, "malloc");
#endif

    /* Initialize values */
    tdata = wdata;
    for (u = 0; u < MISC8_DIM0; u++)
        for (v = 0; v < MISC8_DIM1; v++)
            *tdata++ = (int)(((u * MISC8_DIM1) + v) % 13);

    /* Create a simple dataspace */
    sid = H5Screate_simple(rank, dims, NULL);
    CHECK(sid, FAIL, "H5Screate_simple");

    /* Select a hyperslab which coincides with chunk boundaries */
    /* (For later use) */
    start[0] = 1;
    start[1] = 1;
    count[0] = (MISC8_CHUNK_DIM0 * 2) - 1;
    count[1] = (MISC8_CHUNK_DIM1 * 2) - 1;
    ret      = H5Sselect_hyperslab(sid, H5S_SELECT_SET, start, NULL, count, NULL);
    CHECK(ret, FAIL, "H5Sselect_hyperslab");

    /* Create a dataset creation property list */
    dcpl = H5Pcreate(H5P_DATASET_CREATE);
    CHECK(dcpl, FAIL, "H5Pcreate");

    /*          I.  contiguous dataset tests    */

    ret = H5Pset_layout(dcpl, H5D_CONTIGUOUS);
    CHECK(ret, FAIL, "H5Pset_layout");

    /* Set the space allocation time to early */
    ret = H5Pset_alloc_time(dcpl, H5D_ALLOC_TIME_EARLY);
    CHECK(ret, FAIL, "H5Pset_alloc_time");

    /* Create a contiguous dataset, with space allocation early */
    did = H5Dcreate2(fid, MISC8_DSETNAME1, H5T_NATIVE_INT, sid, H5P_DEFAULT, dcpl, H5P_DEFAULT);
    CHECK(did, FAIL, "H5Dcreate2");

    /* Check the storage size */
    storage_size = H5Dget_storage_size(did);
    CHECK(storage_size, 0, "H5Dget_storage_size");
    VERIFY(storage_size, (hsize_t)(MISC8_DIM0 * MISC8_DIM1 * H5Tget_size(H5T_NATIVE_INT)),
           "H5Dget_storage_size");

    /* Close dataset ID */
    ret = H5Dclose(did);
    CHECK(ret, FAIL, "H5Dclose");

#ifndef H5_HAVE_PARALLEL
    /* Set the space allocation time to late */
    ret = H5Pset_alloc_time(dcpl, H5D_ALLOC_TIME_LATE);
    CHECK(ret, FAIL, "H5Pset_alloc_time");

    /* Create a contiguous dataset, with space allocation late */
    did = H5Dcreate2(fid, MISC8_DSETNAME2, H5T_NATIVE_INT, sid, H5P_DEFAULT, dcpl, H5P_DEFAULT);
    CHECK(did, FAIL, "H5Dcreate2");

    /* Check the storage size before data is written */
    storage_size = H5Dget_storage_size(did);
    VERIFY(storage_size, 0, "H5Dget_storage_size");

    /* Write data */
    ret = H5Dwrite(did, H5T_NATIVE_INT, H5S_ALL, H5S_ALL, H5P_DEFAULT, wdata);
    CHECK(ret, FAIL, "H5Dwrite");

    /* Check the storage size after data is written */
    storage_size = H5Dget_storage_size(did);
    CHECK(storage_size, 0, "H5Dget_storage_size");
    VERIFY(storage_size, (hsize_t)(MISC8_DIM0 * MISC8_DIM1 * H5Tget_size(H5T_NATIVE_INT)),
           "H5Dget_storage_size");

    /* Close dataset ID */
    ret = H5Dclose(did);
    CHECK(ret, FAIL, "H5Dclose");

    /* Set the space allocation time to incremental */
    ret = H5Pset_alloc_time(dcpl, H5D_ALLOC_TIME_INCR);
    CHECK(ret, FAIL, "H5Pset_alloc_time");

    /* Create a contiguous dataset, with space allocation late */
    did = H5Dcreate2(fid, MISC8_DSETNAME3, H5T_NATIVE_INT, sid, H5P_DEFAULT, dcpl, H5P_DEFAULT);
    CHECK(did, FAIL, "H5Dcreate2");

    /* Check the storage size before data is written */
    storage_size = H5Dget_storage_size(did);
    VERIFY(storage_size, 0, "H5Dget_storage_size");

    /* Write data */
    ret = H5Dwrite(did, H5T_NATIVE_INT, H5S_ALL, H5S_ALL, H5P_DEFAULT, wdata);
    CHECK(ret, FAIL, "H5Dwrite");

    /* Check the storage size after data is written */
    storage_size = H5Dget_storage_size(did);
    CHECK(storage_size, 0, "H5Dget_storage_size");
    VERIFY(storage_size, (hsize_t)(MISC8_DIM0 * MISC8_DIM1 * H5Tget_size(H5T_NATIVE_INT)),
           "H5Dget_storage_size");

    /* Close dataset ID */
    ret = H5Dclose(did);
    CHECK(ret, FAIL, "H5Dclose");
#endif /* H5_HAVE_PARALLEL */

    /*          II.     compact dataset tests           */
    ret = H5Pset_layout(dcpl, H5D_COMPACT);
    CHECK(ret, FAIL, "H5Pset_layout");

    /* Set the space allocation time to late */
    ret = H5Pset_alloc_time(dcpl, H5D_ALLOC_TIME_LATE);
    CHECK(ret, FAIL, "H5Pset_alloc_time");

    /* Create a contiguous dataset, with space allocation late */
    /* Should fail */
    H5E_BEGIN_TRY
    {
        did = H5Dcreate2(fid, MISC8_DSETNAME4, H5T_NATIVE_INT, sid, H5P_DEFAULT, dcpl, H5P_DEFAULT);
    }
    H5E_END_TRY
    VERIFY(did, FAIL, "H5Dcreate2");

    /* Set the space allocation time to incremental */
    ret = H5Pset_alloc_time(dcpl, H5D_ALLOC_TIME_INCR);
    CHECK(ret, FAIL, "H5Pset_alloc_time");

    /* Create a contiguous dataset, with space allocation incremental */
    /* Should fail */
    H5E_BEGIN_TRY
    {
        did = H5Dcreate2(fid, MISC8_DSETNAME4, H5T_NATIVE_INT, sid, H5P_DEFAULT, dcpl, H5P_DEFAULT);
    }
    H5E_END_TRY
    VERIFY(did, FAIL, "H5Dcreate2");

    /* Set the space allocation time to early */
    ret = H5Pset_alloc_time(dcpl, H5D_ALLOC_TIME_EARLY);
    CHECK(ret, FAIL, "H5Pset_alloc_time");

    /* Set the fill time to allocation */
    ret = H5Pset_fill_time(dcpl, H5D_FILL_TIME_ALLOC);
    CHECK(ret, FAIL, "H5Pset_alloc_time");

    /* Create a contiguous dataset, with space allocation early */
    did = H5Dcreate2(fid, MISC8_DSETNAME4, H5T_NATIVE_INT, sid, H5P_DEFAULT, dcpl, H5P_DEFAULT);
    CHECK(did, FAIL, "H5Dcreate2");

    /* Check the storage size */
    storage_size = H5Dget_storage_size(did);
    CHECK(storage_size, 0, "H5Dget_storage_size");
    VERIFY(storage_size, (hsize_t)(MISC8_DIM0 * MISC8_DIM1 * H5Tget_size(H5T_NATIVE_INT)),
           "H5Dget_storage_size");

    /* Close dataset ID */
    ret = H5Dclose(did);
    CHECK(ret, FAIL, "H5Dclose");

    /*          III.    chunked dataset tests           */

    ret = H5Pset_layout(dcpl, H5D_CHUNKED);
    CHECK(ret, FAIL, "H5Pset_layout");

    /* Set the space allocation time to early */
    ret = H5Pset_alloc_time(dcpl, H5D_ALLOC_TIME_EARLY);
    CHECK(ret, FAIL, "H5Pset_alloc_time");

    /* Use chunked storage for this dataset */
    ret = H5Pset_chunk(dcpl, rank, chunk_dims);
    CHECK(ret, FAIL, "H5Pset_chunk");

    /* Create a chunked dataset, with space allocation early */
    did = H5Dcreate2(fid, MISC8_DSETNAME5, H5T_NATIVE_INT, sid, H5P_DEFAULT, dcpl, H5P_DEFAULT);
    CHECK(did, FAIL, "H5Dcreate2");

    /* Check the storage size after data is written */
    storage_size = H5Dget_storage_size(did);
    CHECK(storage_size, 0, "H5Dget_storage_size");
    VERIFY(storage_size, (hsize_t)(MISC8_DIM0 * MISC8_DIM1 * H5Tget_size(H5T_NATIVE_INT)),
           "H5Dget_storage_size");

    /* Close dataset ID */
    ret = H5Dclose(did);
    CHECK(ret, FAIL, "H5Dclose");

#ifndef H5_HAVE_PARALLEL
    /* Set the space allocation time to late */
    ret = H5Pset_alloc_time(dcpl, H5D_ALLOC_TIME_LATE);
    CHECK(ret, FAIL, "H5Pset_alloc_time");

    /* Use chunked storage for this dataset */
    ret = H5Pset_chunk(dcpl, rank, chunk_dims);
    CHECK(ret, FAIL, "H5Pset_chunk");

    /* Create a chunked dataset, with space allocation late */
    did = H5Dcreate2(fid, MISC8_DSETNAME6, H5T_NATIVE_INT, sid, H5P_DEFAULT, dcpl, H5P_DEFAULT);
    CHECK(did, FAIL, "H5Dcreate2");

    /* Check the storage size after dataset is created */
    storage_size = H5Dget_storage_size(did);
    VERIFY(storage_size, 0, "H5Dget_storage_size");

    /* Write part of the dataset */
    ret = H5Dwrite(did, H5T_NATIVE_INT, sid, sid, H5P_DEFAULT, wdata);
    CHECK(ret, FAIL, "H5Dwrite");

    /* Check the storage size after data is written */
    storage_size = H5Dget_storage_size(did);
    CHECK(storage_size, 0, "H5Dget_storage_size");
    VERIFY(storage_size, (hsize_t)(MISC8_DIM0 * MISC8_DIM1 * H5Tget_size(H5T_NATIVE_INT)),
           "H5Dget_storage_size");

    /* Close dataset ID */
    ret = H5Dclose(did);
    CHECK(ret, FAIL, "H5Dclose");

    /* Set the space allocation time to incremental */
    ret = H5Pset_alloc_time(dcpl, H5D_ALLOC_TIME_INCR);
    CHECK(ret, FAIL, "H5Pset_alloc_time");

    /* Create a chunked dataset, with space allocation incremental */
    did = H5Dcreate2(fid, MISC8_DSETNAME7, H5T_NATIVE_INT, sid, H5P_DEFAULT, dcpl, H5P_DEFAULT);
    CHECK(did, FAIL, "H5Dcreate2");

    /* Check the storage size before data is written */
    storage_size = H5Dget_storage_size(did);
    VERIFY(storage_size, 0, "H5Dget_storage_size");

    /* Write part of the dataset */
    ret = H5Dwrite(did, H5T_NATIVE_INT, sid, sid, H5P_DEFAULT, wdata);
    CHECK(ret, FAIL, "H5Dwrite");

    /* Check the storage size after only four chunks are written */
    storage_size = H5Dget_storage_size(did);
    VERIFY(storage_size, (hsize_t)(4 * MISC8_CHUNK_DIM0 * MISC8_CHUNK_DIM1 * H5Tget_size(H5T_NATIVE_INT)),
           "H5Dget_storage_size");

    /* Write entire dataset */
    ret = H5Dwrite(did, H5T_NATIVE_INT, H5S_ALL, H5S_ALL, H5P_DEFAULT, wdata);
    CHECK(ret, FAIL, "H5Dwrite");

    /* Read data */
    ret = H5Dread(did, H5T_NATIVE_INT, H5S_ALL, H5S_ALL, H5P_DEFAULT, rdata);
    CHECK(ret, FAIL, "H5Dread");

    /* Check values written */
    tdata  = wdata;
    tdata2 = rdata;
    for (u = 0; u < MISC8_DIM0; u++)
        for (v = 0; v < MISC8_DIM1; v++, tdata++, tdata2++)
            if (*tdata != *tdata2)
                TestErrPrintf("Error on line %d: u=%u, v=%d, *tdata=%d, *tdata2=%d\n", __LINE__, (unsigned)u,
                              (unsigned)v, (int)*tdata, (int)*tdata2);

    /* Check the storage size after data is written */
    storage_size = H5Dget_storage_size(did);
    CHECK(storage_size, 0, "H5Dget_storage_size");
    VERIFY(storage_size, (hsize_t)(MISC8_DIM0 * MISC8_DIM1 * H5Tget_size(H5T_NATIVE_INT)),
           "H5Dget_storage_size");

    /* Close dataset ID */
    ret = H5Dclose(did);
    CHECK(ret, FAIL, "H5Dclose");
#endif /* H5_HAVE_PARALLEL */

    /* Set the space allocation time to early */
    ret = H5Pset_alloc_time(dcpl, H5D_ALLOC_TIME_EARLY);
    CHECK(ret, FAIL, "H5Pset_alloc_time");

    /* Use compression as well as chunking for these datasets */
#ifdef H5_HAVE_FILTER_DEFLATE
    ret = H5Pset_deflate(dcpl, 9);
    CHECK(ret, FAIL, "H5Pset_deflate");
#endif /* end H5_HAVE_FILTER_DEFLATE */

    /* Create a chunked dataset, with space allocation early */
    did = H5Dcreate2(fid, MISC8_DSETNAME8, H5T_NATIVE_INT, sid, H5P_DEFAULT, dcpl, H5P_DEFAULT);
    CHECK(did, FAIL, "H5Dcreate2");

    /* Write part of the dataset */
    ret = H5Dwrite(did, H5T_NATIVE_INT, sid, sid, H5P_DEFAULT, wdata);
    CHECK(ret, FAIL, "H5Dwrite");

    /* Check the storage size after data is written */
    storage_size = H5Dget_storage_size(did);
    CHECK(storage_size, 0, "H5Dget_storage_size");
#ifdef H5_HAVE_FILTER_DEFLATE
    if (storage_size >= (MISC8_DIM0 * MISC8_DIM1 * H5Tget_size(H5T_NATIVE_INT)))
        TestErrPrintf("Error on line %d: data wasn't compressed! storage_size=%u\n", __LINE__,
                      (unsigned)storage_size);
#else  /* Compression is not configured */
    if (storage_size != (MISC8_DIM0 * MISC8_DIM1 * H5Tget_size(H5T_NATIVE_INT)))
        TestErrPrintf("Error on line %d: wrong storage size! storage_size=%u\n", __LINE__,
                      (unsigned)storage_size);
#endif /* H5_HAVE_FILTER_DEFLATE */

    /* Close dataset ID */
    ret = H5Dclose(did);
    CHECK(ret, FAIL, "H5Dclose");

#ifndef H5_HAVE_PARALLEL
    /* Set the space allocation time to late */
    ret = H5Pset_alloc_time(dcpl, H5D_ALLOC_TIME_LATE);
    CHECK(ret, FAIL, "H5Pset_alloc_time");

    /* Create a chunked dataset, with space allocation late */
    did = H5Dcreate2(fid, MISC8_DSETNAME9, H5T_NATIVE_INT, sid, H5P_DEFAULT, dcpl, H5P_DEFAULT);
    CHECK(did, FAIL, "H5Dcreate2");

    /* Check the storage size before data is written */
    storage_size = H5Dget_storage_size(did);
    VERIFY(storage_size, 0, "H5Dget_storage_size");

    /* Write part of the dataset */
    ret = H5Dwrite(did, H5T_NATIVE_INT, sid, sid, H5P_DEFAULT, wdata);
    CHECK(ret, FAIL, "H5Dwrite");

    /* Check the storage size after only four chunks are written */
    storage_size = H5Dget_storage_size(did);
    CHECK(storage_size, 0, "H5Dget_storage_size");
#ifdef H5_HAVE_FILTER_DEFLATE
    if (storage_size >= (MISC8_DIM0 * MISC8_DIM1 * H5Tget_size(H5T_NATIVE_INT)))
        TestErrPrintf("Error on line %d: data wasn't compressed! storage_size=%u\n", __LINE__,
                      (unsigned)storage_size);
#else  /* Compression is not configured */
    if (storage_size != (MISC8_DIM0 * MISC8_DIM1 * H5Tget_size(H5T_NATIVE_INT)))
        TestErrPrintf("Error on line %d: wrong storage size! storage_size=%u\n", __LINE__,
                      (unsigned)storage_size);
#endif /* H5_HAVE_FILTER_DEFLATE */

    /* Write entire dataset */
    ret = H5Dwrite(did, H5T_NATIVE_INT, H5S_ALL, H5S_ALL, H5P_DEFAULT, wdata);
    CHECK(ret, FAIL, "H5Dwrite");

    /* Read data */
    ret = H5Dread(did, H5T_NATIVE_INT, H5S_ALL, H5S_ALL, H5P_DEFAULT, rdata);
    CHECK(ret, FAIL, "H5Dread");

    /* Check values written */
    tdata  = wdata;
    tdata2 = rdata;
    for (u = 0; u < MISC8_DIM0; u++)
        for (v = 0; v < MISC8_DIM1; v++, tdata++, tdata2++)
            if (*tdata != *tdata2)
                TestErrPrintf("Error on line %d: u=%u, v=%d, *tdata=%d, *tdata2=%d\n", __LINE__, (unsigned)u,
                              (unsigned)v, (int)*tdata, (int)*tdata2);

    /* Check the storage size after data is written */
    storage_size = H5Dget_storage_size(did);
    CHECK(storage_size, 0, "H5Dget_storage_size");
#ifdef H5_HAVE_FILTER_DEFLATE
    if (storage_size >= (MISC8_DIM0 * MISC8_DIM1 * H5Tget_size(H5T_NATIVE_INT)))
        TestErrPrintf("Error on line %d: data wasn't compressed! storage_size=%u\n", __LINE__,
                      (unsigned)storage_size);
#else
    if (storage_size != (MISC8_DIM0 * MISC8_DIM1 * H5Tget_size(H5T_NATIVE_INT)))
        TestErrPrintf("Error on line %d: wrong storage size! storage_size=%u\n", __LINE__,
                      (unsigned)storage_size);
#endif /*H5_HAVE_FILTER_DEFLATE*/

    /* Close dataset ID */
    ret = H5Dclose(did);
    CHECK(ret, FAIL, "H5Dclose");

    /* Set the space allocation time to incremental */
    ret = H5Pset_alloc_time(dcpl, H5D_ALLOC_TIME_INCR);
    CHECK(ret, FAIL, "H5Pset_alloc_time");

    /* Create a chunked dataset, with space allocation incremental */
    did = H5Dcreate2(fid, MISC8_DSETNAME10, H5T_NATIVE_INT, sid, H5P_DEFAULT, dcpl, H5P_DEFAULT);
    CHECK(did, FAIL, "H5Dcreate2");

    /* Check the storage size before data is written */
    storage_size = H5Dget_storage_size(did);
    VERIFY(storage_size, 0, "H5Dget_storage_size");

    /* Write part of the dataset */
    ret = H5Dwrite(did, H5T_NATIVE_INT, sid, sid, H5P_DEFAULT, wdata);
    CHECK(ret, FAIL, "H5Dwrite");

    /* Check the storage size after only four chunks are written */
    storage_size = H5Dget_storage_size(did);
    CHECK(storage_size, 0, "H5Dget_storage_size");
#ifdef H5_HAVE_FILTER_DEFLATE
    if (storage_size >= (4 * MISC8_CHUNK_DIM0 * MISC8_CHUNK_DIM1 * H5Tget_size(H5T_NATIVE_INT)))
        TestErrPrintf("Error on line %d: data wasn't compressed! storage_size=%u\n", __LINE__,
                      (unsigned)storage_size);
#else  /* Compression is not configured */
    if (storage_size != (4 * MISC8_CHUNK_DIM0 * MISC8_CHUNK_DIM1 * H5Tget_size(H5T_NATIVE_INT)))
        TestErrPrintf("Error on line %d: wrong storage size! storage_size=%u\n", __LINE__,
                      (unsigned)storage_size);
#endif /* H5_HAVE_FILTER_DEFLATE */

    /* Write entire dataset */
    ret = H5Dwrite(did, H5T_NATIVE_INT, H5S_ALL, H5S_ALL, H5P_DEFAULT, wdata);
    CHECK(ret, FAIL, "H5Dwrite");

    /* Read data */
    ret = H5Dread(did, H5T_NATIVE_INT, H5S_ALL, H5S_ALL, H5P_DEFAULT, rdata);
    CHECK(ret, FAIL, "H5Dread");

    /* Check values written */
    tdata  = wdata;
    tdata2 = rdata;
    for (u = 0; u < MISC8_DIM0; u++)
        for (v = 0; v < MISC8_DIM1; v++, tdata++, tdata2++)
            if (*tdata != *tdata2)
                TestErrPrintf("Error on line %d: u=%u, v=%d, *tdata=%d, *tdata2=%d\n", __LINE__, (unsigned)u,
                              (unsigned)v, (int)*tdata, (int)*tdata2);

    /* Check the storage size after data is written */
    storage_size = H5Dget_storage_size(did);
    CHECK(storage_size, 0, "H5Dget_storage_size");
#ifdef H5_HAVE_FILTER_DEFLATE
    if (storage_size >= (MISC8_DIM0 * MISC8_DIM1 * H5Tget_size(H5T_NATIVE_INT)))
        TestErrPrintf("Error on line %d: data wasn't compressed! storage_size=%u\n", __LINE__,
                      (unsigned)storage_size);
#else
    if (storage_size != (MISC8_DIM0 * MISC8_DIM1 * H5Tget_size(H5T_NATIVE_INT)))
        TestErrPrintf("Error on line %d: wrong storage size! storage_size=%u\n", __LINE__,
                      (unsigned)storage_size);
#endif /*H5_HAVE_FILTER_DEFLATE*/

    /* Close dataset ID */
    ret = H5Dclose(did);
    CHECK(ret, FAIL, "H5Dclose");
#endif /* H5_HAVE_PARALLEL */

    /* Close dataset creation property list */
    ret = H5Pclose(dcpl);
    CHECK(ret, FAIL, "H5Pclose");

    /* Close dataspace */
    ret = H5Sclose(sid);
    CHECK(ret, FAIL, "H5Sclose");

    /* Close file */
    ret = H5Fclose(fid);
    CHECK(ret, FAIL, "H5Fclose");

    /* Free the read & write buffers */
    free(wdata);
#ifndef H5_HAVE_PARALLEL
    free(rdata);
#endif
} /* end test_misc8() */

/****************************************************************
**
**  test_misc9(): Test that H5Fopen() does not succeed for core
**      files, H5Fcreate() must be used to open them.
**
****************************************************************/
static void
test_misc9(void)
{
    hid_t  fapl, fid;
    herr_t ret;

    /* Output message about test being performed */
    MESSAGE(5, ("Testing core file opening\n"));

    fapl = H5Pcreate(H5P_FILE_ACCESS);
    CHECK(fapl, FAIL, "H5Pcreate");

    ret = H5Pset_fapl_core(fapl, (size_t)1024, 0);
    CHECK(ret, FAIL, "H5Pset_fapl_core");

    H5E_BEGIN_TRY
    {
        fid = H5Fopen(MISC9_FILE, H5F_ACC_RDWR, fapl);
    }
    H5E_END_TRY
    VERIFY(fid, FAIL, "H5Fopen");

    ret = H5Pclose(fapl);
    CHECK(ret, FAIL, "H5Pset_fapl_core");
} /* end test_misc9() */

/****************************************************************
**
**  test_misc10(): Test opening a dataset created with an older
**      version of the library (shares the tmtimeo.h5 file with the mtime.c
**      test - see notes in gen_old_mtime.c for notes on generating this
**      data file) and using the dataset creation property list from
**      that dataset to create a dataset with the current version of
**      the library.  Also tests using file creation property in same way.
**
****************************************************************/
static void
test_misc10(void)
{
    hid_t       file, file_new;                                     /* File IDs for old & new files */
    hid_t       fcpl;                                               /* File creation property list */
    hid_t       dataset, dataset_new;                               /* Dataset IDs for old & new datasets */
    hid_t       dcpl;                                               /* Dataset creation property list */
    hid_t       space, type;                                        /* Old dataset's dataspace & datatype */
    const char *testfile = H5_get_srcdir_filename(MISC10_FILE_OLD); /* Corrected test file name */
    bool        vol_is_native;
    bool        driver_is_default_compatible;
    herr_t      ret;

    /* Output message about test being performed */
    MESSAGE(5, ("Testing using old dataset creation property list\n"));

    /* Check if VFD used is native file format compatible */
    CHECK(h5_driver_is_default_vfd_compatible(H5P_DEFAULT, &driver_is_default_compatible), FAIL,
          "h5_driver_is_default_vfd_compatible");
    if (!driver_is_default_compatible) {
        MESSAGE(5, (" -- SKIPPED --\n"));
        return;
    }

    /*
     * Open the old file and the dataset and get old settings.
     */
    file = H5Fopen(testfile, H5F_ACC_RDONLY, H5P_DEFAULT);
    CHECK(file, FAIL, "H5Fopen");

    /* Check if native VOL is being used */
    CHECK(h5_using_native_vol(H5P_DEFAULT, file, &vol_is_native), FAIL, "h5_using_native_vol");
    if (!vol_is_native) {
        CHECK(H5Fclose(file), FAIL, "H5Fclose");
        MESSAGE(5, (" -- SKIPPED --\n"));
        return;
    }

    fcpl = H5Fget_create_plist(file);
    CHECK(fcpl, FAIL, "H5Fget_create_plist");

    dataset = H5Dopen2(file, MISC10_DSETNAME, H5P_DEFAULT);
    CHECK(dataset, FAIL, "H5Dopen2");
    dcpl = H5Dget_create_plist(dataset);
    CHECK(dcpl, FAIL, "H5Dget_create_plist");
    space = H5Dget_space(dataset);
    CHECK(space, FAIL, "H5Dget_space");
    type = H5Dget_type(dataset);
    CHECK(type, FAIL, "H5Dget_type");

    /* Create new file & dataset */
    file_new = H5Fcreate(MISC10_FILE_NEW, H5F_ACC_TRUNC, fcpl, H5P_DEFAULT);
    CHECK(file_new, FAIL, "H5Fcreate");

    dataset_new = H5Dcreate2(file_new, MISC10_DSETNAME, type, space, H5P_DEFAULT, dcpl, H5P_DEFAULT);
    CHECK(dataset_new, FAIL, "H5Dcreate2");

    /* Close new dataset & file */
    ret = H5Dclose(dataset_new);
    CHECK(ret, FAIL, "H5Dclose");
    ret = H5Fclose(file_new);
    CHECK(ret, FAIL, "H5Fclose");

    /* Close old dataset information */
    ret = H5Tclose(type);
    CHECK(ret, FAIL, "H5Tclose");
    ret = H5Sclose(space);
    CHECK(ret, FAIL, "H5Sclose");
    ret = H5Dclose(dataset);
    CHECK(ret, FAIL, "H5Dclose");
    ret = H5Pclose(dcpl);
    CHECK(ret, FAIL, "H5Pclose");

    /* Close old file information */
    ret = H5Fclose(file);
    CHECK(ret, FAIL, "H5Fclose");
    ret = H5Pclose(fcpl);
    CHECK(ret, FAIL, "H5Pclose");
} /* end test_misc10() */

/****************************************************************
**
**  test_misc11(): Test that all properties in a file creation property
**      list are stored correctly in the file and can be retrieved
**      when the file is re-opened.
**
****************************************************************/
static void
test_misc11(void)
{
    hid_t                 file;      /* File IDs for old & new files */
    hid_t                 fcpl;      /* File creation property list */
    hsize_t               userblock; /* Userblock size retrieved from FCPL */
    size_t                off_size;  /* Size of offsets in the file */
    size_t                len_size;  /* Size of lengths in the file */
    unsigned              sym_ik;    /* Symbol table B-tree initial 'K' value */
    unsigned              istore_ik; /* Indexed storage B-tree initial 'K' value */
    unsigned              sym_lk;    /* Symbol table B-tree leaf 'K' value */
    unsigned              nindexes;  /* Shared message number of indexes */
    H5F_info2_t           finfo;     /* global information about file */
    H5F_fspace_strategy_t strategy;  /* File space strategy */
    hsize_t               threshold; /* Free-space section threshold */
    bool                  persist;   /* To persist free-space or not */
    bool                  vol_is_native;
    herr_t                ret; /* Generic return value */

    /* Output message about test being performed */
    MESSAGE(5, ("Testing file creation properties retrieved correctly\n"));

    /* Creating a file with the default file creation property list should
     * create a version 0 superblock
     */

    /* Create file with default file creation property list */
    file = H5Fcreate(MISC11_FILE, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
    CHECK(file, FAIL, "H5Fcreate");

    /* Check if native VOL is being used */
    CHECK(h5_using_native_vol(H5P_DEFAULT, file, &vol_is_native), FAIL, "h5_using_native_vol");

    if (vol_is_native) {
        /* Get the file's version information */
        ret = H5Fget_info2(file, &finfo);
        CHECK(ret, FAIL, "H5Fget_info2");
        VERIFY(finfo.super.version, 0, "H5Fget_info2");
        VERIFY(finfo.free.version, 0, "H5Fget_info2");
        VERIFY(finfo.sohm.version, 0, "H5Fget_info2");
    }

    /* Close file */
    ret = H5Fclose(file);
    CHECK(ret, FAIL, "H5Fclose");

    /* Create a file creation property list */
    fcpl = H5Pcreate(H5P_FILE_CREATE);
    CHECK(fcpl, FAIL, "H5Pcreate");

    /* Set all the properties in the FCPL */
    ret = H5Pset_userblock(fcpl, (hsize_t)MISC11_USERBLOCK);
    CHECK(ret, FAIL, "H5Pset_userblock");

    ret = H5Pset_sizes(fcpl, (size_t)MISC11_SIZEOF_OFF, (size_t)MISC11_SIZEOF_LEN);
    CHECK(ret, FAIL, "H5Pset_sizes");

    /* This should fail as (32770*2) will exceed ^16 - 2 bytes for storing btree entries */
    H5E_BEGIN_TRY
    {
        ret = H5Pset_sym_k(fcpl, 32770, 0);
    }
    H5E_END_TRY
    VERIFY(ret, FAIL, "H5Pset_sym_k");

    ret = H5Pset_sym_k(fcpl, MISC11_SYM_IK, MISC11_SYM_LK);
    CHECK(ret, FAIL, "H5Pset_sym_k");

    /* This should fail as (32770*2) will exceed ^16 - 2 bytes for storing btree entries */
    H5E_BEGIN_TRY
    {
        ret = H5Pset_istore_k(fcpl, 32770);
    }
    H5E_END_TRY
    VERIFY(ret, FAIL, "H5Pset_istore_k");

    ret = H5Pset_istore_k(fcpl, MISC11_ISTORE_IK);
    CHECK(ret, FAIL, "H5Pset_istore_k");

    ret = H5Pset_shared_mesg_nindexes(fcpl, MISC11_NINDEXES);
    CHECK(ret, FAIL, "H5Pset_shared_mesg");

    ret = H5Pset_file_space_strategy(fcpl, H5F_FSPACE_STRATEGY_NONE, false, (hsize_t)1);
    CHECK(ret, FAIL, "H5Pset_file_space");

    /* Creating a file with the non-default file creation property list should
     * create a version 2 superblock
     */

    /* Create file with custom file creation property list */
    file = H5Fcreate(MISC11_FILE, H5F_ACC_TRUNC, fcpl, H5P_DEFAULT);
    CHECK(file, FAIL, "H5Fcreate");

    /* Check if native VOL is being used */
    CHECK(h5_using_native_vol(H5P_DEFAULT, file, &vol_is_native), FAIL, "h5_using_native_vol");

    /* Close FCPL */
    ret = H5Pclose(fcpl);
    CHECK(ret, FAIL, "H5Pclose");

    if (vol_is_native) {
        /* Get the file's version information */
        ret = H5Fget_info2(file, &finfo);
        CHECK(ret, FAIL, "H5Fget_info2");
        VERIFY(finfo.super.version, 2, "H5Fget_info2");
        VERIFY(finfo.free.version, 0, "H5Fget_info2");
        VERIFY(finfo.sohm.version, 0, "H5Fget_info2");
    }

    /* Close file */
    ret = H5Fclose(file);
    CHECK(ret, FAIL, "H5Fclose");

    /* Re-open the file */
    file = H5Fopen(MISC11_FILE, H5F_ACC_RDONLY, H5P_DEFAULT);
    CHECK(file, FAIL, "H5Fcreate");

    /* Check if native VOL is being used */
    CHECK(h5_using_native_vol(H5P_DEFAULT, file, &vol_is_native), FAIL, "h5_using_native_vol");

    /* Get the file's creation property list */
    fcpl = H5Fget_create_plist(file);
    CHECK(fcpl, FAIL, "H5Fget_create_plist");

    if (vol_is_native) {
        /* Get the file's version information */
        ret = H5Fget_info2(file, &finfo);
        CHECK(ret, FAIL, "H5Fget_info2");
        VERIFY(finfo.super.version, 2, "H5Fget_info2");
        VERIFY(finfo.free.version, 0, "H5Fget_info2");
        VERIFY(finfo.sohm.version, 0, "H5Fget_info2");
    }

    /* Retrieve all the property values & check them */
    ret = H5Pget_userblock(fcpl, &userblock);
    CHECK(ret, FAIL, "H5Pget_userblock");
    VERIFY(userblock, MISC11_USERBLOCK, "H5Pget_userblock");

    ret = H5Pget_sizes(fcpl, &off_size, &len_size);
    CHECK(ret, FAIL, "H5Pget_sizes");
    VERIFY(off_size, MISC11_SIZEOF_OFF, "H5Pget_sizes");
    VERIFY(len_size, MISC11_SIZEOF_LEN, "H5Pget_sizes");

    ret = H5Pget_sym_k(fcpl, &sym_ik, &sym_lk);
    CHECK(ret, FAIL, "H5Pget_sym_k");
    VERIFY(sym_ik, MISC11_SYM_IK, "H5Pget_sym_k");
    VERIFY(sym_lk, MISC11_SYM_LK, "H5Pget_sym_k");

    ret = H5Pget_istore_k(fcpl, &istore_ik);
    CHECK(ret, FAIL, "H5Pget_istore_k");
    VERIFY(istore_ik, MISC11_ISTORE_IK, "H5Pget_istore_k");

    ret = H5Pget_shared_mesg_nindexes(fcpl, &nindexes);
    CHECK(ret, FAIL, "H5Pget_shared_mesg_nindexes");
    VERIFY(nindexes, MISC11_NINDEXES, "H5Pget_shared_mesg_nindexes");

    ret = H5Pget_file_space_strategy(fcpl, &strategy, &persist, &threshold);
    CHECK(ret, FAIL, "H5Pget_file_space_strategy");
    VERIFY(strategy, 3, "H5Pget_file_space_strategy");
    VERIFY(persist, false, "H5Pget_file_space_strategy");
    VERIFY(threshold, 1, "H5Pget_file_space_strategy");

    /* Close file */
    ret = H5Fclose(file);
    CHECK(ret, FAIL, "H5Fclose");

    /* Close FCPL */
    ret = H5Pclose(fcpl);
    CHECK(ret, FAIL, "H5Pclose");
} /* end test_misc11() */

/****************************************************************
**
**  test_misc12(): Test that VL-types operate correctly in chunked
**      datasets that are extended.
**
****************************************************************/
static void
test_misc12(void)
{
    const char *wdata[MISC12_SPACE1_DIM1] = {
        "Four score and seven years ago our forefathers brought forth on this continent a new nation,",
        "conceived in liberty and dedicated to the proposition that all men are created equal.",
        "Now we are engaged in a great civil war,",
        "testing whether that nation or any nation so conceived and so dedicated can long endure."};
    const char *wdata1[MISC12_APPEND_SIZE] = {
        "O Gloria inmarcesible! O Jubilo inmortal! En surcos de dolores, el",
        "bien germina ya! Ceso la horrible noche, La libertad sublime",
        "derrama las auroras de su invencible luz.", "La humanidad entera, que entre cadenas gime, comprende",
        "las palabras del que murio en la cruz."};
    char   *rdata[MISC12_SPACE1_DIM1 + MISC12_APPEND_SIZE]; /* Information read in */
    hid_t   fid1;
    hid_t   dataset;
    hid_t   sid1, space, memspace;
    hid_t   tid1, cparms;
    hsize_t dims1[]     = {MISC12_SPACE1_DIM1};
    hsize_t dimsn[]     = {MISC12_APPEND_SIZE};
    hsize_t maxdims1[1] = {H5S_UNLIMITED};
    hsize_t chkdims1[1] = {MISC12_CHUNK_SIZE};
    hsize_t newsize[1]  = {MISC12_SPACE1_DIM1 + MISC12_APPEND_SIZE};
    hsize_t offset[1]   = {MISC12_SPACE1_DIM1};
    hsize_t count[1]    = {MISC12_APPEND_SIZE};
    int     i;   /* counting variable */
    herr_t  ret; /* Generic return value  */

    /* Output message about test being performed */
    MESSAGE(5, ("Testing VL-type in chunked dataset\n"));

    /* This test requirese a relatively "fresh" library environment */
    ret = H5garbage_collect();
    CHECK(ret, FAIL, "H5garbage_collect");

    /* Create file */
    fid1 = H5Fcreate(MISC12_FILE, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
    CHECK(fid1, FAIL, "H5Fcreate");

    /* Create dataspace for datasets */
    sid1 = H5Screate_simple(MISC12_SPACE1_RANK, dims1, maxdims1);
    CHECK(sid1, FAIL, "H5Screate_simple");

    /* Create a datatype to refer to */
    tid1 = H5Tcopy(H5T_C_S1);
    CHECK(tid1, FAIL, "H5Tcopy");

    ret = H5Tset_size(tid1, H5T_VARIABLE);
    CHECK(ret, FAIL, "H5Tset_size");

    cparms = H5Pcreate(H5P_DATASET_CREATE);
    CHECK(cparms, FAIL, "H5Pcreate");

    ret = H5Pset_chunk(cparms, 1, chkdims1);
    CHECK(ret, FAIL, "H5Pset_chunk");

    /* Create a dataset */
    dataset = H5Dcreate2(fid1, MISC12_DSET_NAME, tid1, sid1, H5P_DEFAULT, cparms, H5P_DEFAULT);
    CHECK(dataset, FAIL, "H5Dcreate2");

    /* Write dataset to disk */
    ret = H5Dwrite(dataset, tid1, H5S_ALL, H5S_ALL, H5P_DEFAULT, wdata);
    CHECK(ret, FAIL, "H5Dwrite");

    /* Extend dataset */
    ret = H5Dset_extent(dataset, newsize);
    CHECK(ret, FAIL, "H5Dset_extent");

    memspace = H5Screate_simple(MISC12_SPACE1_RANK, dimsn, NULL);
    CHECK(memspace, FAIL, "H5Screate_simple");

    space = H5Dget_space(dataset);
    CHECK(space, FAIL, "H5Dget_space");

    ret = H5Sselect_hyperslab(space, H5S_SELECT_SET, offset, NULL, count, NULL);
    CHECK(ret, FAIL, "H5Sselect_hyperslab");

    /* Write data to new portion of dataset */
    ret = H5Dwrite(dataset, tid1, memspace, space, H5P_DEFAULT, wdata1);
    CHECK(ret, FAIL, "H5Dwrite");

    /* Read all data back */
    ret = H5Dread(dataset, tid1, H5S_ALL, H5S_ALL, H5P_DEFAULT, rdata);
    CHECK(ret, FAIL, "H5Dread");

    for (i = 0; i < MISC12_SPACE1_DIM1; i++)
        if (strcmp(wdata[i], rdata[i]) != 0)
            TestErrPrintf("Error on line %d: wdata[%d]=%s, rdata[%d]=%s\n", __LINE__, i, wdata[i], i,
                          rdata[i]);
    for (; i < (MISC12_SPACE1_DIM1 + MISC12_APPEND_SIZE); i++)
        if (strcmp(wdata1[i - MISC12_SPACE1_DIM1], rdata[i]) != 0)
            TestErrPrintf("Error on line %d: wdata1[%d]=%s, rdata[%d]=%s\n", __LINE__, i - MISC12_SPACE1_DIM1,
                          wdata1[i - MISC12_SPACE1_DIM1], i, rdata[i]);

    ret = H5Sselect_all(space);
    CHECK(ret, FAIL, "H5Sselect_all");

    /* Reclaim VL data memory */
    ret = H5Treclaim(tid1, space, H5P_DEFAULT, rdata);
    CHECK(ret, FAIL, "H5Treclaim");

    /* Close Everything */
    ret = H5Dclose(dataset);
    CHECK(ret, FAIL, "H5Dclose");
    ret = H5Tclose(tid1);
    CHECK(ret, FAIL, "H5Tclose");
    ret = H5Sclose(space);
    CHECK(ret, FAIL, "H5Sclose");
    ret = H5Sclose(memspace);
    CHECK(ret, FAIL, "H5Sclose");
    ret = H5Sclose(sid1);
    CHECK(ret, FAIL, "H5Sclose");
    ret = H5Pclose(cparms);
    CHECK(ret, FAIL, "H5Pclose");
    ret = H5Fclose(fid1);
    CHECK(ret, FAIL, "H5Fclose");
} /* end test_misc12() */

/* Various routines for misc. 13 test */
static void
misc13_init_data(unsigned *original_data)
{
    unsigned u;

    for (u = 0; u < MISC13_DIM1; u++)
        original_data[u] = u;
}

static bool
misc13_verify_data_match(const unsigned *original_data, const unsigned *read_data)
{
    unsigned u;

    for (u = 0; u < MISC13_DIM1; u++)
        if (original_data[u] != read_data[u])
            return false;

    return true;
}

static void
misc13_create_dataset(hid_t loc_id, const char *name, hid_t dcpl, const unsigned *data)
{
    hid_t   dsid = H5I_INVALID_HID; /* Dataset ID */
    hid_t   sid  = H5I_INVALID_HID; /* Dataspace ID */
    hsize_t dims[MISC13_RANK];      /* Dataset dimensions */
    herr_t  ret;                    /* Generic return value */

    /* Create dataspace for use with dataset */
    dims[0] = MISC13_DIM1;
    sid     = H5Screate_simple(MISC13_RANK, dims, NULL);
    CHECK(sid, FAIL, "H5Screate_simple");

    /* Create contiguous dataset in root group */
    dsid = H5Dcreate2(loc_id, name, H5T_NATIVE_UINT, sid, H5P_DEFAULT, dcpl, H5P_DEFAULT);
    CHECK(dsid, FAIL, "H5Dcreate2");

    /* Write some data to dataset */
    ret = H5Dwrite(dsid, H5T_NATIVE_UINT, H5S_ALL, H5S_ALL, H5P_DEFAULT, data);
    CHECK(ret, FAIL, "H5Dwrite");

    /* Close the contiguous dataset */
    ret = H5Dclose(dsid);
    CHECK(ret, FAIL, "H5Dclose");

    /* Close the dataspace */
    ret = H5Sclose(sid);
    CHECK(ret, FAIL, "H5Sclose");

} /* end misc13_create_dataset() */

static void
misc13_verify_dataset(hid_t loc_id, const char *name, const unsigned *data)
{
    unsigned *read_data = NULL;            /* Data to write to dataset */
    hid_t     dsid      = H5I_INVALID_HID; /* Dataset ID */
    herr_t    ret;                         /* Generic return value */

    /* Create a data buffer for the dataset read */
    read_data = (unsigned *)calloc(MISC13_DIM1, sizeof(unsigned));
    CHECK_PTR(read_data, "calloc");

    /* Open the contiguous dataset in the root group */
    dsid = H5Dopen2(loc_id, name, H5P_DEFAULT);
    CHECK(dsid, FAIL, "H5Dopen2");

    /* Read the data */
    ret = H5Dread(dsid, H5T_NATIVE_UINT, H5S_ALL, H5S_ALL, H5P_DEFAULT, read_data);
    CHECK(ret, FAIL, "H5Dread");

    /* Verify that the data are correct */
    ret = misc13_verify_data_match(data, read_data);
    CHECK(ret, FAIL, "misc13_verify_data_match");

    /* Close the contiguous dataset */
    ret = H5Dclose(dsid);
    CHECK(ret, FAIL, "H5Dclose");

    /* Free the dataset read buffer */
    free(read_data);

} /* end misc13_verify_dataset() */

static void
misc13_create_hdf_file(const char *name, const unsigned *data)
{
    hid_t   fid    = H5I_INVALID_HID; /* File ID */
    hid_t   gid1   = H5I_INVALID_HID; /* Group ID (level 1) */
    hid_t   gid2   = H5I_INVALID_HID; /* Group ID (level 2) */
    hid_t   tid    = H5I_INVALID_HID; /* Datatype ID */
    hid_t   dcplid = H5I_INVALID_HID; /* Dataset creation property list ID */
    hsize_t chunk_dims[MISC13_RANK];  /* Chunk dimensions */
    herr_t  ret;                      /* Generic return value */

    /* Create file */
    fid = H5Fcreate(name, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
    CHECK(fid, FAIL, "H5Fcreate");

    /* Create DCPL for use with datasets */
    dcplid = H5Pcreate(H5P_DATASET_CREATE);
    CHECK(dcplid, FAIL, "H5Pcreate");

    /* Set the DCPL to be chunked */
    ret = H5Pset_layout(dcplid, H5D_CHUNKED);
    CHECK(ret, FAIL, "H5Pset_layout");

    /* Use chunked storage for this DCPL */
    chunk_dims[0] = MISC13_CHUNK_DIM1;
    ret           = H5Pset_chunk(dcplid, MISC13_RANK, chunk_dims);
    CHECK(ret, FAIL, "H5Pset_chunk");

    /* Create contiguous dataset in root group */
    misc13_create_dataset(fid, MISC13_DSET1_NAME, H5P_DEFAULT, data);

    /* Create chunked dataset in root group */
    misc13_create_dataset(fid, MISC13_DSET2_NAME, dcplid, data);

    /* Create a datatype to commit to the file */
    tid = H5Tcopy(H5T_NATIVE_INT);
    CHECK(tid, FAIL, "H5Tcopy");

    /* Create a named datatype in the root group */
    ret = H5Tcommit2(fid, MISC13_DTYPE_NAME, tid, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
    CHECK(ret, FAIL, "H5Tcommit2");

    /* Close named datatype */
    ret = H5Tclose(tid);
    CHECK(ret, FAIL, "H5Tclose");

    /* Create a group in the root group */
    gid1 = H5Gcreate2(fid, MISC13_GROUP1_NAME, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
    CHECK(gid1, FAIL, "H5Gcreate2");

    /* Create another group in the new group */
    gid2 = H5Gcreate2(gid1, MISC13_GROUP2_NAME, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
    CHECK(gid2, FAIL, "H5Gcreate2");

    /* Close the second group */
    ret = H5Gclose(gid2);
    CHECK(ret, FAIL, "H5Gclose");

    /* Create contiguous dataset in new group */
    misc13_create_dataset(gid1, MISC13_DSET1_NAME, H5P_DEFAULT, data);

    /* Create chunked dataset in new group */
    misc13_create_dataset(gid1, MISC13_DSET2_NAME, dcplid, data);

    /* Create a datatype to commit to the new group */
    tid = H5Tcopy(H5T_NATIVE_INT);
    CHECK(tid, FAIL, "H5Tcopy");

    /* Create a named datatype in the new group */
    ret = H5Tcommit2(gid1, MISC13_DTYPE_NAME, tid, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
    CHECK(ret, FAIL, "H5Tcommit2");

    /* Close named datatype */
    ret = H5Tclose(tid);
    CHECK(ret, FAIL, "H5Tclose");

    /* Close the first group */
    ret = H5Gclose(gid1);
    CHECK(ret, FAIL, "H5Gclose");

    /* Close the DCPL */
    ret = H5Pclose(dcplid);
    CHECK(ret, FAIL, "H5Pclose");

    /* Close the file */
    ret = H5Fclose(fid);
    CHECK(ret, FAIL, "H5Fclose");

} /* end misc13_create_hdf_file() */

static void
misc13_insert_user_block(const char *old_name, const char *new_name, const char *str, size_t size)
{
    FILE  *new_fp     = NULL; /* Pointers to new & old files */
    FILE  *old_fp     = NULL;
    void  *user_block = NULL; /* Pointer to user block to write to file */
    void  *copy_buf   = NULL; /* Pointer to buffer for copying data */
    size_t written;           /* Amount of data written to new file */
    size_t read_in;           /* Amount of data read in from old file */
    int    ret;               /* Generic status value */

    /* Allocate space for the user block */
    user_block = calloc(size, (size_t)1);
    CHECK_PTR(user_block, "calloc");

    /* Copy in the user block data */
    memcpy(user_block, str, strlen(str));

    /* Open the new file */
    new_fp = fopen(new_name, "wb");
    CHECK_PTR(new_fp, "fopen");

    /* Write the user block to the new file */
    written = fwrite(user_block, (size_t)1, size, new_fp);
    VERIFY(written, size, "fwrite");

    /* Open the old file */
    old_fp = fopen(old_name, "rb");
    CHECK_PTR(old_fp, "fopen");

    /* Allocate space for the copy buffer */
    copy_buf = malloc((size_t)MISC13_COPY_BUF_SIZE);
    CHECK_PTR(copy_buf, "malloc");

    /* Copy data from the old file to the new file */
    while ((read_in = fread(copy_buf, (size_t)1, (size_t)MISC13_COPY_BUF_SIZE, old_fp)) > 0) {
        /* Write the data to the new file */
        written = fwrite(copy_buf, (size_t)1, read_in, new_fp);
        VERIFY(written, read_in, "fwrite");
    }

    /* Close the old file */
    ret = fclose(old_fp);
    VERIFY(ret, 0, "fclose");

    /* Close the new file */
    ret = fclose(new_fp);
    VERIFY(ret, 0, "fclose");

    /* Free the copy buffer */
    free(copy_buf);

    /* Free the user block */
    free(user_block);

} /* end misc13_insert_user_block() */

static void
misc13_verify_file(const char *name, const unsigned *data, hsize_t userblock_size, bool check_for_new_dataset)
{
    hid_t   fid    = H5I_INVALID_HID; /* File ID */
    hid_t   gid1   = H5I_INVALID_HID; /* Group IDs */
    hid_t   gid2   = H5I_INVALID_HID; /* Group IDs */
    hid_t   tid    = H5I_INVALID_HID; /* Datatype ID */
    hid_t   fcplid = H5I_INVALID_HID; /* File creation property list ID */
    hsize_t ub_size_out;              /* Userblock size retrieved from FCPL */
    herr_t  ret;                      /* Generic return value */

    /* Open the file */
    fid = H5Fopen(name, H5F_ACC_RDONLY, H5P_DEFAULT);
    CHECK(fid, FAIL, "H5Fopen");

    /* Get the file's FCPL */
    fcplid = H5Fget_create_plist(fid);
    CHECK(fcplid, FAIL, "H5Fget_create_plist");

    /* Get the user block size for the file */
    ret = H5Pget_userblock(fcplid, &ub_size_out);
    CHECK(ret, FAIL, "H5Pget_userblock");

    /* Check the userblock size */
    VERIFY(userblock_size, ub_size_out, "H5Pget_userblock");

    /* Close the FCPL */
    ret = H5Pclose(fcplid);
    CHECK(ret, FAIL, "H5Pclose");

    /* Verify the contiguous dataset in the root group */
    misc13_verify_dataset(fid, MISC13_DSET1_NAME, data);

    /* Verify the chunked dataset in the root group */
    misc13_verify_dataset(fid, MISC13_DSET2_NAME, data);

    /* Verify the "new" contiguous dataset in the root group, if asked */
    if (check_for_new_dataset)
        misc13_verify_dataset(fid, MISC13_DSET3_NAME, data);

    /* Open the named datatype in the root group */
    tid = H5Topen2(fid, MISC13_DTYPE_NAME, H5P_DEFAULT);
    CHECK(tid, FAIL, "H5Topen2");

    /* Verify the type is correct */
    VERIFY(H5Tequal(tid, H5T_NATIVE_INT), true, "H5Tequal");

    /* Close named datatype */
    ret = H5Tclose(tid);
    CHECK(ret, FAIL, "H5Tclose");

    /* Open the first group */
    gid1 = H5Gopen2(fid, MISC13_GROUP1_NAME, H5P_DEFAULT);
    CHECK(gid1, FAIL, "H5Gopen2");

    /* Verify the contiguous dataset in the first group */
    misc13_verify_dataset(gid1, MISC13_DSET1_NAME, data);

    /* Verify the chunked dataset in the first group */
    misc13_verify_dataset(gid1, MISC13_DSET2_NAME, data);

    /* Open the named datatype in the first group */
    tid = H5Topen2(gid1, MISC13_DTYPE_NAME, H5P_DEFAULT);
    CHECK(tid, FAIL, "H5Topen2");

    /* Verify the type is correct */
    VERIFY(H5Tequal(tid, H5T_NATIVE_INT), true, "H5Tequal");

    /* Close named datatype */
    ret = H5Tclose(tid);
    CHECK(ret, FAIL, "H5Tclose");

    /* Open the second group */
    gid2 = H5Gopen2(gid1, MISC13_GROUP2_NAME, H5P_DEFAULT);
    CHECK(gid2, FAIL, "H5Gopen2");

    /* Close the second group */
    ret = H5Gclose(gid2);
    CHECK(ret, FAIL, "H5Gclose");

    /* Close the first group */
    ret = H5Gclose(gid1);
    CHECK(ret, FAIL, "H5Gclose");

    /* Close the file */
    ret = H5Fclose(fid);
    CHECK(ret, FAIL, "H5Fclose");

} /* end misc13_verify_file() */

static void
misc13_add_to_new_file(const char *name, const unsigned *data)
{
    hid_t  fid = H5I_INVALID_HID; /* File ID */
    herr_t ret;                   /* Generic return value */

    /* Open the file */
    fid = H5Fopen(name, H5F_ACC_RDWR, H5P_DEFAULT);
    CHECK(fid, FAIL, "H5Fopen");

    /* Create new contiguous dataset in root group */
    misc13_create_dataset(fid, MISC13_DSET3_NAME, H5P_DEFAULT, data);

    /* Close the file */
    ret = H5Fclose(fid);
    CHECK(ret, FAIL, "H5Fclose");

} /* end misc13_add_to_new_file() */

/****************************************************************
**
**  test_misc13(): Test that file contents can be "slid down" by
**      inserting a user block in front of an existing file.
**
****************************************************************/
static void
test_misc13(void)
{
    unsigned *data = NULL;           /* Data to write to dataset */
    hsize_t   userblock_size;        /* Correct size of userblock */
    bool      check_for_new_dataset; /* Whether to check for the post-userblock-creation dataset */
    bool      vol_is_native;

    /* Check if native VOL is being used */
    CHECK(h5_using_native_vol(H5P_DEFAULT, H5I_INVALID_HID, &vol_is_native), FAIL, "h5_using_native_vol");
    if (!vol_is_native)
        return;

    /* Create a data buffer for the datasets */
    data = (unsigned *)calloc(MISC13_DIM1, sizeof(unsigned));
    CHECK_PTR(data, "calloc");

    /* Initialize data to write */
    misc13_init_data(data);

    /* Create first file, with no user block */
    misc13_create_hdf_file(MISC13_FILE_1, data);

    /* Verify file contents are correct */
    userblock_size        = 0;
    check_for_new_dataset = false;
    misc13_verify_file(MISC13_FILE_1, data, userblock_size, check_for_new_dataset);

    /* Create a new file by inserting a user block in front of the first file */
    misc13_insert_user_block(MISC13_FILE_1, MISC13_FILE_2, "Test String", (size_t)MISC13_USERBLOCK_SIZE);

    /* Verify file contents are still correct */
    userblock_size        = MISC13_USERBLOCK_SIZE;
    check_for_new_dataset = false;
    misc13_verify_file(MISC13_FILE_2, data, userblock_size, check_for_new_dataset);

    /* Make certain we can modify the new file */
    misc13_add_to_new_file(MISC13_FILE_2, data);

    /* Verify file contents are still correct */
    userblock_size        = MISC13_USERBLOCK_SIZE;
    check_for_new_dataset = true;
    misc13_verify_file(MISC13_FILE_2, data, userblock_size, check_for_new_dataset);

    /* Free the dataset buffer */
    free(data);

} /* end test_misc13() */

/****************************************************************
**
**  test_misc14(): Test that file contents can be "slid down" by
**      inserting a user block in front of an existing file.
**
****************************************************************/
static void
test_misc14(void)
{
    hid_t  file_id;      /* File ID */
    hid_t  fapl;         /* File access property list ID */
    hid_t  DataSpace;    /* Dataspace ID */
    hid_t  Dataset1;     /* Dataset ID #1 */
    hid_t  Dataset2;     /* Dataset ID #2 */
    hid_t  Dataset3;     /* Dataset ID #3 */
    double data1 = 5.0;  /* Data to write for dataset #1 */
    double data2 = 10.0; /* Data to write for dataset #2 */
    double data3 = 15.0; /* Data to write for dataset #3 */
    double rdata;        /* Data read in */
    herr_t ret;          /* Generic return value */

    /* Test creating two datasets and deleting the second */

    /* Increase the metadata block size */
    /* (This makes certain that all the data blocks are allocated together) */
    fapl = H5Pcreate(H5P_FILE_ACCESS);
    CHECK(fapl, FAIL, "H5Pcreate");

    ret = H5Pset_meta_block_size(fapl, (hsize_t)MISC14_METADATA_SIZE);
    CHECK(ret, FAIL, "H5Pset_meta_block_size");

    /* Create dataspace to use */
    DataSpace = H5Screate(H5S_SCALAR);
    CHECK(DataSpace, FAIL, "H5Screate");

    /* Open the file */
    file_id = H5Fcreate(MISC14_FILE, H5F_ACC_TRUNC, H5P_DEFAULT, fapl);
    CHECK(file_id, FAIL, "H5Fcreate");

    /* Create first dataset & write data */
    Dataset1 = H5Dcreate2(file_id, MISC14_DSET1_NAME, H5T_NATIVE_DOUBLE, DataSpace, H5P_DEFAULT, H5P_DEFAULT,
                          H5P_DEFAULT);
    CHECK(Dataset1, FAIL, "H5Dcreate2");

    ret = H5Dwrite(Dataset1, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, H5P_DEFAULT, &data1);
    CHECK(ret, FAIL, "H5Dwrite");

    /* Create second dataset (to be unlinked).  */
    Dataset2 = H5Dcreate2(file_id, MISC14_DSET2_NAME, H5T_NATIVE_DOUBLE, DataSpace, H5P_DEFAULT, H5P_DEFAULT,
                          H5P_DEFAULT);
    CHECK(Dataset2, FAIL, "H5Dcreate2");

    ret = H5Dwrite(Dataset2, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, H5P_DEFAULT, &data2);
    CHECK(ret, FAIL, "H5Dwrite");

    /* Check data from first dataset */
    ret = H5Dread(Dataset1, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, H5P_DEFAULT, &rdata);
    CHECK(ret, FAIL, "H5Dread");
    if (!H5_DBL_ABS_EQUAL(rdata, data1))
        TestErrPrintf("Error on line %d: data1!=rdata\n", __LINE__);

    /* Unlink second dataset */
    ret = H5Ldelete(file_id, MISC14_DSET2_NAME, H5P_DEFAULT);
    CHECK(ret, FAIL, "H5Ldelete");

    /* Close second dataset */
    ret = H5Dclose(Dataset2);
    CHECK(ret, FAIL, "H5Dclose");

    /* Verify the data from dataset #1 */
    ret = H5Dread(Dataset1, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, H5P_DEFAULT, &rdata);
    CHECK(ret, FAIL, "H5Dread");
    if (!H5_DBL_ABS_EQUAL(rdata, data1))
        TestErrPrintf("Error on line %d: data1!=rdata\n", __LINE__);

    /* Close first dataset */
    ret = H5Dclose(Dataset1);
    CHECK(ret, FAIL, "H5Dclose");

    /* Close the file */
    ret = H5Fclose(file_id);
    CHECK(ret, FAIL, "H5Fclose");

    /* Test creating two datasets and deleting the first */

    /* Open the file */
    file_id = H5Fcreate(MISC14_FILE, H5F_ACC_TRUNC, H5P_DEFAULT, fapl);
    CHECK(file_id, FAIL, "H5Fcreate");

    /* Create first dataset & write data */
    Dataset1 = H5Dcreate2(file_id, MISC14_DSET1_NAME, H5T_NATIVE_DOUBLE, DataSpace, H5P_DEFAULT, H5P_DEFAULT,
                          H5P_DEFAULT);
    CHECK(Dataset1, FAIL, "H5Dcreate2");

    ret = H5Dwrite(Dataset1, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, H5P_DEFAULT, &data1);
    CHECK(ret, FAIL, "H5Dwrite");

    /* Create second dataset  */
    Dataset2 = H5Dcreate2(file_id, MISC14_DSET2_NAME, H5T_NATIVE_DOUBLE, DataSpace, H5P_DEFAULT, H5P_DEFAULT,
                          H5P_DEFAULT);
    CHECK(Dataset2, FAIL, "H5Dcreate2");

    ret = H5Dwrite(Dataset2, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, H5P_DEFAULT, &data2);
    CHECK(ret, FAIL, "H5Dwrite");

    /* Check data from second dataset */
    ret = H5Dread(Dataset2, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, H5P_DEFAULT, &rdata);
    CHECK(ret, FAIL, "H5Dread");
    if (!H5_DBL_ABS_EQUAL(rdata, data2))
        TestErrPrintf("Error on line %d: data2!=rdata\n", __LINE__);

    /* Unlink first dataset */
    ret = H5Ldelete(file_id, MISC14_DSET1_NAME, H5P_DEFAULT);
    CHECK(ret, FAIL, "H5Ldelete");

    /* Close first dataset */
    ret = H5Dclose(Dataset1);
    CHECK(ret, FAIL, "H5Dclose");

    /* Verify the data from dataset #2 */
    ret = H5Dread(Dataset2, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, H5P_DEFAULT, &rdata);
    CHECK(ret, FAIL, "H5Dread");
    if (!H5_DBL_ABS_EQUAL(rdata, data2))
        TestErrPrintf("Error on line %d: data2!=rdata\n", __LINE__);

    /* Close second dataset */
    ret = H5Dclose(Dataset2);
    CHECK(ret, FAIL, "H5Dclose");

    /* Close the file */
    ret = H5Fclose(file_id);
    CHECK(ret, FAIL, "H5Fclose");

    /* Test creating three datasets and deleting the second */

    /* Open the file */
    file_id = H5Fcreate(MISC14_FILE, H5F_ACC_TRUNC, H5P_DEFAULT, fapl);
    CHECK(file_id, FAIL, "H5Fcreate");

    /* Create first dataset & write data */
    Dataset1 = H5Dcreate2(file_id, MISC14_DSET1_NAME, H5T_NATIVE_DOUBLE, DataSpace, H5P_DEFAULT, H5P_DEFAULT,
                          H5P_DEFAULT);
    CHECK(Dataset1, FAIL, "H5Dcreate2");

    ret = H5Dwrite(Dataset1, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, H5P_DEFAULT, &data1);
    CHECK(ret, FAIL, "H5Dwrite");

    /* Create second dataset */
    Dataset2 = H5Dcreate2(file_id, MISC14_DSET2_NAME, H5T_NATIVE_DOUBLE, DataSpace, H5P_DEFAULT, H5P_DEFAULT,
                          H5P_DEFAULT);
    CHECK(Dataset2, FAIL, "H5Dcreate2");

    ret = H5Dwrite(Dataset2, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, H5P_DEFAULT, &data2);
    CHECK(ret, FAIL, "H5Dwrite");

    /* Create third dataset */
    Dataset3 = H5Dcreate2(file_id, MISC14_DSET3_NAME, H5T_NATIVE_DOUBLE, DataSpace, H5P_DEFAULT, H5P_DEFAULT,
                          H5P_DEFAULT);
    CHECK(Dataset2, FAIL, "H5Dcreate2");

    ret = H5Dwrite(Dataset3, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, H5P_DEFAULT, &data3);
    CHECK(ret, FAIL, "H5Dwrite");

    /* Check data from first dataset */
    ret = H5Dread(Dataset1, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, H5P_DEFAULT, &rdata);
    CHECK(ret, FAIL, "H5Dread");
    if (!H5_DBL_ABS_EQUAL(rdata, data1))
        TestErrPrintf("Error on line %d: data1!=rdata\n", __LINE__);

    /* Check data from third dataset */
    ret = H5Dread(Dataset3, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, H5P_DEFAULT, &rdata);
    CHECK(ret, FAIL, "H5Dread");
    if (!H5_DBL_ABS_EQUAL(rdata, data3))
        TestErrPrintf("Error on line %d: data3!=rdata\n", __LINE__);

    /* Unlink second dataset */
    ret = H5Ldelete(file_id, MISC14_DSET2_NAME, H5P_DEFAULT);
    CHECK(ret, FAIL, "H5Ldelete");

    /* Close second dataset */
    ret = H5Dclose(Dataset2);
    CHECK(ret, FAIL, "H5Dclose");

    /* Verify the data from dataset #1 */
    ret = H5Dread(Dataset1, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, H5P_DEFAULT, &rdata);
    CHECK(ret, FAIL, "H5Dread");
    if (!H5_DBL_ABS_EQUAL(rdata, data1))
        TestErrPrintf("Error on line %d: data1!=rdata\n", __LINE__);

    /* Verify the data from dataset #3 */
    ret = H5Dread(Dataset3, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, H5P_DEFAULT, &rdata);
    CHECK(ret, FAIL, "H5Dread");
    if (!H5_DBL_ABS_EQUAL(rdata, data3))
        TestErrPrintf("Error on line %d: data3!=rdata\n", __LINE__);

    /* Close first dataset */
    ret = H5Dclose(Dataset1);
    CHECK(ret, FAIL, "H5Dclose");

    /* Close third dataset */
    ret = H5Dclose(Dataset3);
    CHECK(ret, FAIL, "H5Dclose");

    /* Close the file */
    ret = H5Fclose(file_id);
    CHECK(ret, FAIL, "H5Fclose");

    /* Close shared objects (dataspace & fapl) */
    ret = H5Sclose(DataSpace);
    CHECK(ret, FAIL, "H5Sclose");
    ret = H5Pclose(fapl);
    CHECK(ret, FAIL, "H5Pclose");

} /* end test_misc14() */

/****************************************************************
**
**  test_misc15(): Test that checking a file's access property list
**      more than once correctly increments internal reference counts.
**
****************************************************************/
static void
test_misc15(void)
{
    char   filename[MISC15_BUF_SIZE];
    hid_t  file; /* File ID */
    hid_t  fapl; /* File access property list */
    herr_t ret;  /* Generic return value */

    fapl = h5_fileaccess();
    h5_fixname(MISC15_FILE, fapl, filename, MISC15_BUF_SIZE);

    /* Create the file & get it's FAPL */
    file = H5Fcreate(filename, H5F_ACC_TRUNC, H5P_DEFAULT, fapl);
    CHECK(file, FAIL, "H5Fcreate");

    ret = H5Pclose(fapl);
    CHECK(ret, FAIL, "H5Pclose");

    fapl = H5Fget_access_plist(file);
    CHECK(fapl, FAIL, "H5Fget_access_plist");

    ret = H5Pclose(fapl);
    CHECK(ret, FAIL, "H5Pclose");

    ret = H5Fclose(file);
    CHECK(ret, FAIL, "H5Fclose");

    /* Open the file & get it's FAPL again */
    file = H5Fopen(filename, H5F_ACC_RDONLY, H5P_DEFAULT);
    CHECK(file, FAIL, "H5Fopen");

    fapl = H5Fget_access_plist(file);
    CHECK(fapl, FAIL, "H5Fget_access_plist");

    ret = H5Fclose(file);
    CHECK(ret, FAIL, "H5Fclose");

    /* Verify that the file is still OK */
    ret = H5Fis_accessible(filename, fapl);
    CHECK(ret, FAIL, "H5Fis_accessible");

    ret = H5Pclose(fapl);
    CHECK(ret, FAIL, "H5Pclose");

    file = H5Fopen(filename, H5F_ACC_RDONLY, H5P_DEFAULT);
    CHECK(file, FAIL, "H5Fopen");

    ret = H5Fclose(file);
    CHECK(ret, FAIL, "H5Fclose");
} /* end test_misc15() */

/****************************************************************
**
**  test_misc16(): Test array of NULL-terminated
**  fixed-length string.  It creates a dataset of fixed-length
**  strings.  Each string is MISC16_STR_SIZE long.  There are
**  totally MISC16_SPACE_DIM by MISC16_SPACE_RANK strings.
**
****************************************************************/
static void
test_misc16(void)
{
    hid_t   file; /* File ID */
    herr_t  ret;  /* Generic return value */
    char    wdata[MISC16_SPACE_DIM][MISC16_STR_SIZE];
    char    rdata[MISC16_SPACE_DIM][MISC16_STR_SIZE]; /* Information read in */
    hid_t   dataset;                                  /* Dataset ID            */
    hid_t   sid;                                      /* Dataspace ID            */
    hid_t   tid;                                      /* Datatype ID            */
    hsize_t dims[] = {MISC16_SPACE_DIM};
    int     i;

    memset(wdata, 0, sizeof(wdata));
    memset(rdata, 0, sizeof(rdata));

    /* Initialize the data */
    /* (Note that these are supposed to stress the code, so are a little weird) */
    memcpy(wdata[0], "1234567", MISC16_STR_SIZE);
    memcpy(wdata[1], "1234567\0", MISC16_STR_SIZE);
    memcpy(wdata[2], "12345678", MISC16_STR_SIZE);
    memcpy(wdata[3], "\0\0\0\0\0\0\0\0", MISC16_STR_SIZE);

    /* Create the file */
    file = H5Fcreate(MISC16_FILE, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
    CHECK(file, FAIL, "H5Fcreate");

    /* Create dataspace for datasets */
    sid = H5Screate_simple(MISC16_SPACE_RANK, dims, NULL);
    CHECK(sid, FAIL, "H5Screate_simple");

    /* Create a datatype to refer to */
    tid = H5Tcopy(H5T_C_S1);
    CHECK(tid, FAIL, "H5Tcopy");

    ret = H5Tset_size(tid, (size_t)MISC16_STR_SIZE);
    CHECK(ret, FAIL, "H5Tset_size");

    /*ret = H5Tset_strpad(tid,H5T_STR_NULLPAD);
    CHECK(ret, FAIL, "H5Tset_strpad");*/

    /* Create a dataset */
    dataset = H5Dcreate2(file, MISC16_DSET_NAME, tid, sid, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
    CHECK(dataset, FAIL, "H5Dcreate2");

    /* Write dataset to disk */
    ret = H5Dwrite(dataset, tid, H5S_ALL, H5S_ALL, H5P_DEFAULT, wdata);
    CHECK(ret, FAIL, "H5Dwrite");

    /* Read dataset from disk */
    ret = H5Dread(dataset, tid, H5S_ALL, H5S_ALL, H5P_DEFAULT, rdata);
    CHECK(ret, FAIL, "H5Dread");

    /* Compare data read in */
    for (i = 0; i < MISC16_SPACE_DIM; i++) {
        if (strlen(wdata[i]) != strlen(rdata[i])) {
            TestErrPrintf(
                "Line %u: VL data length don't match!, strlen(wdata[%d])=%d, strlen(rdata[%d])=%d\n",
                (unsigned)__LINE__, (int)i, (int)strlen(wdata[i]), (int)i, (int)strlen(rdata[i]));
            continue;
        } /* end if */
        if (strcmp(wdata[i], rdata[i]) != 0) {
            TestErrPrintf("Line %u: VL data values don't match!, wdata[%d]=%s, rdata[%d]=%s\n",
                          (unsigned)__LINE__, (int)i, wdata[i], (int)i, rdata[i]);
            continue;
        } /* end if */
    }     /* end for */

    /* Close Dataset */
    ret = H5Dclose(dataset);
    CHECK(ret, FAIL, "H5Dclose");

    /* Close datatype */
    ret = H5Tclose(tid);
    CHECK(ret, FAIL, "H5Tclose");

    /* Close disk dataspace */
    ret = H5Sclose(sid);
    CHECK(ret, FAIL, "H5Sclose");

    ret = H5Fclose(file);
    CHECK(ret, FAIL, "H5Fclose");
} /* end test_misc16() */

/****************************************************************
**
**  test_misc17(): Test array of characters.  It creates a dataset
**  of ASCII characters, with dimensionality of MISC17_SPACE_DIM1
**  by MISC17_SPACE_DIM2.
**
****************************************************************/
static void
test_misc17(void)
{
    hid_t   file; /* File ID */
    herr_t  ret;  /* Generic return value */
    char    wdata[MISC17_SPACE_DIM1][MISC17_SPACE_DIM2];
    char    rdata[MISC17_SPACE_DIM1][MISC17_SPACE_DIM2]; /* Information read in */
    hid_t   dataset;                                     /* Dataset ID            */
    hid_t   sid;                                         /* Dataspace ID            */
    hid_t   tid;                                         /* Datatype ID            */
    hsize_t dims[] = {MISC17_SPACE_DIM1, MISC17_SPACE_DIM2};
    int     i;

    memset(wdata, 0, sizeof(wdata));
    memset(rdata, 0, sizeof(rdata));

    /* Initialize the data */
    /* (Note that these are supposed to stress the code, so are a little weird) */
    memcpy(wdata[0], "1234567", MISC17_SPACE_DIM2);
    memcpy(wdata[1], "1234567\0", MISC17_SPACE_DIM2);
    memcpy(wdata[2], "12345678", MISC17_SPACE_DIM2);
    memcpy(wdata[3], "\0\0\0\0\0\0\0\0", MISC17_SPACE_DIM2);

    /* Create the file */
    file = H5Fcreate(MISC17_FILE, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
    CHECK(file, FAIL, "H5Fcreate");

    /* Create dataspace for datasets */
    sid = H5Screate_simple(MISC17_SPACE_RANK, dims, NULL);
    CHECK(sid, FAIL, "H5Screate_simple");

    /* Create a datatype to refer to */
    tid = H5Tcopy(H5T_C_S1);
    CHECK(tid, FAIL, "H5Tcopy");

    ret = H5Tset_strpad(tid, H5T_STR_NULLPAD);
    CHECK(ret, FAIL, "H5Tset_strpad");

    /* Create a dataset */
    dataset = H5Dcreate2(file, MISC17_DSET_NAME, tid, sid, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
    CHECK(dataset, FAIL, "H5Dcreate2");

    /* Write dataset to disk */
    ret = H5Dwrite(dataset, tid, H5S_ALL, H5S_ALL, H5P_DEFAULT, wdata);
    CHECK(ret, FAIL, "H5Dwrite");

    /* Read dataset from disk */
    ret = H5Dread(dataset, tid, H5S_ALL, H5S_ALL, H5P_DEFAULT, rdata);
    CHECK(ret, FAIL, "H5Dread");

    /* Compare data in the way of strings. */
    for (i = 0; i < MISC17_SPACE_DIM1; i++) {
        if (strlen(wdata[i]) != strlen(rdata[i])) {
            TestErrPrintf(
                "Line %u: VL data length don't match!, strlen(wdata[%d])=%d, strlen(rdata[%d])=%d\n",
                (unsigned)__LINE__, (int)i, (int)strlen(wdata[i]), (int)i, (int)strlen(rdata[i]));
            continue;
        } /* end if */
        if (strcmp(wdata[i], rdata[i]) != 0) {
            TestErrPrintf("Line %u: VL data values don't match!, wdata[%d]=%s, rdata[%d]=%s\n",
                          (unsigned)__LINE__, (int)i, wdata[i], (int)i, rdata[i]);
            continue;
        } /* end if */
    }     /* end for */

    /* Close Dataset */
    ret = H5Dclose(dataset);
    CHECK(ret, FAIL, "H5Dclose");

    /* Close datatype */
    ret = H5Tclose(tid);
    CHECK(ret, FAIL, "H5Tclose");

    /* Close disk dataspace */
    ret = H5Sclose(sid);
    CHECK(ret, FAIL, "H5Sclose");

    ret = H5Fclose(file);
    CHECK(ret, FAIL, "H5Fclose");
} /* end test_misc17() */

/****************************************************************
**
**  test_misc18(): Test new object header information in H5O_info_t
**  struct.
**
****************************************************************/
static void
test_misc18(void)
{
    hid_t fid;        /* File ID */
    hid_t sid;        /* 'Space ID */
    hid_t did1, did2; /* Dataset IDs */
    hid_t aid;        /* Attribute ID */
#ifndef H5_NO_DEPRECATED_SYMBOLS
    H5O_info1_t old_oinfo;           /* (deprecated) information about object */
#endif                               /* H5_NO_DEPRECATED_SYMBOLS */
    H5O_info2_t       oinfo;         /* Data model information about object */
    H5O_native_info_t ninfo;         /* Native file format information about object */
    char              attr_name[32]; /* Attribute name buffer */
    unsigned          u;             /* Local index variable */
    bool              vol_is_native;
    herr_t            ret; /* Generic return value */

    /* Create the file */
    fid = H5Fcreate(MISC18_FILE, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
    CHECK(fid, FAIL, "H5Fcreate");

    /* Check if native VOL is being used */
    CHECK(h5_using_native_vol(H5P_DEFAULT, fid, &vol_is_native), FAIL, "h5_using_native_vol");

    /* Create dataspace for attributes */
    sid = H5Screate(H5S_SCALAR);
    CHECK(sid, FAIL, "H5Screate");

    /* Create first dataset */
    did1 = H5Dcreate2(fid, MISC18_DSET1_NAME, H5T_STD_U32LE, sid, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
    CHECK(did1, FAIL, "H5Dcreate2");

    /* Get object information */
    ret = H5Oget_info_by_name3(fid, MISC18_DSET1_NAME, &oinfo, H5O_INFO_NUM_ATTRS, H5P_DEFAULT);
    CHECK(ret, FAIL, "H5Oget_info_by_name");
    VERIFY(oinfo.num_attrs, 0, "H5Oget_info_by_name");

    if (vol_is_native) {
#ifndef H5_NO_DEPRECATED_SYMBOLS
        ret = H5Oget_info_by_name2(fid, MISC18_DSET1_NAME, &old_oinfo, H5O_INFO_HDR | H5O_INFO_NUM_ATTRS,
                                   H5P_DEFAULT);
        CHECK(ret, FAIL, "H5Oget_info_by_name");
        VERIFY(old_oinfo.hdr.nmesgs, 6, "H5Oget_info_by_name");
        VERIFY(old_oinfo.hdr.nchunks, 1, "H5Oget_info_by_name");
        VERIFY(old_oinfo.hdr.space.total, 272, "H5Oget_info_by_name");
        VERIFY(old_oinfo.hdr.space.free, 152, "H5Oget_info_by_name");
        VERIFY(old_oinfo.num_attrs, 0, "H5Oget_info_by_name");
#endif /* H5_NO_DEPRECATED_SYMBOLS */
        ret = H5Oget_native_info_by_name(fid, MISC18_DSET1_NAME, &ninfo, H5O_NATIVE_INFO_HDR, H5P_DEFAULT);
        CHECK(ret, FAIL, "H5Oget_native_info_by_name");
        VERIFY(ninfo.hdr.nmesgs, 6, "H5Oget_native_info_by_name");
        VERIFY(ninfo.hdr.nchunks, 1, "H5Oget_native_info_by_name");
        VERIFY(ninfo.hdr.space.total, 272, "H5Oget_native_info_by_name");
        VERIFY(ninfo.hdr.space.free, 152, "H5Oget_native_info_by_name");
    }

    /* Create second dataset */
    did2 = H5Dcreate2(fid, MISC18_DSET2_NAME, H5T_STD_U32LE, sid, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
    CHECK(did2, FAIL, "H5Dcreate2");

    /* Get object information */
    ret = H5Oget_info_by_name3(fid, MISC18_DSET2_NAME, &oinfo, H5O_INFO_NUM_ATTRS, H5P_DEFAULT);
    CHECK(ret, FAIL, "H5Oget_info_by_name");
    VERIFY(oinfo.num_attrs, 0, "H5Oget_info_by_name");

    if (vol_is_native) {
#ifndef H5_NO_DEPRECATED_SYMBOLS
        ret = H5Oget_info_by_name2(fid, MISC18_DSET2_NAME, &old_oinfo, H5O_INFO_HDR | H5O_INFO_NUM_ATTRS,
                                   H5P_DEFAULT);
        CHECK(ret, FAIL, "H5Oget_info_by_name");
        VERIFY(old_oinfo.hdr.nmesgs, 6, "H5Oget_info_by_name");
        VERIFY(old_oinfo.hdr.nchunks, 1, "H5Oget_info_by_name");
        VERIFY(old_oinfo.hdr.space.total, 272, "H5Oget_info_by_name");
        VERIFY(old_oinfo.hdr.space.free, 152, "H5Oget_info_by_name");
        VERIFY(old_oinfo.num_attrs, 0, "H5Oget_info_by_name");
#endif /* H5_NO_DEPRECATED_SYMBOLS */
        ret = H5Oget_native_info_by_name(fid, MISC18_DSET2_NAME, &ninfo, H5O_NATIVE_INFO_HDR, H5P_DEFAULT);
        CHECK(ret, FAIL, "H5Oget_native_info_by_name");
        VERIFY(ninfo.hdr.nmesgs, 6, "H5Oget_native_info_by_name");
        VERIFY(ninfo.hdr.nchunks, 1, "H5Oget_native_info_by_name");
        VERIFY(ninfo.hdr.space.total, 272, "H5Oget_native_info_by_name");
        VERIFY(ninfo.hdr.space.free, 152, "H5Oget_native_info_by_name");
    }

    /* Loop creating attributes on each dataset, flushing them to the file each time */
    for (u = 0; u < 10; u++) {
        /* Set up attribute name */
        snprintf(attr_name, sizeof(attr_name), "Attr %u", u);

        /* Create & close attribute on first dataset */
        aid = H5Acreate2(did1, attr_name, H5T_STD_U32LE, sid, H5P_DEFAULT, H5P_DEFAULT);
        CHECK(aid, FAIL, "H5Acreate2");

        ret = H5Aclose(aid);
        CHECK(ret, FAIL, "H5Aclose");

        /* Create & close attribute on second dataset */
        aid = H5Acreate2(did2, attr_name, H5T_STD_U32LE, sid, H5P_DEFAULT, H5P_DEFAULT);
        CHECK(aid, FAIL, "H5Acreate2");

        ret = H5Aclose(aid);
        CHECK(ret, FAIL, "H5Aclose");

        /* Flush file, to 'fix' size of dataset object headers */
        ret = H5Fflush(fid, H5F_SCOPE_GLOBAL);
        CHECK(ret, FAIL, "H5Fflush");
    } /* end for */

    /* Get object information for dataset #1 now */
    ret = H5Oget_info_by_name3(fid, MISC18_DSET1_NAME, &oinfo, H5O_INFO_NUM_ATTRS, H5P_DEFAULT);
    CHECK(ret, FAIL, "H5Oget_info_by_name");
    VERIFY(oinfo.num_attrs, 10, "H5Oget_info_by_name");

    if (vol_is_native) {
#ifndef H5_NO_DEPRECATED_SYMBOLS
        ret = H5Oget_info_by_name2(fid, MISC18_DSET1_NAME, &old_oinfo, H5O_INFO_HDR | H5O_INFO_NUM_ATTRS,
                                   H5P_DEFAULT);
        CHECK(ret, FAIL, "H5Oget_info_by_name");
        VERIFY(old_oinfo.hdr.nmesgs, 24, "H5Oget_info_by_name");
        VERIFY(old_oinfo.hdr.nchunks, 9, "H5Oget_info_by_name");
        VERIFY(old_oinfo.hdr.space.total, 888, "H5Oget_info_by_name");
        VERIFY(old_oinfo.hdr.space.free, 16, "H5Oget_info_by_name");
        VERIFY(old_oinfo.num_attrs, 10, "H5Oget_info_by_name");
#endif /* H5_NO_DEPRECATED_SYMBOLS */
        ret = H5Oget_native_info_by_name(fid, MISC18_DSET1_NAME, &ninfo, H5O_NATIVE_INFO_HDR, H5P_DEFAULT);
        CHECK(ret, FAIL, "H5Oget_native_info_by_name");
        VERIFY(ninfo.hdr.nmesgs, 24, "H5Oget_native_info_by_name");
        VERIFY(ninfo.hdr.nchunks, 9, "H5Oget_native_info_by_name");
        VERIFY(ninfo.hdr.space.total, 888, "H5Oget_native_info_by_name");
        VERIFY(ninfo.hdr.space.free, 16, "H5Oget_native_info_by_name");
    }

    /* Get object information for dataset #2 now */
    ret = H5Oget_info_by_name3(fid, MISC18_DSET2_NAME, &oinfo, H5O_INFO_NUM_ATTRS, H5P_DEFAULT);
    CHECK(ret, FAIL, "H5Oget_info_by_name");
    VERIFY(oinfo.num_attrs, 10, "H5Oget_info_by_name");

    if (vol_is_native) {
#ifndef H5_NO_DEPRECATED_SYMBOLS
        ret = H5Oget_info_by_name2(fid, MISC18_DSET2_NAME, &old_oinfo, H5O_INFO_HDR | H5O_INFO_NUM_ATTRS,
                                   H5P_DEFAULT);
        CHECK(ret, FAIL, "H5Oget_info_by_name");
        VERIFY(old_oinfo.hdr.nmesgs, 24, "H5Oget_info_by_name");
        VERIFY(old_oinfo.hdr.nchunks, 9, "H5Oget_info_by_name");
        VERIFY(old_oinfo.hdr.space.total, 888, "H5Oget_info_by_name");
        VERIFY(old_oinfo.hdr.space.free, 16, "H5Oget_info_by_name");
        VERIFY(old_oinfo.num_attrs, 10, "H5Oget_info_by_name");
#endif /* H5_NO_DEPRECATED_SYMBOLS */
        ret = H5Oget_native_info_by_name(fid, MISC18_DSET2_NAME, &ninfo, H5O_NATIVE_INFO_HDR, H5P_DEFAULT);
        CHECK(ret, FAIL, "H5Oget_mative_info_by_name");
        VERIFY(ninfo.hdr.nmesgs, 24, "H5Oget_native_info_by_name");
        VERIFY(ninfo.hdr.nchunks, 9, "H5Oget_native_info_by_name");
        VERIFY(ninfo.hdr.space.total, 888, "H5Oget_native_info_by_name");
        VERIFY(ninfo.hdr.space.free, 16, "H5Oget_native_info_by_name");
    }

    /* Close second dataset */
    ret = H5Dclose(did2);
    CHECK(ret, FAIL, "H5Sclose");

    /* Close first dataset */
    ret = H5Dclose(did1);
    CHECK(ret, FAIL, "H5Sclose");

    /* Close disk dataspace */
    ret = H5Sclose(sid);
    CHECK(ret, FAIL, "H5Sclose");

    ret = H5Fclose(fid);
    CHECK(ret, FAIL, "H5Fclose");
} /* end test_misc18() */

/****************************************************************
**
**  test_misc19(): Test incrementing & decrementing ref count on IDs
**
****************************************************************/
static void
test_misc19(void)
{
    hid_t         fid     = H5I_INVALID_HID; /* File ID                  */
    hid_t         sid     = H5I_INVALID_HID; /* Dataspace ID             */
    hid_t         did     = H5I_INVALID_HID; /* Dataset ID               */
    hid_t         tid     = H5I_INVALID_HID; /* Datatype ID              */
    hid_t         aid     = H5I_INVALID_HID; /* Attribute ID             */
    hid_t         plid    = H5I_INVALID_HID; /* Property List ID         */
    hid_t         pcid    = H5I_INVALID_HID; /* Property Class ID        */
    hid_t         gid     = H5I_INVALID_HID; /* Group ID                 */
    hid_t         ecid    = H5I_INVALID_HID; /* Error Class ID           */
    hid_t         emid    = H5I_INVALID_HID; /* Error Message ID         */
    hid_t         esid    = H5I_INVALID_HID; /* Error Stack ID           */
    hid_t         vfdid   = H5I_INVALID_HID; /* Virtual File Driver ID   */
    hid_t         volid   = H5I_INVALID_HID; /* Virtual Object Layer ID  */
    H5FD_class_t *vfd_cls = NULL;            /* VFD class                */
    H5VL_class_t *vol_cls = NULL;            /* VOL class                */
    int           rc;                        /* Reference count          */
    herr_t        ret;                       /* Generic return value     */

    /* Check H5I operations on files */

    /* Create the file */
    fid = H5Fcreate(MISC19_FILE, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
    CHECK(fid, FAIL, "H5Fcreate");

    /* Check the reference count */
    rc = H5Iget_ref(fid);
    VERIFY(rc, 1, "H5Iget_ref");

    /* Inc the reference count */
    rc = H5Iinc_ref(fid);
    VERIFY(rc, 2, "H5Iinc_ref");

    /* Close the file normally */
    ret = H5Fclose(fid);
    CHECK(ret, FAIL, "H5Fclose");

    /* Check the reference count */
    rc = H5Iget_ref(fid);
    VERIFY(rc, 1, "H5Iget_ref");

    /* Close the file by decrementing the reference count */
    rc = H5Idec_ref(fid);
    VERIFY(rc, 0, "H5Idec_ref");

    /* Try closing the file again (should fail) */
    H5E_BEGIN_TRY
    {
        ret = H5Fclose(fid);
    }
    H5E_END_TRY
    VERIFY(ret, FAIL, "H5Fclose");

    /* Check H5I operations on property lists */

    /* Create the property list */
    plid = H5Pcreate(H5P_DATASET_CREATE);
    CHECK(plid, FAIL, "H5Pcreate");

    /* Check the reference count */
    rc = H5Iget_ref(plid);
    VERIFY(rc, 1, "H5Iget_ref");

    /* Inc the reference count */
    rc = H5Iinc_ref(plid);
    VERIFY(rc, 2, "H5Iinc_ref");

    /* Close the property list normally */
    ret = H5Pclose(plid);
    CHECK(ret, FAIL, "H5Pclose");

    /* Check the reference count */
    rc = H5Iget_ref(plid);
    VERIFY(rc, 1, "H5Iget_ref");

    /* Close the property list by decrementing the reference count */
    rc = H5Idec_ref(plid);
    VERIFY(rc, 0, "H5Idec_ref");

    /* Try closing the property list again (should fail) */
    H5E_BEGIN_TRY
    {
        ret = H5Pclose(plid);
    }
    H5E_END_TRY
    VERIFY(ret, FAIL, "H5Pclose");

    /* Check H5I operations on property classes */

    /* Create a property class */
    pcid = H5Pcreate_class(H5P_DATASET_CREATE, "foo", NULL, NULL, NULL, NULL, NULL, NULL);
    CHECK(pcid, FAIL, "H5Pcreate_class");

    /* Check the reference count */
    rc = H5Iget_ref(pcid);
    VERIFY(rc, 1, "H5Iget_ref");

    /* Inc the reference count */
    rc = H5Iinc_ref(pcid);
    VERIFY(rc, 2, "H5Iinc_ref");

    /* Close the property class normally */
    ret = H5Pclose_class(pcid);
    CHECK(ret, FAIL, "H5Pclose_class");

    /* Check the reference count */
    rc = H5Iget_ref(pcid);
    VERIFY(rc, 1, "H5Iget_ref");

    /* Close the property class by decrementing the reference count */
    rc = H5Idec_ref(pcid);
    VERIFY(rc, 0, "H5Idec_ref");

    /* Try closing the property class again (should fail) */
    H5E_BEGIN_TRY
    {
        ret = H5Pclose_class(pcid);
    }
    H5E_END_TRY
    VERIFY(ret, FAIL, "H5Pclose_class");

    /* Check H5I operations on datatypes */

    /* Create a datatype */
    tid = H5Tcreate(H5T_OPAQUE, (size_t)16);
    CHECK(tid, FAIL, "H5Tcreate");

    /* Check the reference count */
    rc = H5Iget_ref(tid);
    VERIFY(rc, 1, "H5Iget_ref");

    /* Inc the reference count */
    rc = H5Iinc_ref(tid);
    VERIFY(rc, 2, "H5Iinc_ref");

    /* Close the datatype normally */
    ret = H5Tclose(tid);
    CHECK(ret, FAIL, "H5Tclose");

    /* Check the reference count */
    rc = H5Iget_ref(tid);
    VERIFY(rc, 1, "H5Iget_ref");

    /* Close the datatype by decrementing the reference count */
    rc = H5Idec_ref(tid);
    VERIFY(rc, 0, "H5Idec_ref");

    /* Try closing the datatype again (should fail) */
    H5E_BEGIN_TRY
    {
        ret = H5Tclose(tid);
    }
    H5E_END_TRY
    VERIFY(ret, FAIL, "H5Tclose");

    /* Check H5I operations on dataspaces */

    /* Create a dataspace */
    sid = H5Screate(H5S_SCALAR);
    CHECK(sid, FAIL, "H5Screate");

    /* Check the reference count */
    rc = H5Iget_ref(sid);
    VERIFY(rc, 1, "H5Iget_ref");

    /* Inc the reference count */
    rc = H5Iinc_ref(sid);
    VERIFY(rc, 2, "H5Iinc_ref");

    /* Close the dataspace normally */
    ret = H5Sclose(sid);
    CHECK(ret, FAIL, "H5Sclose");

    /* Check the reference count */
    rc = H5Iget_ref(sid);
    VERIFY(rc, 1, "H5Iget_ref");

    /* Close the dataspace by decrementing the reference count */
    rc = H5Idec_ref(sid);
    VERIFY(rc, 0, "H5Idec_ref");

    /* Try closing the dataspace again (should fail) */
    H5E_BEGIN_TRY
    {
        ret = H5Sclose(sid);
    }
    H5E_END_TRY
    VERIFY(ret, FAIL, "H5Sclose");

    /* Check H5I operations on datasets */

    /* Create a file */
    fid = H5Fcreate(MISC19_FILE, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
    CHECK(fid, FAIL, "H5Fcreate");

    /* Create a dataspace */
    sid = H5Screate(H5S_SCALAR);
    CHECK(sid, FAIL, "H5Screate");

    /* Create a dataset */
    did = H5Dcreate2(fid, MISC19_DSET_NAME, H5T_NATIVE_INT, sid, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
    CHECK(did, FAIL, "H5Dcreate2");

    /* Check the reference count */
    rc = H5Iget_ref(did);
    VERIFY(rc, 1, "H5Iget_ref");

    /* Inc the reference count */
    rc = H5Iinc_ref(did);
    VERIFY(rc, 2, "H5Iinc_ref");

    /* Close the dataset normally */
    ret = H5Dclose(did);
    CHECK(ret, FAIL, "H5Dclose");

    /* Check the reference count */
    rc = H5Iget_ref(did);
    VERIFY(rc, 1, "H5Iget_ref");

    /* Close the dataset by decrementing the reference count */
    rc = H5Idec_ref(did);
    VERIFY(rc, 0, "H5Idec_ref");

    /* Try closing the dataset again (should fail) */
    H5E_BEGIN_TRY
    {
        ret = H5Dclose(did);
    }
    H5E_END_TRY
    VERIFY(ret, FAIL, "H5Dclose");

    /* Close the dataspace */
    ret = H5Sclose(sid);
    CHECK(ret, FAIL, "H5Sclose");

    /* Close the file */
    ret = H5Fclose(fid);
    CHECK(ret, FAIL, "H5Fclose");

    /* Check H5I operations on attributes */

    /* Create a file */
    fid = H5Fcreate(MISC19_FILE, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
    CHECK(fid, FAIL, "H5Fcreate");

    /* Open the root group */
    gid = H5Gopen2(fid, "/", H5P_DEFAULT);
    CHECK(gid, FAIL, "H5Gopen2");

    /* Create a dataspace */
    sid = H5Screate(H5S_SCALAR);
    CHECK(sid, FAIL, "H5Screate");

    /* Create an attribute */
    aid = H5Acreate2(gid, MISC19_ATTR_NAME, H5T_NATIVE_INT, sid, H5P_DEFAULT, H5P_DEFAULT);
    CHECK(aid, FAIL, "H5Acreate2");

    /* Check the reference count */
    rc = H5Iget_ref(aid);
    VERIFY(rc, 1, "H5Iget_ref");

    /* Inc the reference count */
    rc = H5Iinc_ref(aid);
    VERIFY(rc, 2, "H5Iinc_ref");

    /* Close the dataset normally */
    ret = H5Aclose(aid);
    CHECK(ret, FAIL, "H5Aclose");

    /* Check the reference count */
    rc = H5Iget_ref(aid);
    VERIFY(rc, 1, "H5Iget_ref");

    /* Close the attribute by decrementing the reference count */
    rc = H5Idec_ref(aid);
    VERIFY(rc, 0, "H5Idec_ref");

    /* Try closing the attribute again (should fail) */
    H5E_BEGIN_TRY
    {
        ret = H5Aclose(aid);
    }
    H5E_END_TRY
    VERIFY(ret, FAIL, "H5Aclose");

    /* Close the root group */
    ret = H5Gclose(gid);
    CHECK(ret, FAIL, "H5Gclose");

    /* Close the dataspace */
    ret = H5Sclose(sid);
    CHECK(ret, FAIL, "H5Sclose");

    /* Close the file */
    ret = H5Fclose(fid);
    CHECK(ret, FAIL, "H5Fclose");

    /* Check H5I operations on groups */

    /* Create a file */
    fid = H5Fcreate(MISC19_FILE, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
    CHECK(fid, FAIL, "H5Fcreate");

    /* Create a group */
    gid = H5Gcreate2(fid, MISC19_GROUP_NAME, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
    CHECK(gid, FAIL, "H5Gcreate2");

    /* Check the reference count */
    rc = H5Iget_ref(gid);
    VERIFY(rc, 1, "H5Iget_ref");

    /* Inc the reference count */
    rc = H5Iinc_ref(gid);
    VERIFY(rc, 2, "H5Iinc_ref");

    /* Close the group normally */
    ret = H5Gclose(gid);
    CHECK(ret, FAIL, "H5Gclose");

    /* Check the reference count */
    rc = H5Iget_ref(gid);
    VERIFY(rc, 1, "H5Iget_ref");

    /* Close the group by decrementing the reference count */
    rc = H5Idec_ref(gid);
    VERIFY(rc, 0, "H5Idec_ref");

    /* Try closing the group again (should fail) */
    H5E_BEGIN_TRY
    {
        ret = H5Gclose(gid);
    }
    H5E_END_TRY
    VERIFY(ret, FAIL, "H5Gclose");

    /* Close the file */
    ret = H5Fclose(fid);
    CHECK(ret, FAIL, "H5Fclose");

    /* Check H5I operations on error classes */

    /* Create an error class */
    ecid = H5Eregister_class("foo", "bar", "baz");
    CHECK(ecid, FAIL, "H5Eregister_class");

    /* Check the reference count */
    rc = H5Iget_ref(ecid);
    VERIFY(rc, 1, "H5Iget_ref");

    /* Inc the reference count */
    rc = H5Iinc_ref(ecid);
    VERIFY(rc, 2, "H5Iinc_ref");

    /* Close the error class normally */
    ret = H5Eunregister_class(ecid);
    CHECK(ret, FAIL, "H5Eunregister_class");

    /* Check the reference count */
    rc = H5Iget_ref(ecid);
    VERIFY(rc, 1, "H5Iget_ref");

    /* Close the error class by decrementing the reference count */
    rc = H5Idec_ref(ecid);
    VERIFY(rc, 0, "H5Idec_ref");

    /* Try closing the error class again (should fail) */
    H5E_BEGIN_TRY
    {
        ret = H5Eunregister_class(ecid);
    }
    H5E_END_TRY
    VERIFY(ret, FAIL, "H5Eunregister_class");

    /* Check H5I operations on error messages */

    /* Create an error class */
    ecid = H5Eregister_class("foo", "bar", "baz");
    CHECK(ecid, FAIL, "H5Eregister_class");

    /* Create an error message */
    emid = H5Ecreate_msg(ecid, H5E_MAJOR, "mumble");
    CHECK(emid, FAIL, "H5Ecreate_msg");

    /* Check the reference count */
    rc = H5Iget_ref(emid);
    VERIFY(rc, 1, "H5Iget_ref");

    /* Inc the reference count */
    rc = H5Iinc_ref(emid);
    VERIFY(rc, 2, "H5Iinc_ref");

    /* Close the error message normally */
    ret = H5Eclose_msg(emid);
    CHECK(ret, FAIL, "H5Eclose_msg");

    /* Check the reference count */
    rc = H5Iget_ref(emid);
    VERIFY(rc, 1, "H5Iget_ref");

    /* Close the error message by decrementing the reference count */
    rc = H5Idec_ref(emid);
    VERIFY(rc, 0, "H5Idec_ref");

    /* Try closing the error message again (should fail) */
    H5E_BEGIN_TRY
    {
        ret = H5Eclose_msg(emid);
    }
    H5E_END_TRY
    VERIFY(ret, FAIL, "H5Eclose_msg");

    /* Close the error class */
    ret = H5Eunregister_class(ecid);
    CHECK(ret, FAIL, "H5Eunregister_class");

    /* Check H5I operations on error stacks */

    /* Create an error stack */
    esid = H5Eget_current_stack();
    CHECK(esid, FAIL, "H5Eget_current_stack");

    /* Check the reference count */
    rc = H5Iget_ref(esid);
    VERIFY(rc, 1, "H5Iget_ref");

    /* Inc the reference count */
    rc = H5Iinc_ref(esid);
    VERIFY(rc, 2, "H5Iinc_ref");

    /* Close the error stack normally */
    ret = H5Eclose_stack(esid);
    CHECK(ret, FAIL, "H5Eclose_stack");

    /* Check the reference count */
    rc = H5Iget_ref(esid);
    VERIFY(rc, 1, "H5Iget_ref");

    /* Close the error stack by decrementing the reference count */
    rc = H5Idec_ref(esid);
    VERIFY(rc, 0, "H5Idec_ref");

    /* Try closing the error stack again (should fail) */
    H5E_BEGIN_TRY
    {
        ret = H5Eclose_stack(esid);
    }
    H5E_END_TRY
    VERIFY(ret, FAIL, "H5Eclose_stack");

    /* Check H5I operations on virtual file drivers */

    /* Get a VFD class to register */
    vfd_cls = h5_get_dummy_vfd_class();
    CHECK_PTR(vfd_cls, "h5_get_dummy_vfd_class");

    /* Register a virtual file driver */
    vfdid = H5FDregister(vfd_cls);
    CHECK(vfdid, FAIL, "H5FDregister");

    /* Check the reference count */
    rc = H5Iget_ref(vfdid);
    VERIFY(rc, 1, "H5Iget_ref");

    /* Increment the reference count */
    rc = H5Iinc_ref(vfdid);
    VERIFY(rc, 2, "H5Iinc_ref");

    /* Unregister the VFD normally */
    ret = H5FDunregister(vfdid);
    CHECK(ret, FAIL, "H5FDunregister");

    /* Check the reference count */
    rc = H5Iget_ref(vfdid);
    VERIFY(rc, 1, "H5Iget_ref");

    /* Unregister the VFD by decrementing the reference count */
    rc = H5Idec_ref(vfdid);
    VERIFY(rc, 0, "H5Idec_ref");

    /* Try unregistering the VFD again (should fail) */
    H5E_BEGIN_TRY
    {
        ret = H5FDunregister(vfdid);
    }
    H5E_END_TRY
    VERIFY(ret, FAIL, "H5FDunregister");

    free(vfd_cls);

    /* Check H5I operations on virtual object connectors */

    /* Get a VOL class to register */
    vol_cls = h5_get_dummy_vol_class();
    CHECK_PTR(vol_cls, "h5_get_dummy_vol_class");

    /* Register a VOL connector */
    volid = H5VLregister_connector(vol_cls, H5P_DEFAULT);
    CHECK(volid, FAIL, "H5VLregister_connector");

    /* Check the reference count */
    rc = H5Iget_ref(volid);
    VERIFY(rc, 1, "H5Iget_ref");

    /* Increment the reference count */
    rc = H5Iinc_ref(volid);
    VERIFY(rc, 2, "H5Iinc_ref");

    /* Unregister the VOL connector normally */
    ret = H5VLunregister_connector(volid);
    CHECK(ret, FAIL, "H5VLunregister_connector");

    /* Check the reference count */
    rc = H5Iget_ref(volid);
    VERIFY(rc, 1, "H5Iget_ref");

    /* Unregister the VOL connector by decrementing the reference count */
    rc = H5Idec_ref(volid);
    VERIFY(rc, 0, "H5Idec_ref");

    /* Try unregistering the VOL connector again (should fail) */
    H5E_BEGIN_TRY
    {
        ret = H5VLunregister_connector(volid);
    }
    H5E_END_TRY
    VERIFY(ret, FAIL, "H5VLunregister_connector");

    free(vol_cls);

} /* end test_misc19() */

/****************************************************************
**
**  test_misc20(): Test problems with version 2 of storage layout
**                      message truncating dimensions
**
****************************************************************/
static void
test_misc20(void)
{
    hid_t    fid;                                               /* File ID */
    hid_t    sid;                                               /* 'Space ID */
    hid_t    did;                                               /* Dataset ID */
    hid_t    dcpl;                                              /* Dataset creation property list ID */
    int      rank                          = MISC20_SPACE_RANK; /* Rank of dataspace */
    hsize_t  big_dims[MISC20_SPACE_RANK]   = {MISC20_SPACE_DIM0, MISC20_SPACE_DIM1};   /* Large dimensions */
    hsize_t  small_dims[MISC20_SPACE_RANK] = {MISC20_SPACE2_DIM0, MISC20_SPACE2_DIM1}; /* Small dimensions */
    unsigned version;     /* Version of storage layout info */
    hsize_t  contig_size; /* Size of contiguous storage size from layout into */
    const char *testfile = H5_get_srcdir_filename(MISC20_FILE_OLD); /* Corrected test file name */
    bool        vol_is_native;
    bool        driver_is_default_compatible;
    herr_t      ret; /* Generic return value */

    /* Output message about test being performed */
    MESSAGE(5, ("Testing large dimension truncation fix\n"));

    /* Create the file */
    fid = H5Fcreate(MISC20_FILE, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
    CHECK(fid, FAIL, "H5Fcreate");

    /* Check if native VOL is being used */
    CHECK(h5_using_native_vol(H5P_DEFAULT, fid, &vol_is_native), FAIL, "h5_using_native_vol");
    if (!vol_is_native) {
        CHECK(H5Fclose(fid), FAIL, "H5Fclose");
        MESSAGE(5, (" -- SKIPPED --\n"));
        return;
    }
    /* Check if VFD used is native file format compatible */
    CHECK(h5_driver_is_default_vfd_compatible(H5P_DEFAULT, &driver_is_default_compatible), FAIL,
          "h5_driver_is_default_vfd_compatible");
    if (!driver_is_default_compatible) {
        CHECK(H5Fclose(fid), FAIL, "H5Fclose");
        MESSAGE(5, (" -- SKIPPED --\n"));
        return;
    }

    /* Verify that chunks with dimensions that are too large get rejected */

    /* Create a dataset creation property list */
    dcpl = H5Pcreate(H5P_DATASET_CREATE);
    CHECK(dcpl, FAIL, "H5Pcreate");

    /* Try to use chunked storage for this dataset */
    ret = H5Pset_chunk(dcpl, rank, big_dims);
    VERIFY(ret, FAIL, "H5Pset_chunk");

    /* Verify that the storage for the dataset is the correct size and hasn't
     * been truncated.
     */

    /* Create dataspace with _really_ big dimensions */
    sid = H5Screate_simple(rank, big_dims, NULL);
    CHECK(sid, FAIL, "H5Screate_simple");

    /* Make certain that the dataset's storage doesn't get allocated :-) */
    ret = H5Pset_alloc_time(dcpl, H5D_ALLOC_TIME_LATE);
    CHECK(ret, FAIL, "H5Pset_alloc_time");

    /* Create dataset with big dataspace */
    did = H5Dcreate2(fid, MISC20_DSET_NAME, H5T_NATIVE_INT, sid, H5P_DEFAULT, dcpl, H5P_DEFAULT);
    CHECK(did, FAIL, "H5Dcreate2");

    /* Close datasset */
    ret = H5Dclose(did);
    CHECK(ret, FAIL, "H5Dclose");

    /* Close dataspace */
    ret = H5Sclose(sid);
    CHECK(ret, FAIL, "H5Sclose");

    /* Create dataspace with small dimensions */
    sid = H5Screate_simple(rank, small_dims, NULL);
    CHECK(sid, FAIL, "H5Screate_simple");

    /* Create dataset with big dataspace */
    did = H5Dcreate2(fid, MISC20_DSET2_NAME, H5T_NATIVE_INT, sid, H5P_DEFAULT, dcpl, H5P_DEFAULT);
    CHECK(did, FAIL, "H5Dcreate2");

    /* Close datasset */
    ret = H5Dclose(did);
    CHECK(ret, FAIL, "H5Dclose");

    /* Close dataspace */
    ret = H5Sclose(sid);
    CHECK(ret, FAIL, "H5Sclose");

    /* Close dataset creation property list */
    ret = H5Pclose(dcpl);
    CHECK(ret, FAIL, "H5Pclose");

    /* Close file */
    ret = H5Fclose(fid);
    CHECK(ret, FAIL, "H5Fclose");

    /* Re-open the file */
    fid = H5Fopen(MISC20_FILE, H5F_ACC_RDONLY, H5P_DEFAULT);
    CHECK(fid, FAIL, "H5Fopen");

    /* Open dataset with big dimensions */
    did = H5Dopen2(fid, MISC20_DSET_NAME, H5P_DEFAULT);
    CHECK(did, FAIL, "H5Dopen2");

    /* Get the layout version */
    ret = H5D__layout_version_test(did, &version);
    CHECK(ret, FAIL, "H5D__layout_version_test");
    VERIFY(version, 3, "H5D__layout_version_test");

    /* Get the layout contiguous storage size */
    ret = H5D__layout_contig_size_test(did, &contig_size);
    CHECK(ret, FAIL, "H5D__layout_contig_size_test");
    VERIFY(contig_size, (MISC20_SPACE_DIM0 * MISC20_SPACE_DIM1 * H5Tget_size(H5T_NATIVE_INT)),
           "H5D__layout_contig_size_test");

    /* Close datasset */
    ret = H5Dclose(did);
    CHECK(ret, FAIL, "H5Dclose");

    /* Open dataset with small dimensions */
    did = H5Dopen2(fid, MISC20_DSET2_NAME, H5P_DEFAULT);
    CHECK(did, FAIL, "H5Dopen2");

    /* Get the layout version */
    ret = H5D__layout_version_test(did, &version);
    CHECK(ret, FAIL, "H5D__layout_version_test");
    VERIFY(version, 3, "H5D__layout_version_test");

    /* Get the layout contiguous storage size */
    ret = H5D__layout_contig_size_test(did, &contig_size);
    CHECK(ret, FAIL, "H5D__layout_contig_size_test");
    VERIFY(contig_size, (MISC20_SPACE2_DIM0 * MISC20_SPACE2_DIM1 * H5Tget_size(H5T_NATIVE_INT)),
           "H5D__layout_contig_size_test");

    /* Close datasset */
    ret = H5Dclose(did);
    CHECK(ret, FAIL, "H5Dclose");

    /* Close file */
    ret = H5Fclose(fid);
    CHECK(ret, FAIL, "H5Fclose");

    /* Verify that the storage size is computed correctly for older versions of layout info */

    /*
     * Open the old file and the dataset and get old settings.
     */
    fid = H5Fopen(testfile, H5F_ACC_RDONLY, H5P_DEFAULT);
    CHECK(fid, FAIL, "H5Fopen");

    /* Open dataset with small dimensions */
    did = H5Dopen2(fid, MISC20_DSET_NAME, H5P_DEFAULT);
    CHECK(did, FAIL, "H5Dopen2");

    /* Get the layout version */
    ret = H5D__layout_version_test(did, &version);
    CHECK(ret, FAIL, "H5D__layout_version_test");
    VERIFY(version, 2, "H5D__layout_version_test");

    /* Get the layout contiguous storage size */
    ret = H5D__layout_contig_size_test(did, &contig_size);
    CHECK(ret, FAIL, "H5D__layout_contig_size_test");
    VERIFY(contig_size, (MISC20_SPACE_DIM0 * MISC20_SPACE_DIM1 * H5Tget_size(H5T_STD_I32LE)),
           "H5D__layout_contig_size_test");

    /* Close datasset */
    ret = H5Dclose(did);
    CHECK(ret, FAIL, "H5Dclose");

    /* Close file */
    ret = H5Fclose(fid);
    CHECK(ret, FAIL, "H5Fclose");

} /* end test_misc20() */

/*
   test_misc21 and test_misc22 should be executed when SZIP is present
   and encoder is available.
                            EIP 2004/8/04
*/
#ifdef H5_HAVE_FILTER_SZIP

/****************************************************************
**
**  test_misc21(): Test that late allocation time is treated the same
**                      as incremental allocation time, for chunked datasets
**                      when overwriting entire dataset where the chunks
**                      don't exactly match the dataspace.
**
****************************************************************/
static void
test_misc21(void)
{
    hid_t   fid, sid, dcpl, dsid;
    char   *buf;
    hsize_t dims[2]       = {MISC21_SPACE_DIM0, MISC21_SPACE_DIM1},
            chunk_size[2] = {MISC21_CHUNK_DIM0, MISC21_CHUNK_DIM1};
    herr_t ret; /* Generic return value */

    if (h5_szip_can_encode() != 1)
        return;
    /* Output message about test being performed */
    MESSAGE(5, ("Testing late allocation time w/chunks & filters\n"));

    /* Allocate space for the buffer */
    buf = (char *)calloc(MISC21_SPACE_DIM0 * MISC21_SPACE_DIM1, 1);
    CHECK_PTR(buf, "calloc");

    /* Create the file */
    fid = H5Fcreate(MISC21_FILE, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
    CHECK(fid, FAIL, "H5Fcreate");

    /* Create the DCPL */
    dcpl = H5Pcreate(H5P_DATASET_CREATE);
    CHECK(dcpl, FAIL, "H5Pcreate");

    /* Set custom DCPL properties */
    ret = H5Pset_chunk(dcpl, MISC21_SPACE_RANK, chunk_size);
    CHECK(ret, FAIL, "H5Pset_chunk");
    ret = H5Pset_szip(dcpl, H5_SZIP_NN_OPTION_MASK, 8);
    CHECK(ret, FAIL, "H5Pset_deflate");
    ret = H5Pset_alloc_time(dcpl, H5D_ALLOC_TIME_LATE);
    CHECK(ret, FAIL, "H5Pset_alloc_time");

    /* Create the dataspace for the dataset */
    sid = H5Screate_simple(MISC21_SPACE_RANK, dims, NULL);
    CHECK(ret, FAIL, "H5Screate_simple");

    /* Create the dataset */
    dsid = H5Dcreate2(fid, MISC21_DSET_NAME, H5T_NATIVE_UINT8, sid, H5P_DEFAULT, dcpl, H5P_DEFAULT);
    CHECK(dsid, FAIL, "H5Dcreate2");

    /* Write out the whole dataset */
    ret = H5Dwrite(dsid, H5T_NATIVE_UINT8, H5S_ALL, H5S_ALL, H5P_DEFAULT, buf);
    CHECK(ret, FAIL, "H5Dwrite");

    /* Close everything */
    ret = H5Dclose(dsid);
    CHECK(ret, FAIL, "H5Dclose");
    ret = H5Sclose(sid);
    CHECK(ret, FAIL, "H5Sclose");
    ret = H5Pclose(dcpl);
    CHECK(ret, FAIL, "H5Pclose");
    ret = H5Fclose(fid);
    CHECK(ret, FAIL, "H5Fclose");

    free(buf);
} /* end test_misc21() */

/****************************************************************
**
**  test_misc22(): Test SZIP bits-per-pixel parameter.
**                      This should be set according to the datatype.
**                      Tests for precision and offset combo's.
**
****************************************************************/
static void
test_misc22(void)
{
    hid_t   fapl; /* File access property list */
    hid_t   fid, sid, dcpl, dsid, dcpl2;
    char   *buf;
    hsize_t dims[2]       = {MISC22_SPACE_DIM0, MISC22_SPACE_DIM1},
            chunk_size[2] = {MISC22_CHUNK_DIM0, MISC22_CHUNK_DIM1};
    herr_t ret; /* Generic return value */
    hid_t  dtype;
    /* should extend test to signed ints */
    hid_t idts[4];
    /*  do the same for floats
        hid_t fdts[2]={H5T_NATIVE_FLOAT32,
                  H5T_NATIVE_FLOAT64}
    */
    size_t       prec[4]    = {3, 11, 19, 27};
    size_t       offsets[5] = {0, 3, 11, 19, 27};
    int          i, j, k;
    unsigned int flags;
    size_t       cd_nelmts = 32;
    unsigned int cd_values[32];
    size_t       correct;

    if (h5_szip_can_encode() != 1)
        return;
    idts[0] = H5Tcopy(H5T_NATIVE_UINT8);
    idts[1] = H5Tcopy(H5T_NATIVE_UINT16);
    idts[2] = H5Tcopy(H5T_NATIVE_UINT32);
    idts[3] = H5Tcopy(H5T_NATIVE_UINT64);

    /* Output message about test being performed */
    MESSAGE(5, ("Testing datatypes with SZIP filter\n"));

    /* Allocate space for the buffer */
    buf = (char *)calloc(MISC22_SPACE_DIM0 * MISC22_SPACE_DIM1, 8);
    CHECK_PTR(buf, "calloc");

    /* Create a file access property list */
    fapl = H5Pcreate(H5P_FILE_ACCESS);
    CHECK(fapl, FAIL, "H5Pcreate");

    /* Set property to allow unusual datatypes to be created */
    ret = H5Pset_relax_file_integrity_checks(fapl, H5F_RFIC_UNUSUAL_NUM_UNUSED_NUMERIC_BITS);
    CHECK(ret, FAIL, "H5Pset_relax_file_integrity_checks");

    /* Create the file */
    fid = H5Fcreate(MISC22_FILE, H5F_ACC_TRUNC, H5P_DEFAULT, fapl);
    CHECK(fid, FAIL, "H5Fcreate");

    /* Close file access property list */
    ret = H5Pclose(fapl);
    CHECK(ret, FAIL, "H5Pclose");

    /* Create the dataspace for the dataset */
    sid = H5Screate_simple(MISC22_SPACE_RANK, dims, NULL);
    CHECK(sid, FAIL, "H5Screate_simple");

    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            if (prec[j] > (H5Tget_size(idts[i]) * 8))
                continue; /* skip irrelevant combination */
            for (k = 0; k < 5; k++) {
                if (offsets[k] > (H5Tget_size(idts[i]) * 8))
                    continue; /* skip irrelevant combinations */
                if ((prec[j] + offsets[k]) > (H5Tget_size(idts[i]) * 8))
                    continue;

                MESSAGE(5, ("  Testing datatypes size=%zu precision=%u offset=%d\n", H5Tget_size(idts[i]),
                            (unsigned)prec[j], (unsigned)offsets[k]));

                /* Create the DCPL */
                dcpl = H5Pcreate(H5P_DATASET_CREATE);
                CHECK(dcpl, FAIL, "H5Pcreate");

                /* Set DCPL properties */
                ret = H5Pset_chunk(dcpl, MISC22_SPACE_RANK, chunk_size);
                CHECK(ret, FAIL, "H5Pset_chunk");
                /* Set custom DCPL properties */
                ret = H5Pset_szip(dcpl, H5_SZIP_NN_OPTION_MASK, 32); /* vary the PPB */
                CHECK(ret, FAIL, "H5Pset_szip");

                /* set up the datatype according to the loop */
                dtype = H5Tcopy(idts[i]);
                CHECK(dtype, FAIL, "H5Tcopy");
                ret = H5Tset_precision(dtype, prec[j]);
                CHECK(ret, FAIL, "H5Tset_precision");
                ret = H5Tset_offset(dtype, offsets[k]);
                CHECK(ret, FAIL, "H5Tset_precision");

                /* compute the correct PPB that should be set by SZIP */
                if (offsets[k] == 0)
                    correct = prec[j];
                else
                    correct = H5Tget_size(idts[i]) * 8;
                if (correct > 24) {
                    if (correct <= 32)
                        correct = 32;
                    else if (correct <= 64)
                        correct = 64;
                } /* end if */

                /* Create the dataset */
                dsid = H5Dcreate2(fid, MISC22_DSET_NAME, dtype, sid, H5P_DEFAULT, dcpl, H5P_DEFAULT);
                CHECK(dsid, FAIL, "H5Dcreate2");

                /* Write out the whole dataset */
                ret = H5Dwrite(dsid, dtype, H5S_ALL, H5S_ALL, H5P_DEFAULT, buf);
                CHECK(ret, FAIL, "H5Dwrite");

                /* Close everything */
                ret = H5Dclose(dsid);
                CHECK(ret, FAIL, "H5Dclose");
                ret = H5Tclose(dtype);
                CHECK(ret, FAIL, "H5Tclose");
                ret = H5Pclose(dcpl);
                CHECK(ret, FAIL, "H5Pclose");

                dsid = H5Dopen2(fid, MISC22_DSET_NAME, H5P_DEFAULT);
                CHECK(dsid, FAIL, "H5Dopen2");

                dcpl2 = H5Dget_create_plist(dsid);
                CHECK(dcpl2, FAIL, "H5Dget_create_plist");

                ret = H5Pget_filter_by_id2(dcpl2, H5Z_FILTER_SZIP, &flags, &cd_nelmts, cd_values, 0, NULL,
                                           NULL);
                CHECK(ret, FAIL, "H5Pget_filter_by_id2");

                VERIFY(cd_values[2], (unsigned)correct, "SZIP filter returned value for precision");

                ret = H5Dclose(dsid);
                CHECK(ret, FAIL, "H5Dclose");

                ret = H5Ldelete(fid, MISC22_DSET_NAME, H5P_DEFAULT);
                CHECK(ret, FAIL, "H5Ldelete");

                ret = H5Pclose(dcpl2);
                CHECK(ret, FAIL, "H5Pclose");
            }
        }
    }
    ret = H5Tclose(idts[0]);
    CHECK(ret, FAIL, "H5Tclose");
    ret = H5Tclose(idts[1]);
    CHECK(ret, FAIL, "H5Tclose");
    ret = H5Tclose(idts[2]);
    CHECK(ret, FAIL, "H5Tclose");
    ret = H5Tclose(idts[3]);
    CHECK(ret, FAIL, "H5Tclose");
    ret = H5Sclose(sid);
    CHECK(ret, FAIL, "H5Sclose");
    ret = H5Fclose(fid);
    CHECK(ret, FAIL, "H5Fclose");

    free(buf);
} /* end test_misc22() */
#endif /* H5_HAVE_FILTER_SZIP */

/****************************************************************
**
**  test_misc23(): Test intermediate group creation.
**
****************************************************************/
static void
test_misc23(void)
{
    hsize_t dims[]  = {10};
    hid_t   file_id = 0, group_id = 0, type_id = 0, space_id = 0, tmp_id = 0, create_id = H5P_DEFAULT,
          access_id = H5P_DEFAULT;
    char        objname[MISC23_NAME_BUF_SIZE]; /* Name of object */
    H5O_info2_t oinfo;
    htri_t      tri_status;
    ssize_t     namelen;
    herr_t      status;

    /* Output message about test being performed */
    MESSAGE(5, ("Testing intermediate group creation\n"));

    /* Create a new file using default properties. */
    file_id = H5Fcreate(MISC23_FILE, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
    CHECK(file_id, FAIL, "H5Fcreate");

    /* Build some infrastructure */
    group_id = H5Gcreate2(file_id, "/A", H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
    CHECK(group_id, FAIL, "H5Gcreate2");

    space_id = H5Screate_simple(1, dims, NULL);
    CHECK(space_id, FAIL, "H5Screate_simple");

    type_id = H5Tcopy(H5T_STD_I32BE);
    CHECK(type_id, FAIL, "H5Tcopy");

#ifndef H5_NO_DEPRECATED_SYMBOLS
    /**********************************************************************
     * test the old APIs
     **********************************************************************/

    H5E_BEGIN_TRY
    {
        tmp_id = H5Gcreate1(file_id, "/A/B00a/grp", (size_t)0);
    }
    H5E_END_TRY
    VERIFY(tmp_id, FAIL, "H5Gcreate1");

    /* Make sure that size_hint values that can't fit into a 32-bit
     * unsigned integer are rejected. Only necessary on systems where
     * size_t is a 64-bit type.
     */
    if (SIZE_MAX > UINT32_MAX) {
        H5E_BEGIN_TRY
        {
            tmp_id = H5Gcreate1(file_id, "/size_hint_too_large", SIZE_MAX);
        }
        H5E_END_TRY
        VERIFY(tmp_id, FAIL, "H5Gcreate1");
    }

    /* Make sure the largest size_hint value works */
    H5E_BEGIN_TRY
    {
        tmp_id = H5Gcreate1(file_id, "/largest_size_hint", UINT32_MAX);
    }
    H5E_END_TRY
    CHECK(tmp_id, FAIL, "H5Gcreate1");
    status = H5Gclose(tmp_id);
    CHECK(status, FAIL, "H5Gclose");

    tmp_id = H5Gcreate1(file_id, "/A/grp", (size_t)0);
    CHECK(tmp_id, FAIL, "H5Gcreate1");
    status = H5Gclose(tmp_id);
    CHECK(status, FAIL, "H5Gclose");

    H5E_BEGIN_TRY
    {
        tmp_id = H5Dcreate1(file_id, "/A/B00c/dset", type_id, space_id, create_id);
    }
    H5E_END_TRY
    VERIFY(tmp_id, FAIL, "H5Dcreate1");

    tmp_id = H5Dcreate1(file_id, "/A/dset", type_id, space_id, create_id);
    CHECK(tmp_id, FAIL, "H5Dcreate1");
    status = H5Dclose(tmp_id);
    CHECK(status, FAIL, "H5Dclose");
#endif /* H5_NO_DEPRECATED_SYMBOLS */

    /**********************************************************************
     * test H5Gcreate2()
     **********************************************************************/

    /* Create link creation property list */
    create_id = H5Pcreate(H5P_LINK_CREATE);
    CHECK(create_id, FAIL, "H5Pcreate");

    /* Set flag for intermediate group creation */
    status = H5Pset_create_intermediate_group(create_id, true);
    CHECK(status, FAIL, "H5Pset_create_intermediate_group");

    tmp_id = H5Gcreate2(file_id, "/A/B01/grp", create_id, H5P_DEFAULT, access_id);
    CHECK(tmp_id, FAIL, "H5Gcreate2");

    /* Query that the name of the new group is correct */
    namelen = H5Iget_name(tmp_id, objname, (size_t)MISC23_NAME_BUF_SIZE);
    CHECK(namelen, FAIL, "H5Iget_name");
    VERIFY_STR(objname, "/A/B01/grp", "H5Iget_name");
    VERIFY(namelen, strlen("/A/B01/grp"), "H5Iget_name");

    status = H5Gclose(tmp_id);
    CHECK(status, FAIL, "H5Gclose");

    /* Check that intermediate group is set up correctly */
    tmp_id = H5Gopen2(file_id, "/A/B01", H5P_DEFAULT);
    CHECK(tmp_id, FAIL, "H5Gopen2");

    status = H5Oget_info3(tmp_id, &oinfo, H5O_INFO_BASIC);
    CHECK(status, FAIL, "H5Oget_info3");
    VERIFY(oinfo.rc, 1, "H5Oget_info3");

    status = H5Gclose(tmp_id);
    CHECK(status, FAIL, "H5Gclose");

    tmp_id = H5Gcreate2(file_id, "/A/B02/C02/grp", create_id, H5P_DEFAULT, access_id);
    CHECK(tmp_id, FAIL, "H5Gcreate2");

    status = H5Gclose(tmp_id);
    CHECK(status, FAIL, "H5Gclose");

    tmp_id = H5Gcreate2(group_id, "B03/grp/", create_id, H5P_DEFAULT, access_id);
    CHECK(tmp_id, FAIL, "H5Gcreate2");

    status = H5Gclose(tmp_id);
    CHECK(status, FAIL, "H5Gclose");

    tmp_id = H5Gcreate2(group_id, "/A/B04/grp/", create_id, H5P_DEFAULT, access_id);
    CHECK(tmp_id, FAIL, "H5Gcreate2");

    status = H5Gclose(tmp_id);
    CHECK(status, FAIL, "H5Gclose");

    tmp_id = H5Gcreate2(file_id, "/A/B05/C05/A", create_id, H5P_DEFAULT, access_id);
    CHECK(tmp_id, FAIL, "H5Gcreate2");

    status = H5Gclose(tmp_id);
    CHECK(status, FAIL, "H5Gclose");

    status = H5Pclose(create_id);
    CHECK(status, FAIL, "H5Pclose");

    /**********************************************************************
     * test H5Dcreate2()
     **********************************************************************/

    /* Create link creation property list */
    create_id = H5Pcreate(H5P_LINK_CREATE);
    CHECK(create_id, FAIL, "H5Pcreate");

    /* Set flag for intermediate group creation */
    status = H5Pset_create_intermediate_group(create_id, true);
    CHECK(status, FAIL, "H5Pset_create_intermediate_group");

    tmp_id = H5Dcreate2(file_id, "/A/B06/dset", type_id, space_id, create_id, H5P_DEFAULT, H5P_DEFAULT);
    CHECK(tmp_id, FAIL, "H5Dcreate2");

    status = H5Dclose(tmp_id);
    CHECK(status, FAIL, "H5Dclose");

    tmp_id = H5Dcreate2(file_id, "/A/B07/B07/dset", type_id, space_id, create_id, H5P_DEFAULT, H5P_DEFAULT);
    CHECK(tmp_id, FAIL, "H5Dcreate2");

    status = H5Dclose(tmp_id);
    CHECK(status, FAIL, "H5Dclose");

    tmp_id = H5Dcreate2(group_id, "B08/dset", type_id, space_id, create_id, H5P_DEFAULT, H5P_DEFAULT);
    CHECK(tmp_id, FAIL, "H5Dcreate2");

    status = H5Dclose(tmp_id);
    CHECK(status, FAIL, "H5Dclose");

    tmp_id = H5Dcreate2(group_id, "/A/B09/dset", type_id, space_id, create_id, H5P_DEFAULT, H5P_DEFAULT);
    CHECK(tmp_id, FAIL, "H5Dcreate2");

    status = H5Dclose(tmp_id);
    CHECK(status, FAIL, "H5Dclose");

    tmp_id = H5Dcreate2(file_id, "/A/B10/C10/A/dset", type_id, space_id, create_id, H5P_DEFAULT, H5P_DEFAULT);
    CHECK(tmp_id, FAIL, "H5Dcreate2");

    status = H5Dclose(tmp_id);
    CHECK(status, FAIL, "H5Dclose");

    status = H5Tclose(type_id);
    CHECK(status, FAIL, "H5Tclose");

    status = H5Sclose(space_id);
    CHECK(status, FAIL, "H5Sclose");

    status = H5Pclose(create_id);
    CHECK(status, FAIL, "H5Pclose");

    /**********************************************************************
     * test H5Tcommit2()
     **********************************************************************/

    /* Create link creation property list */
    create_id = H5Pcreate(H5P_LINK_CREATE);
    CHECK(create_id, FAIL, "H5Pcreate");

    /* Set flag for intermediate group creation */
    status = H5Pset_create_intermediate_group(create_id, true);
    CHECK(status, FAIL, "H5Pset_create_intermediate_group");

    tmp_id = H5Tcopy(H5T_NATIVE_INT16);
    CHECK(tmp_id, FAIL, "H5Tcopy");

    status = H5Tcommit2(file_id, "/A/B11/dtype", tmp_id, create_id, H5P_DEFAULT, access_id);
    CHECK(status, FAIL, "H5Tcommit2");

    status = H5Tclose(tmp_id);
    CHECK(status, FAIL, "H5Tclose");

    tmp_id = H5Tcopy(H5T_NATIVE_INT32);
    CHECK(tmp_id, FAIL, "H5Tcopy");

    status = H5Tcommit2(file_id, "/A/B12/C12/dtype", tmp_id, create_id, H5P_DEFAULT, access_id);
    CHECK(status, FAIL, "H5Tcommit2");

    status = H5Tclose(tmp_id);
    CHECK(status, FAIL, "H5Tclose");

    tmp_id = H5Tcopy(H5T_NATIVE_INT64);
    CHECK(tmp_id, FAIL, "H5Tcopy");

    status = H5Tcommit2(group_id, "B13/C12/dtype", tmp_id, create_id, H5P_DEFAULT, access_id);
    CHECK(status, FAIL, "H5Tcommit2");

    status = H5Tclose(tmp_id);
    CHECK(status, FAIL, "H5Tclose");

    tmp_id = H5Tcopy(H5T_NATIVE_FLOAT);
    CHECK(tmp_id, FAIL, "H5Tcopy");

    status = H5Tcommit2(group_id, "/A/B14/dtype", tmp_id, create_id, H5P_DEFAULT, access_id);
    CHECK(status, FAIL, "H5Tcommit2");

    status = H5Tclose(tmp_id);
    CHECK(status, FAIL, "H5Tclose");

    tmp_id = H5Tcopy(H5T_NATIVE_DOUBLE);
    CHECK(tmp_id, FAIL, "H5Tcopy");

    status = H5Tcommit2(file_id, "/A/B15/C15/A/dtype", tmp_id, create_id, H5P_DEFAULT, access_id);
    CHECK(status, FAIL, "H5Tcommit2");

    status = H5Tclose(tmp_id);
    CHECK(status, FAIL, "H5Tclose");

    status = H5Pclose(create_id);
    CHECK(status, FAIL, "H5Pclose");

    /**********************************************************************
     * test H5Lcopy()
     **********************************************************************/

    /* Create link creation property list */
    create_id = H5Pcreate(H5P_LINK_CREATE);
    CHECK(create_id, FAIL, "H5Pcreate");

    /* Set flag for intermediate group creation */
    status = H5Pset_create_intermediate_group(create_id, true);
    CHECK(status, FAIL, "H5Pset_create_intermediate_group");

    status = H5Lcopy(file_id, "/A/B01/grp", file_id, "/A/B16/grp", create_id, access_id);
    CHECK(status, FAIL, "H5Lcopy");

    tri_status = H5Lexists(file_id, "/A/B16/grp", access_id);
    VERIFY(tri_status, true, "H5Lexists");

    tri_status = H5Lexists(file_id, "/A/B01/grp", access_id);
    VERIFY(tri_status, true, "H5Lexists");

    /**********************************************************************
     * test H5Lmove()
     **********************************************************************/

    status = H5Lmove(file_id, "/A/B16/grp", file_id, "/A/B17/grp", create_id, access_id);
    CHECK(status, FAIL, "H5Lmove");

    tri_status = H5Lexists(file_id, "/A/B17/grp", access_id);
    VERIFY(tri_status, true, "H5Lexists");

    tri_status = H5Lexists(file_id, "/A/B16/grp", access_id);
    VERIFY(tri_status, false, "H5Lexists");

    /**********************************************************************
     * test H5Lcreate_hard()
     **********************************************************************/

    status = H5Lcreate_hard(file_id, "/A/B01/grp", file_id, "/A/B18/grp", create_id, access_id);
    CHECK(status, FAIL, "H5Lcreate_hard");

    tri_status = H5Lexists(file_id, "/A/B18/grp", access_id);
    VERIFY(tri_status, true, "H5Lexists");

    /**********************************************************************
     * test H5Lcreate_soft()
     **********************************************************************/

    status = H5Lcreate_soft("/A/B01/grp", file_id, "/A/B19/grp", create_id, access_id);
    CHECK(status, FAIL, "H5Lcreate_soft");

    tri_status = H5Lexists(file_id, "/A/B19/grp", access_id);
    VERIFY(tri_status, true, "H5Lexists");

    /**********************************************************************
     * test H5Lcreate_external()
     **********************************************************************/

    if (vol_cap_flags_g & H5VL_CAP_FLAG_EXTERNAL_LINKS) {
        status =
            H5Lcreate_external("fake_filename", "fake_path", file_id, "/A/B20/grp", create_id, access_id);
        CHECK(status, FAIL, "H5Lcreate_external");

        tri_status = H5Lexists(file_id, "/A/B20/grp", access_id);
        VERIFY(tri_status, true, "H5Lexists");
    }

    /**********************************************************************
     * test H5Lcreate_ud()
     **********************************************************************/

    if (vol_cap_flags_g & H5VL_CAP_FLAG_UD_LINKS) {
        status = H5Lcreate_ud(file_id, "/A/B21/grp", H5L_TYPE_EXTERNAL, "file\0obj", (size_t)9, create_id,
                              access_id);
        CHECK(status, FAIL, "H5Lcreate_ud");

        tri_status = H5Lexists(file_id, "/A/B21/grp", access_id);
        VERIFY(tri_status, true, "H5Lexists");
    }

    /**********************************************************************
     * close
     **********************************************************************/

    status = H5Pclose(create_id);
    CHECK(status, FAIL, "H5Pclose");

    status = H5Gclose(group_id);
    CHECK(status, FAIL, "H5Gclose");

    status = H5Fclose(file_id);
    CHECK(status, FAIL, "H5Fclose");

} /* end test_misc23() */

/****************************************************************
**
**  test_misc24(): Test opening objects with inappropriate APIs
**
****************************************************************/
static void
test_misc24(void)
{
    hid_t  file_id = 0, group_id = 0, type_id = 0, space_id = 0, dset_id = 0, tmp_id = 0;
    herr_t ret; /* Generic return value */

    /* Output message about test being performed */
    MESSAGE(5, ("Testing opening objects with inappropriate APIs\n"));

    /* Create a new file using default properties. */
    file_id = H5Fcreate(MISC24_FILE, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
    CHECK(file_id, FAIL, "H5Fcreate");

    /* Create dataspace for dataset */
    space_id = H5Screate(H5S_SCALAR);
    CHECK(space_id, FAIL, "H5Screate");

    /* Create group, dataset & named datatype objects */
    group_id = H5Gcreate2(file_id, MISC24_GROUP_NAME, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
    CHECK(group_id, FAIL, "H5Gcreate2");

    dset_id = H5Dcreate2(file_id, MISC24_DATASET_NAME, H5T_NATIVE_INT, space_id, H5P_DEFAULT, H5P_DEFAULT,
                         H5P_DEFAULT);
    CHECK(dset_id, FAIL, "H5Dcreate2");

    type_id = H5Tcopy(H5T_NATIVE_INT);
    CHECK(type_id, FAIL, "H5Tcopy");

    ret = H5Tcommit2(file_id, MISC24_DATATYPE_NAME, type_id, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
    CHECK(ret, FAIL, "H5Tcommit2");

    /* Create soft links to the objects created */
    ret = H5Lcreate_soft(MISC24_GROUP_NAME, file_id, MISC24_GROUP_LINK, H5P_DEFAULT, H5P_DEFAULT);
    CHECK(ret, FAIL, "H5Lcreate_soft");

    ret = H5Lcreate_soft(MISC24_DATASET_NAME, file_id, MISC24_DATASET_LINK, H5P_DEFAULT, H5P_DEFAULT);
    CHECK(ret, FAIL, "H5Lcreate_soft");

    ret = H5Lcreate_soft(MISC24_DATATYPE_NAME, file_id, MISC24_DATATYPE_LINK, H5P_DEFAULT, H5P_DEFAULT);
    CHECK(ret, FAIL, "H5Lcreate_soft");

    /* Close IDs for objects */
    ret = H5Dclose(dset_id);
    CHECK(ret, FAIL, "H5Dclose");

    ret = H5Sclose(space_id);
    CHECK(ret, FAIL, "H5Sclose");

    ret = H5Gclose(group_id);
    CHECK(ret, FAIL, "H5Gclose");

    ret = H5Tclose(type_id);
    CHECK(ret, FAIL, "H5Tclose");

    /* Attempt to open each kind of object with wrong API, including using soft links */
    H5E_BEGIN_TRY
    {
        tmp_id = H5Dopen2(file_id, MISC24_GROUP_NAME, H5P_DEFAULT);
    }
    H5E_END_TRY
    VERIFY(tmp_id, FAIL, "H5Dopen2");

    H5E_BEGIN_TRY
    {
        tmp_id = H5Dopen2(file_id, MISC24_GROUP_LINK, H5P_DEFAULT);
    }
    H5E_END_TRY
    VERIFY(tmp_id, FAIL, "H5Dopen2");

    H5E_BEGIN_TRY
    {
        tmp_id = H5Topen2(file_id, MISC24_GROUP_NAME, H5P_DEFAULT);
    }
    H5E_END_TRY
    VERIFY(tmp_id, FAIL, "H5Topen2");

    H5E_BEGIN_TRY
    {
        tmp_id = H5Topen2(file_id, MISC24_GROUP_LINK, H5P_DEFAULT);
    }
    H5E_END_TRY
    VERIFY(tmp_id, FAIL, "H5Topen2");

    H5E_BEGIN_TRY
    {
        tmp_id = H5Gopen2(file_id, MISC24_DATASET_NAME, H5P_DEFAULT);
    }
    H5E_END_TRY
    VERIFY(tmp_id, FAIL, "H5Gopen2");

    H5E_BEGIN_TRY
    {
        tmp_id = H5Gopen2(file_id, MISC24_DATASET_LINK, H5P_DEFAULT);
    }
    H5E_END_TRY
    VERIFY(tmp_id, FAIL, "H5Gopen2");

    H5E_BEGIN_TRY
    {
        tmp_id = H5Topen2(file_id, MISC24_DATASET_NAME, H5P_DEFAULT);
    }
    H5E_END_TRY
    VERIFY(tmp_id, FAIL, "H5Topen2");

    H5E_BEGIN_TRY
    {
        tmp_id = H5Topen2(file_id, MISC24_DATASET_LINK, H5P_DEFAULT);
    }
    H5E_END_TRY
    VERIFY(tmp_id, FAIL, "H5Topen2");

    H5E_BEGIN_TRY
    {
        tmp_id = H5Gopen2(file_id, MISC24_DATATYPE_NAME, H5P_DEFAULT);
    }
    H5E_END_TRY
    VERIFY(tmp_id, FAIL, "H5Gopen2");

    H5E_BEGIN_TRY
    {
        tmp_id = H5Gopen2(file_id, MISC24_DATATYPE_LINK, H5P_DEFAULT);
    }
    H5E_END_TRY
    VERIFY(tmp_id, FAIL, "H5Gopen2");

    H5E_BEGIN_TRY
    {
        tmp_id = H5Dopen2(file_id, MISC24_DATATYPE_NAME, H5P_DEFAULT);
    }
    H5E_END_TRY
    VERIFY(tmp_id, FAIL, "H5Dopen2");

    H5E_BEGIN_TRY
    {
        tmp_id = H5Dopen2(file_id, MISC24_DATATYPE_LINK, H5P_DEFAULT);
    }
    H5E_END_TRY
    VERIFY(tmp_id, FAIL, "H5Dopen2");

    /* Try again, with the object already open through valid call */
    /* Open group */
    group_id = H5Gopen2(file_id, MISC24_GROUP_NAME, H5P_DEFAULT);
    CHECK(group_id, FAIL, "H5Gopen2");

    H5E_BEGIN_TRY
    {
        tmp_id = H5Dopen2(file_id, MISC24_GROUP_NAME, H5P_DEFAULT);
    }
    H5E_END_TRY
    VERIFY(tmp_id, FAIL, "H5Dopen2");

    H5E_BEGIN_TRY
    {
        tmp_id = H5Dopen2(file_id, MISC24_GROUP_LINK, H5P_DEFAULT);
    }
    H5E_END_TRY
    VERIFY(tmp_id, FAIL, "H5Dopen2");

    H5E_BEGIN_TRY
    {
        tmp_id = H5Topen2(file_id, MISC24_GROUP_NAME, H5P_DEFAULT);
    }
    H5E_END_TRY
    VERIFY(tmp_id, FAIL, "H5Topen2");

    H5E_BEGIN_TRY
    {
        tmp_id = H5Topen2(file_id, MISC24_GROUP_LINK, H5P_DEFAULT);
    }
    H5E_END_TRY
    VERIFY(tmp_id, FAIL, "H5Topen2");

    ret = H5Gclose(group_id);
    CHECK(ret, FAIL, "H5Gclose");

    /* Open dataset */
    dset_id = H5Dopen2(file_id, MISC24_DATASET_NAME, H5P_DEFAULT);
    CHECK(dset_id, FAIL, "H5Dopen2");

    H5E_BEGIN_TRY
    {
        tmp_id = H5Gopen2(file_id, MISC24_DATASET_NAME, H5P_DEFAULT);
    }
    H5E_END_TRY
    VERIFY(tmp_id, FAIL, "H5Gopen2");

    H5E_BEGIN_TRY
    {
        tmp_id = H5Gopen2(file_id, MISC24_DATASET_LINK, H5P_DEFAULT);
    }
    H5E_END_TRY
    VERIFY(tmp_id, FAIL, "H5Gopen2");

    H5E_BEGIN_TRY
    {
        tmp_id = H5Topen2(file_id, MISC24_DATASET_NAME, H5P_DEFAULT);
    }
    H5E_END_TRY
    VERIFY(tmp_id, FAIL, "H5Topen2");

    H5E_BEGIN_TRY
    {
        tmp_id = H5Topen2(file_id, MISC24_DATASET_LINK, H5P_DEFAULT);
    }
    H5E_END_TRY
    VERIFY(tmp_id, FAIL, "H5Topen2");

    ret = H5Dclose(dset_id);
    CHECK(ret, FAIL, "H5Dclose");

    /* Open named datatype */
    type_id = H5Topen2(file_id, MISC24_DATATYPE_NAME, H5P_DEFAULT);
    CHECK(ret, FAIL, "H5Topen2");

    H5E_BEGIN_TRY
    {
        tmp_id = H5Gopen2(file_id, MISC24_DATATYPE_NAME, H5P_DEFAULT);
    }
    H5E_END_TRY
    VERIFY(tmp_id, FAIL, "H5Gopen2");

    H5E_BEGIN_TRY
    {
        tmp_id = H5Gopen2(file_id, MISC24_DATATYPE_LINK, H5P_DEFAULT);
    }
    H5E_END_TRY
    VERIFY(tmp_id, FAIL, "H5Gopen2");

    H5E_BEGIN_TRY
    {
        tmp_id = H5Dopen2(file_id, MISC24_DATATYPE_NAME, H5P_DEFAULT);
    }
    H5E_END_TRY
    VERIFY(tmp_id, FAIL, "H5Dopen2");

    H5E_BEGIN_TRY
    {
        tmp_id = H5Dopen2(file_id, MISC24_DATATYPE_LINK, H5P_DEFAULT);
    }
    H5E_END_TRY
    VERIFY(tmp_id, FAIL, "H5Dopen2");

    ret = H5Tclose(type_id);
    CHECK(ret, FAIL, "H5Tclose");

    /* Close file */
    ret = H5Fclose(file_id);
    CHECK(ret, FAIL, "H5Fclose");
} /* end test_misc24() */

/****************************************************************
**
**  test_misc25a(): Exercise null object header message merge bug
**                      with new file
**
****************************************************************/
static void
test_misc25a(void)
{
    hid_t  fid;             /* File ID */
    hid_t  gid, gid2, gid3; /* Group IDs */
    hid_t  aid;             /* Attribute ID */
    hid_t  sid;             /* Dataspace ID */
    hid_t  tid;             /* Datatype ID */
    herr_t ret;             /* Generic return value */

    /* Output message about test being performed */
    MESSAGE(5, ("Exercise null object header message bug\n"));

    /* Create file */
    fid = H5Fcreate(MISC25A_FILE, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
    CHECK(fid, FAIL, "H5Fcreate");

    /* Create top group */
    gid = H5Gcreate2(fid, MISC25A_GROUP0_NAME, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
    CHECK(gid, FAIL, "H5Gcreate2");

    /* Close top group */
    ret = H5Gclose(gid);
    CHECK(ret, FAIL, "H5Gclose");

    /* Create first group */
    gid = H5Gcreate2(fid, MISC25A_GROUP1_NAME, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
    CHECK(gid, FAIL, "H5Gcreate2");

    /* Close first group */
    ret = H5Gclose(gid);
    CHECK(ret, FAIL, "H5Gclose");

    /* Create second group */
    gid2 = H5Gcreate2(fid, MISC25A_GROUP2_NAME, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
    CHECK(gid2, FAIL, "H5Gcreate2");

    /* Close second group */
    ret = H5Gclose(gid2);
    CHECK(ret, FAIL, "H5Gclose");

    /* Close file */
    ret = H5Fclose(fid);
    CHECK(ret, FAIL, "H5Fclose");

    /* Re-open file */
    fid = H5Fopen(MISC25A_FILE, H5F_ACC_RDWR, H5P_DEFAULT);
    CHECK(fid, FAIL, "H5Fopen");

    /* Re-open first group */
    gid = H5Gopen2(fid, MISC25A_GROUP1_NAME, H5P_DEFAULT);
    CHECK(gid, FAIL, "H5Gopen2");

    /* Create dataspace for attribute */
    sid = H5Screate(H5S_SCALAR);
    CHECK(sid, FAIL, "H5Screate");

    /* Create datatype for attribute */
    tid = H5Tcopy(H5T_C_S1);
    CHECK(tid, FAIL, "H5Tcopy");
    ret = H5Tset_size(tid, (size_t)MISC25A_ATTR1_LEN);
    CHECK(ret, FAIL, "H5Tset_size");

    /* Add 1st attribute on first group */
    aid = H5Acreate2(gid, MISC25A_ATTR1_NAME, tid, sid, H5P_DEFAULT, H5P_DEFAULT);
    CHECK(aid, FAIL, "H5Acreate2");

    /* Close dataspace */
    ret = H5Sclose(sid);
    CHECK(ret, FAIL, "H5Sclose");

    /* Close datatype */
    ret = H5Tclose(tid);
    CHECK(ret, FAIL, "H5Tclose");

    /* Close attribute */
    ret = H5Aclose(aid);
    CHECK(ret, FAIL, "H5Aclose");

    /* Create dataspace for 2nd attribute */
    sid = H5Screate(H5S_SCALAR);
    CHECK(sid, FAIL, "H5Screate");

    /* Create datatype for attribute */
    tid = H5Tcopy(H5T_C_S1);
    CHECK(tid, FAIL, "H5Tcopy");
    ret = H5Tset_size(tid, (size_t)MISC25A_ATTR2_LEN);
    CHECK(ret, FAIL, "H5Tset_size");

    /* Add 2nd attribute on first group */
    aid = H5Acreate2(gid, MISC25A_ATTR2_NAME, tid, sid, H5P_DEFAULT, H5P_DEFAULT);
    CHECK(aid, FAIL, "H5Acreate2");

    /* Close dataspace */
    ret = H5Sclose(sid);
    CHECK(ret, FAIL, "H5Sclose");

    /* Close datatype */
    ret = H5Tclose(tid);
    CHECK(ret, FAIL, "H5Tclose");

    /* Close 2nd attribute */
    ret = H5Aclose(aid);
    CHECK(ret, FAIL, "H5Aclose");

    /* Close first group */
    ret = H5Gclose(gid);
    CHECK(ret, FAIL, "H5Gclose");

    /* Close file */
    ret = H5Fclose(fid);
    CHECK(ret, FAIL, "H5Fclose");

    /* Re-open file */
    fid = H5Fopen(MISC25A_FILE, H5F_ACC_RDWR, H5P_DEFAULT);
    CHECK(fid, FAIL, "H5Fopen");

    /* Create third group */
    gid3 = H5Gcreate2(fid, MISC25A_GROUP3_NAME, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
    CHECK(gid3, FAIL, "H5Gcreate2");

    /* Close third group */
    ret = H5Gclose(gid3);
    CHECK(ret, FAIL, "H5Gclose");

    /* Re-open first group */
    gid = H5Gopen2(fid, MISC25A_GROUP1_NAME, H5P_DEFAULT);
    CHECK(gid, FAIL, "H5Gopen2");

    /* Delete 2nd attribute */
    ret = H5Adelete(gid, MISC25A_ATTR2_NAME);
    CHECK(ret, FAIL, "H5Adelete");

    /* Close first group */
    ret = H5Gclose(gid);
    CHECK(ret, FAIL, "H5Gclose");

    /* Close file */
    ret = H5Fclose(fid);
    CHECK(ret, FAIL, "H5Fclose");

    /* Re-open file */
    fid = H5Fopen(MISC25A_FILE, H5F_ACC_RDWR, H5P_DEFAULT);
    CHECK(fid, FAIL, "H5Fopen");

    /* Re-open first group */
    gid = H5Gopen2(fid, MISC25A_GROUP1_NAME, H5P_DEFAULT);
    CHECK(gid, FAIL, "H5Gopen2");

    /* Create dataspace for 3rd attribute */
    sid = H5Screate(H5S_SCALAR);
    CHECK(sid, FAIL, "H5Screate");

    /* Create datatype for attribute */
    tid = H5Tcopy(H5T_C_S1);
    CHECK(tid, FAIL, "H5Tcopy");
    ret = H5Tset_size(tid, (size_t)MISC25A_ATTR3_LEN);
    CHECK(ret, FAIL, "H5Tset_size");

    /* Add 3rd attribute on first group (smaller than 2nd attribute) */
    aid = H5Acreate2(gid, MISC25A_ATTR3_NAME, tid, sid, H5P_DEFAULT, H5P_DEFAULT);
    CHECK(aid, FAIL, "H5Acreate2");

    /* Close dataspace */
    ret = H5Sclose(sid);
    CHECK(ret, FAIL, "H5Sclose");

    /* Close datatype */
    ret = H5Tclose(tid);
    CHECK(ret, FAIL, "H5Tclose");

    /* Close 3rd attribute */
    ret = H5Aclose(aid);
    CHECK(ret, FAIL, "H5Aclose");

    /* Close first group */
    ret = H5Gclose(gid);
    CHECK(ret, FAIL, "H5Gclose");

    /* Close file */
    ret = H5Fclose(fid);
    CHECK(ret, FAIL, "H5Fclose");

    /* Re-open file */
    fid = H5Fopen(MISC25A_FILE, H5F_ACC_RDWR, H5P_DEFAULT);
    CHECK(fid, FAIL, "H5Fopen");

    /* Re-open first group */
    gid = H5Gopen2(fid, MISC25A_GROUP1_NAME, H5P_DEFAULT);
    CHECK(gid, FAIL, "H5Gopen2");

    /* Delete 3rd attribute */
    ret = H5Adelete(gid, MISC25A_ATTR3_NAME);
    CHECK(ret, FAIL, "H5Adelete");

    /* Create dataspace for 3rd attribute */
    sid = H5Screate(H5S_SCALAR);
    CHECK(sid, FAIL, "H5Screate");

    /* Create datatype for attribute */
    tid = H5Tcopy(H5T_C_S1);
    CHECK(tid, FAIL, "H5Tcopy");
    ret = H5Tset_size(tid, (size_t)MISC25A_ATTR2_LEN);
    CHECK(ret, FAIL, "H5Tset_size");

    /* Re-create 2nd attribute on first group */
    aid = H5Acreate2(gid, MISC25A_ATTR2_NAME, tid, sid, H5P_DEFAULT, H5P_DEFAULT);
    CHECK(aid, FAIL, "H5Acreate2");

    /* Close dataspace */
    ret = H5Sclose(sid);
    CHECK(ret, FAIL, "H5Sclose");

    /* Close datatype */
    ret = H5Tclose(tid);
    CHECK(ret, FAIL, "H5Tclose");

    /* Close 2nd attribute */
    ret = H5Aclose(aid);
    CHECK(ret, FAIL, "H5Aclose");

    /* Close first group */
    ret = H5Gclose(gid);
    CHECK(ret, FAIL, "H5Gclose");

    /* Close file */
    ret = H5Fclose(fid);
    CHECK(ret, FAIL, "H5Fclose");

    /* Re-open file */
    fid = H5Fopen(MISC25A_FILE, H5F_ACC_RDWR, H5P_DEFAULT);
    CHECK(fid, FAIL, "H5Fopen");

    /* Re-open first group */
    gid = H5Gopen2(fid, MISC25A_GROUP1_NAME, H5P_DEFAULT);
    CHECK(gid, FAIL, "H5Gopen2");

    /* Delete 2nd attribute */
    ret = H5Adelete(gid, MISC25A_ATTR2_NAME);
    CHECK(ret, FAIL, "H5Adelete");

    /* Close first group */
    ret = H5Gclose(gid);
    CHECK(ret, FAIL, "H5Gclose");

    /* Close file */
    ret = H5Fclose(fid);
    CHECK(ret, FAIL, "H5Fclose");

    /* Re-open file */
    fid = H5Fopen(MISC25A_FILE, H5F_ACC_RDWR, H5P_DEFAULT);
    CHECK(fid, FAIL, "H5Fopen");

    /* Re-open first group */
    gid = H5Gopen2(fid, MISC25A_GROUP1_NAME, H5P_DEFAULT);
    CHECK(gid, FAIL, "H5Gopen2");

    /* Create dataspace for 3rd attribute */
    sid = H5Screate(H5S_SCALAR);
    CHECK(sid, FAIL, "H5Screate");

    /* Create datatype for attribute */
    tid = H5Tcopy(H5T_C_S1);
    CHECK(tid, FAIL, "H5Tcopy");
    ret = H5Tset_size(tid, (size_t)MISC25A_ATTR2_LEN);
    CHECK(ret, FAIL, "H5Tset_size");

    /* Re-create 2nd attribute on first group */
    aid = H5Acreate2(gid, MISC25A_ATTR2_NAME, tid, sid, H5P_DEFAULT, H5P_DEFAULT);
    CHECK(aid, FAIL, "H5Acreate2");

    /* Close dataspace */
    ret = H5Sclose(sid);
    CHECK(ret, FAIL, "H5Sclose");

    /* Close datatype */
    ret = H5Tclose(tid);
    CHECK(ret, FAIL, "H5Tclose");

    /* Close 2nd attribute */
    ret = H5Aclose(aid);
    CHECK(ret, FAIL, "H5Aclose");

    /* Close first group */
    ret = H5Gclose(gid);
    CHECK(ret, FAIL, "H5Gclose");

    /* Close file */
    ret = H5Fclose(fid);
    CHECK(ret, FAIL, "H5Fclose");
} /* end test_misc25a() */

/****************************************************************
**
**  test_misc25b(): Exercise null object header message merge bug
**                      with existing file  (This test relies on
**                      the file produced by test/gen_mergemsg.c)
**
****************************************************************/
static void
test_misc25b(void)
{
    hid_t       fid;                                             /* File ID */
    hid_t       gid;                                             /* Group ID */
    const char *testfile = H5_get_srcdir_filename(MISC25B_FILE); /* Corrected test file name */
    bool        vol_is_native;
    bool        driver_is_default_compatible;
    herr_t      ret; /* Generic return value */

    /* Output message about test being performed */
    MESSAGE(5, ("Exercise null object header message bug\n"));

    /* Check if native VOL is being used */
    CHECK(h5_using_native_vol(H5P_DEFAULT, H5I_INVALID_HID, &vol_is_native), FAIL, "h5_using_native_vol");
    if (!vol_is_native) {
        MESSAGE(5, (" -- SKIPPED --\n"));
        return;
    }

    ret = h5_driver_is_default_vfd_compatible(H5P_DEFAULT, &driver_is_default_compatible);
    CHECK(ret, FAIL, "h5_driver_is_default_vfd_compatible");

    if (!driver_is_default_compatible) {
        MESSAGE(5, (" -- SKIPPED --\n"));
        return;
    }

    /* Open file */
    fid = H5Fopen(testfile, H5F_ACC_RDONLY, H5P_DEFAULT);
    CHECK(fid, FAIL, "H5Fopen");

    /* Re-open group with object header messages that will merge */
    gid = H5Gopen2(fid, MISC25B_GROUP, H5P_DEFAULT);
    CHECK(gid, FAIL, "H5Gopen2");

    /* Close first group */
    ret = H5Gclose(gid);
    CHECK(ret, FAIL, "H5Gclose");

    /* Close file */
    ret = H5Fclose(fid);
    CHECK(ret, FAIL, "H5Fclose");
} /* end test_misc25b() */

/****************************************************************
**
**  test_misc25c(): Exercise another null object header message merge bug.
**
****************************************************************/
static void
test_misc25c(void)
{
    hid_t  fid;  /* File ID */
    hid_t  fapl; /* File access property list ID */
    hid_t  gcpl; /* Group creation property list ID */
    hid_t  sid;  /* Dataspace ID */
    hid_t  did;  /* Dataset ID */
    hid_t  gid;  /* Group ID */
    hid_t  gid2; /* Group ID */
    hid_t  aid;  /* Attribute ID */
    herr_t ret;  /* Generic return value */

    /* Output message about test being performed */
    MESSAGE(5, ("Exercise another null object header message bug\n"));

    /* Compose file access property list */
    fapl = H5Pcreate(H5P_FILE_ACCESS);
    CHECK(fapl, FAIL, "H5Pcreate");
    ret = H5Pset_libver_bounds(fapl, H5F_LIBVER_LATEST, H5F_LIBVER_LATEST);
    CHECK(ret, FAIL, "H5Pset_libver_bounds");

    /* Create the file */
    fid = H5Fcreate(MISC25C_FILE, H5F_ACC_TRUNC, H5P_DEFAULT, fapl);
    CHECK(fid, FAIL, "H5Fcreate");

    /* Compose group creation property list */
    gcpl = H5Pcreate(H5P_GROUP_CREATE);
    CHECK(gcpl, FAIL, "H5Pcreate");
    ret = H5Pset_link_creation_order(gcpl, (H5P_CRT_ORDER_TRACKED | H5P_CRT_ORDER_INDEXED));
    CHECK(ret, FAIL, "H5Pset_link_creation_order");
    ret = H5Pset_attr_creation_order(gcpl, (H5P_CRT_ORDER_TRACKED | H5P_CRT_ORDER_INDEXED));
    CHECK(ret, FAIL, "H5Pset_attr_creation_order");
    ret = H5Pset_est_link_info(gcpl, 1, 18);
    CHECK(ret, FAIL, "H5Pset_est_link_info");

    /* Create a group for the dataset */
    gid = H5Gcreate2(fid, MISC25C_DSETGRPNAME, H5P_DEFAULT, gcpl, H5P_DEFAULT);
    CHECK(gid, FAIL, "H5Gcreate2");

    /* Create the dataspace */
    sid = H5Screate(H5S_SCALAR);
    CHECK(sid, FAIL, "H5Screate");

    /* Create the dataset */
    did = H5Dcreate2(gid, MISC25C_DSETNAME, H5T_NATIVE_INT, sid, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
    CHECK(did, FAIL, "H5Dcreate2");

    /* Create an extra group */
    gid2 = H5Gcreate2(fid, MISC25C_GRPNAME, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
    CHECK(gid2, FAIL, "H5Gcreate2");

    /* Close the extra group */
    ret = H5Gclose(gid2);
    CHECK(ret, FAIL, "H5Gclose");

    /* Add an attribute to the dataset group */
    aid = H5Acreate2(gid, MISC25C_ATTRNAME, H5T_NATIVE_CHAR, sid, H5P_DEFAULT, H5P_DEFAULT);
    CHECK(aid, FAIL, "H5Acreate2");

    /* Close the attribute */
    ret = H5Aclose(aid);
    CHECK(ret, FAIL, "H5Aclose");

    /* Create a second extra group */
    gid2 = H5Gcreate2(fid, MISC25C_GRPNAME2, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
    CHECK(gid2, FAIL, "H5Gcreate2");

    /* Close the second extra group */
    ret = H5Gclose(gid2);
    CHECK(ret, FAIL, "H5Gclose");

    /* Add second attribute to the dataset group */
    aid = H5Acreate2(gid, MISC25C_ATTRNAME2, H5T_NATIVE_INT, sid, H5P_DEFAULT, H5P_DEFAULT);
    CHECK(aid, FAIL, "H5Acreate2");

    /* Close the attribute */
    ret = H5Aclose(aid);
    CHECK(ret, FAIL, "H5Aclose");

    /* Close the dataset */
    ret = H5Dclose(did);
    CHECK(ret, FAIL, "H5Dclose");

    /* Close the dataset group */
    ret = H5Gclose(gid);
    CHECK(ret, FAIL, "H5Gclose");

    /* Close the dataspace */
    ret = H5Sclose(sid);
    CHECK(ret, FAIL, "H5Sclose");

    /* Close the file */
    ret = H5Fclose(fid);
    CHECK(ret, FAIL, "H5Fclose");

    /* Close the property lists */
    ret = H5Pclose(fapl);
    CHECK(ret, FAIL, "H5Pclose");
    ret = H5Pclose(gcpl);
    CHECK(ret, FAIL, "H5Pclose");

    /* Re-open the file */
    fid = H5Fopen(MISC25C_FILE, H5F_ACC_RDWR, H5P_DEFAULT);
    CHECK(fid, FAIL, "H5Fopen");

    /* Re-open the dataset group */
    gid = H5Gopen2(fid, MISC25C_DSETGRPNAME, H5P_DEFAULT);
    CHECK(gid, FAIL, "H5Gopen2");

    /* Rename the dataset */
    ret = H5Lmove(gid, MISC25C_DSETNAME, H5L_SAME_LOC, MISC25C_DSETNAME2, H5P_DEFAULT, H5P_DEFAULT);
    CHECK(ret, FAIL, "H5Lmove");

    /* Delete the first attribute */
    ret = H5Adelete(gid, MISC25C_ATTRNAME);
    CHECK(ret, FAIL, "H5Adelete");

    /* Close the dataset group */
    ret = H5Gclose(gid);
    CHECK(ret, FAIL, "H5Gclose");

    /* Close the file */
    ret = H5Fclose(fid);
    CHECK(ret, FAIL, "H5Fclose");
} /* end test_misc25c() */

/****************************************************************
**
**  test_misc26(): Regression test: ensure that copying filter
**                      pipelines works properly.
**
****************************************************************/
static void
test_misc26(void)
{
    hid_t   fid;                 /* File ID */
    hid_t   sid;                 /* Dataspace ID */
    hid_t   did;                 /* Dataset ID */
    hid_t   dcpl1, dcpl2, dcpl3; /* Property List IDs */
    hsize_t dims[] = {1};
    herr_t  ret; /* Generic return value */

    /* Output message about test being performed */
    MESSAGE(5, ("Copying filter pipelines\n"));

    /* Create the property list.  It needs chunking so we can add filters */
    dcpl1 = H5Pcreate(H5P_DATASET_CREATE);
    CHECK_I(dcpl1, "H5Pcreate");
    ret = H5Pset_chunk(dcpl1, 1, dims);
    CHECK_I(ret, "H5Pset_chunk");

    /* Add a filter with a data value to the property list */
    ret = H5Pset_deflate(dcpl1, 1);
    CHECK_I(ret, "H5Pset_deflate");

    /* Copy the property list */
    dcpl2 = H5Pcopy(dcpl1);
    CHECK_I(dcpl2, "H5Pcopy");

    /* Add a filter with no data values to the copy */
    ret = H5Pset_shuffle(dcpl2);
    CHECK_I(ret, "H5Pset_shuffle");

    /* Copy the copy */
    dcpl3 = H5Pcopy(dcpl2);
    CHECK_I(dcpl3, "H5Pcopy");

    /* Add another filter */
    ret = H5Pset_deflate(dcpl3, 2);
    CHECK_I(ret, "H5Pset_deflate");

    /* Create a new file and datasets within that file that use these
     * property lists
     */
    fid = H5Fcreate(MISC26_FILE, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
    CHECK(fid, FAIL, "H5Fcreate");

    sid = H5Screate_simple(1, dims, dims);
    CHECK(sid, FAIL, "H5Screate_simple");

    did = H5Dcreate2(fid, "dataset1", H5T_NATIVE_FLOAT, sid, H5P_DEFAULT, dcpl1, H5P_DEFAULT);
    CHECK(did, FAIL, "H5Dcreate2");
    ret = H5Dclose(did);
    CHECK_I(ret, "H5Dclose");

    did = H5Dcreate2(fid, "dataset2", H5T_NATIVE_FLOAT, sid, H5P_DEFAULT, dcpl2, H5P_DEFAULT);
    CHECK(did, FAIL, "H5Dcreate2");
    ret = H5Dclose(did);
    CHECK_I(ret, "H5Dclose");

    did = H5Dcreate2(fid, "dataset3", H5T_NATIVE_FLOAT, sid, H5P_DEFAULT, dcpl3, H5P_DEFAULT);
    CHECK(did, FAIL, "H5Dcreate2");
    ret = H5Dclose(did);
    CHECK_I(ret, "H5Dclose");

    /* Close the dataspace and file */
    ret = H5Sclose(sid);
    CHECK_I(ret, "H5Sclose");
    ret = H5Fclose(fid);
    CHECK_I(ret, "H5Fclose");

    /* Close the property lists.  */
    ret = H5Pclose(dcpl1);
    CHECK_I(ret, "H5Pclose");
    ret = H5Pclose(dcpl2);
    CHECK_I(ret, "H5Pclose");
    ret = H5Pclose(dcpl3);
    CHECK_I(ret, "H5Pclose");
}

/****************************************************************
**
**  test_misc27(): Ensure that objects with incorrect # of object
**                      header messages are handled appropriately.
**
** (Note that this test file is generated by the "gen_bad_ohdr.c" code)
**
****************************************************************/
static void
test_misc27(void)
{
    hid_t       fid;                                            /* File ID */
    hid_t       gid;                                            /* Group ID */
    const char *testfile = H5_get_srcdir_filename(MISC27_FILE); /* Corrected test file name */
    bool        vol_is_native;
    bool        driver_is_default_compatible;
    herr_t      ret; /* Generic return value */

    /* Output message about test being performed */
    MESSAGE(5, ("Corrupt object header handling\n"));

    /* Check if native VOL is being used */
    CHECK(h5_using_native_vol(H5P_DEFAULT, H5I_INVALID_HID, &vol_is_native), FAIL, "h5_using_native_vol");
    if (!vol_is_native) {
        MESSAGE(5, (" -- SKIPPED --\n"));
        return;
    }

    ret = h5_driver_is_default_vfd_compatible(H5P_DEFAULT, &driver_is_default_compatible);
    CHECK(ret, FAIL, "h5_driver_is_default_vfd_compatible");

    if (!driver_is_default_compatible) {
        MESSAGE(5, (" -- SKIPPED --\n"));
        return;
    }

    /* Open the file */
    fid = H5Fopen(testfile, H5F_ACC_RDONLY, H5P_DEFAULT);
    CHECK(fid, FAIL, "H5Fopen");

#ifdef H5_STRICT_FORMAT_CHECKS
    /* Open group with incorrect # of object header messages (should fail) */
    H5E_BEGIN_TRY
    {
        gid = H5Gopen2(fid, MISC27_GROUP, H5P_DEFAULT);
    }
    H5E_END_TRY
    VERIFY(gid, FAIL, "H5Gopen2");
#else  /* H5_STRICT_FORMAT_CHECKS */
    /* Open group with incorrect # of object header messages */
    gid = H5Gopen2(fid, MISC27_GROUP, H5P_DEFAULT);
    CHECK(gid, FAIL, "H5Gopen2");

    /* Close group */
    ret = H5Gclose(gid);
    CHECK(ret, FAIL, "H5Gclose");
#endif /* H5_STRICT_FORMAT_CHECKS */

    /* Close file */
    ret = H5Fclose(fid);
    CHECK(ret, FAIL, "H5Fclose");
} /* end test_misc27() */

/****************************************************************
**
**  test_misc28(): Ensure that the dataset chunk cache will hold
**                 the correct number of chunks in cache without
**                 evicting them.
**
****************************************************************/
static void
test_misc28(void)
{
    hid_t   fid;        /* File ID */
    hid_t   sidf;       /* File Dataspace ID */
    hid_t   sidm;       /* Memory Dataspace ID */
    hid_t   did;        /* Dataset ID */
    hid_t   dcpl, fapl; /* Property List IDs */
    hsize_t dims[]  = {MISC28_SIZE, MISC28_SIZE};
    hsize_t mdims[] = {MISC28_SIZE};
    hsize_t cdims[] = {1, 1};
    hsize_t start[] = {0, 0};
    hsize_t count[] = {MISC28_SIZE, 1};
    size_t  nbytes_used;
    int     nused;
    char    buf[MISC28_SIZE];
    int     i;
    bool    vol_is_native;
    herr_t  ret; /* Generic return value */

    /* Output message about test being performed */
    MESSAGE(5, ("Dataset chunk cache\n"));

    /* Create the fapl and set the cache size.  Set nelmts to larger than the
     * file size so we can be guaranteed that no chunks will be evicted due to
     * a hash collision.  Set nbytes to fit exactly 1 column of chunks (10
     * bytes). */
    fapl = H5Pcreate(H5P_FILE_ACCESS);
    CHECK(fapl, FAIL, "H5Pcreate");
    ret = H5Pset_cache(fapl, MISC28_NSLOTS, MISC28_NSLOTS, MISC28_SIZE, 0.75);
    CHECK(ret, FAIL, "H5Pset_cache");

    /* Create the dcpl and set the chunk size */
    dcpl = H5Pcreate(H5P_DATASET_CREATE);
    CHECK(dcpl, FAIL, "H5Pcreate");
    ret = H5Pset_chunk(dcpl, 2, cdims);
    CHECK(ret, FAIL, "H5Pset_chunk");

    /* Create a new file and datasets within that file that use these
     * property lists
     */
    fid = H5Fcreate(MISC28_FILE, H5F_ACC_TRUNC, H5P_DEFAULT, fapl);
    CHECK(fid, FAIL, "H5Fcreate");

    /* Check if native VOL is being used */
    CHECK(h5_using_native_vol(fapl, fid, &vol_is_native), FAIL, "h5_using_native_vol");

    sidf = H5Screate_simple(2, dims, NULL);
    CHECK(sidf, FAIL, "H5Screate_simple");

    did = H5Dcreate2(fid, "dataset", H5T_NATIVE_CHAR, sidf, H5P_DEFAULT, dcpl, H5P_DEFAULT);
    CHECK(did, FAIL, "H5Dcreate2");

    if (vol_is_native) {
        /* Verify that the chunk cache is empty */
        ret = H5D__current_cache_size_test(did, &nbytes_used, &nused);
        CHECK(ret, FAIL, "H5D__current_cache_size_test");
        VERIFY(nbytes_used, (size_t)0, "H5D__current_cache_size_test");
        VERIFY(nused, 0, "H5D__current_cache_size_test");
    }

    /* Initialize write buffer */
    for (i = 0; i < MISC28_SIZE; i++)
        buf[i] = (char)i;

    /* Create memory dataspace and selection in file dataspace */
    sidm = H5Screate_simple(1, mdims, NULL);
    CHECK(sidm, FAIL, "H5Screate_simple");

    ret = H5Sselect_hyperslab(sidf, H5S_SELECT_SET, start, NULL, count, NULL);
    CHECK(ret, FAIL, "H5Sselect_hyperslab");

    /* Write hypserslab */
    ret = H5Dwrite(did, H5T_NATIVE_CHAR, sidm, sidf, H5P_DEFAULT, buf);
    CHECK(ret, FAIL, "H5Dwrite");

    if (vol_is_native) {
        /* Verify that all 10 chunks written have been cached */
        ret = H5D__current_cache_size_test(did, &nbytes_used, &nused);
        CHECK(ret, FAIL, "H5D__current_cache_size_test");
        VERIFY(nbytes_used, (size_t)MISC28_SIZE, "H5D__current_cache_size_test");
        VERIFY(nused, MISC28_SIZE, "H5D__current_cache_size_test");
    }

    /* Initialize write buffer */
    for (i = 0; i < MISC28_SIZE; i++)
        buf[i] = (char)(MISC28_SIZE - 1 - i);

    /* Select new hyperslab */
    start[1] = 1;
    ret      = H5Sselect_hyperslab(sidf, H5S_SELECT_SET, start, NULL, count, NULL);
    CHECK(ret, FAIL, "H5Sselect_hyperslab");

    /* Write hyperslab */
    ret = H5Dwrite(did, H5T_NATIVE_CHAR, sidm, sidf, H5P_DEFAULT, buf);
    CHECK(ret, FAIL, "H5Dwrite");

    if (vol_is_native) {
        /* Verify that the size of the cache remains at 10 */
        ret = H5D__current_cache_size_test(did, &nbytes_used, &nused);
        CHECK(ret, FAIL, "H5D__current_cache_size_test");
        VERIFY(nbytes_used, (size_t)MISC28_SIZE, "H5D__current_cache_size_test");
        VERIFY(nused, MISC28_SIZE, "H5D__current_cache_size_test");
    }

    /* Close dataset */
    ret = H5Dclose(did);
    CHECK(ret, FAIL, "H5Dclose");

    /* Re open dataset */
    did = H5Dopen2(fid, "dataset", H5P_DEFAULT);
    CHECK(did, FAIL, "H5Dopen2");

    if (vol_is_native) {
        /* Verify that the chunk cache is empty */
        ret = H5D__current_cache_size_test(did, &nbytes_used, &nused);
        CHECK(ret, FAIL, "H5D__current_cache_size_test");
        VERIFY(nbytes_used, (size_t)0, "H5D__current_cache_size_test");
        VERIFY(nused, 0, "H5D__current_cache_size_test");
    }

    /* Select hyperslabe for reading */
    start[1] = 0;
    ret      = H5Sselect_hyperslab(sidf, H5S_SELECT_SET, start, NULL, count, NULL);
    CHECK(ret, FAIL, "H5Sselect_hyperslab");

    /* Read hypserslab */
    ret = H5Dread(did, H5T_NATIVE_CHAR, sidm, sidf, H5P_DEFAULT, buf);
    CHECK(ret, FAIL, "H5Dread");

    /* Verify the data read */
    for (i = 0; i < MISC28_SIZE; i++)
        VERIFY(buf[i], i, "H5Dread");

    if (vol_is_native) {
        /* Verify that all 10 chunks read have been cached */
        ret = H5D__current_cache_size_test(did, &nbytes_used, &nused);
        CHECK(ret, FAIL, "H5D__current_cache_size_test");
        VERIFY(nbytes_used, (size_t)MISC28_SIZE, "H5D__current_cache_size_test");
        VERIFY(nused, MISC28_SIZE, "H5D__current_cache_size_test");
    }

    /* Select new hyperslab */
    start[1] = 1;
    ret      = H5Sselect_hyperslab(sidf, H5S_SELECT_SET, start, NULL, count, NULL);
    CHECK(ret, FAIL, "H5Sselect_hyperslab");

    /* Read hyperslab */
    ret = H5Dread(did, H5T_NATIVE_CHAR, sidm, sidf, H5P_DEFAULT, buf);
    CHECK(ret, FAIL, "H5Dread");

    /* Verify the data read */
    for (i = 0; i < MISC28_SIZE; i++)
        VERIFY(buf[i], MISC28_SIZE - 1 - i, "H5Dread");

    if (vol_is_native) {
        /* Verify that the size of the cache remains at 10 */
        ret = H5D__current_cache_size_test(did, &nbytes_used, &nused);
        CHECK(ret, FAIL, "H5D__current_cache_size_test");
        VERIFY(nbytes_used, (size_t)MISC28_SIZE, "H5D__current_cache_size_test");
        VERIFY(nused, MISC28_SIZE, "H5D__current_cache_size_test");
    }

    /* Close dataset */
    ret = H5Dclose(did);
    CHECK(ret, FAIL, "H5Dclose");

    /* Close the dataspaces and file */
    ret = H5Sclose(sidf);
    CHECK_I(ret, "H5Sclose");
    ret = H5Sclose(sidm);
    CHECK_I(ret, "H5Sclose");
    ret = H5Fclose(fid);
    CHECK_I(ret, "H5Fclose");

    /* Close the property lists.  */
    ret = H5Pclose(dcpl);
    CHECK_I(ret, "H5Pclose");
    ret = H5Pclose(fapl);
    CHECK_I(ret, "H5Pclose");
} /* end test_misc28() */

/****************************************************************
**
**  test_misc29(): Ensure that speculative metadata reads don't
**                 get raw data into the metadata accumulator.
**
****************************************************************/
static void
test_misc29(void)
{
    bool   vol_is_native;
    bool   driver_is_default_compatible;
    hid_t  fid; /* File ID */
    herr_t ret; /* Generic return value */

    /* Output message about test being performed */
    MESSAGE(5, ("Speculative metadata reads\n"));

    /* Check if native VOL is being used */
    CHECK(h5_using_native_vol(H5P_DEFAULT, H5I_INVALID_HID, &vol_is_native), FAIL, "h5_using_native_vol");
    if (!vol_is_native) {
        MESSAGE(5, (" -- SKIPPED --\n"));
        return;
    }

    ret = h5_driver_is_default_vfd_compatible(H5P_DEFAULT, &driver_is_default_compatible);
    CHECK(ret, FAIL, "h5_driver_is_default_vfd_compatible");

    if (!driver_is_default_compatible) {
        MESSAGE(5, (" -- SKIPPED --\n"));
        return;
    }

    /* Make a copy of the data file from svn. */
    ret = h5_make_local_copy(MISC29_ORIG_FILE, MISC29_COPY_FILE);
    CHECK(ret, -1, "h5_make_local_copy");

    /* Open the copied file */
    fid = H5Fopen(MISC29_COPY_FILE, H5F_ACC_RDWR, H5P_DEFAULT);
    CHECK(fid, FAIL, "H5Fopen");

    /* Delete the last dataset */
    ret = H5Ldelete(fid, MISC29_DSETNAME, H5P_DEFAULT);
    CHECK(ret, FAIL, "H5Ldelete");

    /* Close the file */
    ret = H5Fclose(fid);
    CHECK(ret, FAIL, "H5Fclose");
} /* end test_misc29() */

static int
test_misc30_get_info_cb(hid_t loc_id, const char *name, const H5L_info2_t H5_ATTR_UNUSED *info,
                        void H5_ATTR_UNUSED *op_data)
{
    H5O_info2_t object_info;

    return H5Oget_info_by_name3(loc_id, name, &object_info, H5O_INFO_BASIC, H5P_DEFAULT);
}

static int
test_misc30_get_info(hid_t loc_id)
{
    return H5Literate2(loc_id, H5_INDEX_NAME, H5_ITER_INC, NULL, test_misc30_get_info_cb, NULL);
}

/****************************************************************
**
**  test_misc30(): Exercise local heap code that loads prefix
**                 separately from data block, causing the free
**                 block information to get lost.
**
****************************************************************/
static void
test_misc30(void)
{
    hsize_t  file_size[] = {0, 0}; /* Sizes of file created */
    unsigned get_info;             /* Whether to perform the get info call */
    bool     vol_is_native;

    /* Output message about test being performed */
    MESSAGE(5, ("Local heap dropping free block info\n"));

    /* Check if native VOL is being used */
    CHECK(h5_using_native_vol(H5P_DEFAULT, H5I_INVALID_HID, &vol_is_native), FAIL, "h5_using_native_vol");
    if (!vol_is_native) {
        MESSAGE(5, (" -- SKIPPED --\n"));
        return;
    }

    for (get_info = false; get_info <= true; get_info++) {
        hid_t  fid; /* File ID */
        hid_t  gid; /* Group ID */
        int    i;   /* Local index counter */
        herr_t ret; /* Generic return value */

        fid = H5Fcreate(MISC30_FILE, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
        CHECK(fid, FAIL, "H5Fcreate");
        gid = H5Gcreate2(fid, "/g0", H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
        CHECK(gid, FAIL, "H5Gcreate2");

        ret = H5Gclose(gid);
        CHECK(ret, FAIL, "H5Gclose");
        ret = H5Fclose(fid);
        CHECK(ret, FAIL, "H5Fclose");

        for (i = 0; i < 20; i++) {
            char gname[32];

            fid = H5Fopen(MISC30_FILE, H5F_ACC_RDWR, H5P_DEFAULT);
            CHECK(fid, FAIL, "H5Fopen");

            if (get_info) {
                ret = test_misc30_get_info(fid);
                CHECK(ret, FAIL, "test_misc30_get_info");
            }

            snprintf(gname, sizeof(gname), "/g0/group%d", i);
            gid = H5Gcreate2(fid, gname, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
            CHECK(gid, FAIL, "H5Gcreate2");

            ret = H5Gclose(gid);
            CHECK(ret, FAIL, "H5Gclose");
            ret = H5Fclose(fid);
            CHECK(ret, FAIL, "H5Fclose");
        }

        fid = H5Fopen(MISC30_FILE, H5F_ACC_RDONLY, H5P_DEFAULT);
        CHECK(fid, FAIL, "H5Fopen");
        ret = H5Fget_filesize(fid, &file_size[get_info]);
        CHECK(fid, FAIL, "H5Fget_filesize");
        ret = H5Fclose(fid);
        CHECK(ret, FAIL, "H5Fclose");
    }

    VERIFY(file_size[0], file_size[1], "test_misc30");
} /* end test_misc30() */

/****************************************************************
**
**  test_misc31(): Test reentering library through deprecated
*                  routines that register an id after calling
*                  H5close().
**
****************************************************************/
static void
test_misc31(void)
{
#ifndef H5_NO_DEPRECATED_SYMBOLS
    hid_t  file_id;  /* File id */
    hid_t  space_id; /* Dataspace id */
    hid_t  dset_id;  /* Dataset id */
    hid_t  attr_id;  /* Attribute id */
    hid_t  group_id; /* Group id */
    hid_t  dtype_id; /* Datatype id */
    bool   vol_is_native;
    herr_t ret; /* Generic return value */
#endif          /* H5_NO_DEPRECATED_SYMBOLS */

    /* Output message about test being performed */
    MESSAGE(5, ("Deprecated routines initialize after H5close()\n"));

#ifndef H5_NO_DEPRECATED_SYMBOLS
    file_id = H5Fcreate(MISC31_FILE, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
    CHECK(file_id, FAIL, "H5Fcreate");

    /* Check if native VOL is being used */
    CHECK(h5_using_native_vol(H5P_DEFAULT, file_id, &vol_is_native), FAIL, "h5_using_native_vol");
    if (!vol_is_native) {
        CHECK(H5Fclose(file_id), FAIL, "H5Fclose");
        MESSAGE(5, (" -- SKIPPED --\n"));
        return;
    }

    /* Test dataset package */
    space_id = H5Screate(H5S_SCALAR);
    CHECK(space_id, FAIL, "H5Screate");
    dset_id = H5Dcreate1(file_id, MISC31_DSETNAME, H5T_NATIVE_INT, space_id, H5P_DEFAULT);
    CHECK(dset_id, FAIL, "H5Dcreate1");
    ret = H5close();
    CHECK(ret, FAIL, "H5close");
    file_id = H5Fopen(MISC31_FILE, H5F_ACC_RDWR, H5P_DEFAULT);
    CHECK(file_id, FAIL, "H5Fopen");
    dset_id = H5Dopen1(file_id, MISC31_DSETNAME);
    CHECK(dset_id, FAIL, "H5Dopen1");

    /* Test attribute package */
    space_id = H5Screate(H5S_SCALAR);
    CHECK(space_id, FAIL, "H5Screate");
    attr_id = H5Acreate1(dset_id, MISC31_ATTRNAME1, H5T_NATIVE_INT, space_id, H5P_DEFAULT);
    CHECK(attr_id, FAIL, "H5Acreate1");
    ret = H5close();
    CHECK(ret, FAIL, "H5close");
    file_id = H5Fopen(MISC31_FILE, H5F_ACC_RDWR, H5P_DEFAULT);
    CHECK(file_id, FAIL, "H5Fopen");
    dset_id = H5Dopen1(file_id, MISC31_DSETNAME);
    CHECK(dset_id, FAIL, "H5Dopen1");
    space_id = H5Screate(H5S_SCALAR);
    CHECK(space_id, FAIL, "H5Screate");
    attr_id = H5Acreate1(dset_id, MISC31_ATTRNAME2, H5T_NATIVE_INT, space_id, H5P_DEFAULT);
    CHECK(attr_id, FAIL, "H5Acreate1");

    /* Test group package */
    group_id = H5Gcreate1(file_id, MISC31_GROUPNAME, 0);
    CHECK(group_id, FAIL, "H5Gcreate1");
    ret = H5close();
    CHECK(ret, FAIL, "H5close");
    file_id = H5Fopen(MISC31_FILE, H5F_ACC_RDWR, H5P_DEFAULT);
    CHECK(file_id, FAIL, "H5Fopen");
    group_id = H5Gopen1(file_id, MISC31_GROUPNAME);
    CHECK(group_id, FAIL, "H5Gopen1");

    /* Test property list package */
    ret = H5Pregister1(H5P_OBJECT_CREATE, MISC31_PROPNAME, 0, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
    CHECK(ret, FAIL, "H5Pregister1");
    ret = H5close();
    CHECK(ret, FAIL, "H5close");
    ret = H5Pregister1(H5P_OBJECT_CREATE, MISC31_PROPNAME, 0, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
    CHECK(ret, FAIL, "H5Pregister1");
    ret = H5close();
    CHECK(ret, FAIL, "H5close");

    /* Test datatype package */
    file_id = H5Fopen(MISC31_FILE, H5F_ACC_RDWR, H5P_DEFAULT);
    CHECK(file_id, FAIL, "H5Fopen");
    dtype_id = H5Tcopy(H5T_NATIVE_INT);
    CHECK(dtype_id, FAIL, "H5Tcopy");
    ret = H5Tcommit1(file_id, MISC31_DTYPENAME, dtype_id);
    CHECK(ret, FAIL, "H5Tcommit1");
    ret = H5close();
    CHECK(ret, FAIL, "H5close");
    file_id = H5Fopen(MISC31_FILE, H5F_ACC_RDWR, H5P_DEFAULT);
    CHECK(file_id, FAIL, "H5Fopen");
    dtype_id = H5Topen1(file_id, MISC31_DTYPENAME);
    CHECK(ret, FAIL, "H5Topen1");
    ret = H5Fclose(file_id);
    CHECK(ret, FAIL, "H5Fclose");
    ret = H5Tclose(dtype_id);
    CHECK(ret, FAIL, "H5Tclose");

#else  /* H5_NO_DEPRECATED_SYMBOLS */
    /* Output message about test being skipped */
    MESSAGE(5, (" ...Skipped"));
#endif /* H5_NO_DEPRECATED_SYMBOLS */
} /* end test_misc31() */

/****************************************************************
 *
 *  test_misc32(): Simple test of filter memory allocation
 *                 functions.
 *
 ***************************************************************/
static void
test_misc32(void)
{
    void  *buffer;
    void  *resized;
    size_t size;

    /* Output message about test being performed */
    MESSAGE(5, ("Edge case test of filter memory allocation functions\n"));

    /* Test that the filter memory allocation functions behave correctly
     * at edge cases.
     */

    /* FREE */

    /* Test freeing a NULL pointer.
     * No real confirmation check here, but Valgrind will confirm no
     * shenanigans.
     */
    buffer = NULL;
    H5free_memory(buffer);

    /* ALLOCATE */

    /* Size zero returns NULL.
     * Also checks that a size of zero and setting the buffer clear flag
     * to true can be used together.
     *
     * Note that we have asserts in the code, so only check when NDEBUG
     * is defined.
     */
#ifdef NDEBUG
    buffer = H5allocate_memory(0, false);
    CHECK_PTR_NULL(buffer, "H5allocate_memory"); /*BAD*/
    buffer = H5allocate_memory(0, true);
    CHECK_PTR_NULL(buffer, "H5allocate_memory"); /*BAD*/
#endif                                           /* NDEBUG */

    /* RESIZE */

    /* Size zero returns NULL. Valgrind will confirm buffer is freed. */
    size    = 1024;
    buffer  = H5allocate_memory(size, true);
    resized = H5resize_memory(buffer, 0);
    CHECK_PTR_NULL(resized, "H5resize_memory");

    /* NULL input pointer returns new buffer */
    resized = H5resize_memory(NULL, 1024);
    CHECK_PTR(resized, "H5resize_memory");
    H5free_memory(resized);

    /* NULL input pointer and size zero returns NULL */
#ifdef NDEBUG
    resized = H5resize_memory(NULL, 0);
    CHECK_PTR_NULL(resized, "H5resize_memory"); /*BAD*/
#endif                                          /* NDEBUG */

} /* end test_misc32() */

/****************************************************************
**
**  test_misc33(): Test for H5FFV-10216
**      --verify that H5HL_offset_into() returns error if the
**        input parameter "offset" exceeds heap data block size.
**      --case (1), (2), (3) are scenarios that will traverse to the
**        the 3 locations in the file having bad offset values to
**        the heap.  (See description in gen_bad_offset.c)
**
****************************************************************/
static void
test_misc33(void)
{
    hid_t       fid      = H5I_INVALID_HID;                     /* File ID */
    const char *testfile = H5_get_srcdir_filename(MISC33_FILE); /* Corrected test file name */
    H5O_info2_t oinfo; /* Structure for object metadata information */
    bool        vol_is_native;
    bool        driver_is_default_compatible;
    herr_t      ret; /* Generic return value */

    /* Output message about test being performed */
    MESSAGE(5, ("Testing that bad offset into the heap returns error"));

    /* Check if native VOL is being used */
    CHECK(h5_using_native_vol(H5P_DEFAULT, H5I_INVALID_HID, &vol_is_native), FAIL, "h5_using_native_vol");
    if (!vol_is_native) {
        MESSAGE(5, (" -- SKIPPED --\n"));
        return;
    }

    ret = h5_driver_is_default_vfd_compatible(H5P_DEFAULT, &driver_is_default_compatible);
    CHECK(ret, FAIL, "h5_driver_is_default_vfd_compatible");

    if (!driver_is_default_compatible) {
        MESSAGE(5, (" -- SKIPPED --\n"));
        return;
    }

    /* Open the test file */
    fid = H5Fopen(testfile, H5F_ACC_RDONLY, H5P_DEFAULT);
    CHECK(fid, FAIL, "H5Fopen");

    /* Case (1) */
    H5E_BEGIN_TRY
    {
        ret = H5Oget_info_by_name3(fid, "/soft_two", &oinfo, H5O_INFO_BASIC, H5P_DEFAULT);
    }
    H5E_END_TRY
    VERIFY(ret, FAIL, "H5Oget_info_by_name3");

    /* Case (2) */
    H5E_BEGIN_TRY
    {
        ret = H5Oget_info_by_name3(fid, "/dsetA", &oinfo, H5O_INFO_BASIC, H5P_DEFAULT);
    }
    H5E_END_TRY
    VERIFY(ret, FAIL, "H5Oget_info_by_name3");

    /* Case (3) */
    H5E_BEGIN_TRY
    {
        ret = H5Oget_info_by_name3(fid, "/soft_one", &oinfo, H5O_INFO_BASIC, H5P_DEFAULT);
    }
    H5E_END_TRY
    VERIFY(ret, FAIL, "H5Oget_info_by_name3");

    /* Close the file */
    ret = H5Fclose(fid);
    CHECK(fid, FAIL, "H5Fclose");

} /* end test_misc33() */

/****************************************************************
**
**  test_misc34(): Ensure zero-size memory allocations work
**
****************************************************************/
static void
test_misc34(void)
{
    void  *mem = NULL; /* allocated buffer     */
    char  *dup = NULL; /* 'duplicated' string  */
    size_t sz  = 0;    /* buffer size          */

    /* Output message about test being performed */
    MESSAGE(5, ("Testing O and NULL behavior in H5MM API calls"));

    /* H5MM_xfree(): Ensure that passing NULL is allowed and returns NULL */
    mem = H5MM_xfree(mem);
    CHECK_PTR_NULL(mem, "H5MM_xfree");

    /* H5MM_realloc(): Check behavior:
     *
     *  H5MM_realloc(NULL, size)    <==> H5MM_malloc(size)
     *  H5MM_realloc(ptr, 0)        <==> H5MM_xfree(ptr)
     *  H5MM_realloc(NULL, 0)       <==> NULL
     */
    mem = H5MM_xfree(mem);

    sz  = 1024;
    mem = H5MM_realloc(mem, sz);
    CHECK_PTR(mem, "H5MM_realloc (case 1)");
    /* Don't free mem here! */

    sz  = 0;
    mem = H5MM_realloc(mem, sz);
    CHECK_PTR_NULL(mem, "H5MM_realloc (case 2)");
    mem = H5MM_xfree(mem);

    mem = H5MM_realloc(mem, sz);
    CHECK_PTR_NULL(mem, "H5MM_realloc (case 3)");
    mem = H5MM_xfree(mem);

    /* H5MM_xstrdup(): Ensure NULL returns NULL */
    dup = H5MM_xstrdup((const char *)mem);
    CHECK_PTR_NULL(dup, "H5MM_xstrdup");
    dup = (char *)H5MM_xfree((void *)dup);

} /* end test_misc34() */

/****************************************************************
**
**  test_misc35(): Check operation of free-list routines
**
****************************************************************/
static void
test_misc35(void)
{
    hid_t   sid    = H5I_INVALID_HID;                                           /* Dataspace ID */
    hsize_t dims[] = {MISC35_SPACE_DIM1, MISC35_SPACE_DIM2, MISC35_SPACE_DIM3}; /* Dataspace dims */
    hsize_t coord[MISC35_NPOINTS][MISC35_SPACE_RANK] = /* Coordinates for point selection */
        {{0, 10, 5}, {1, 2, 7},  {2, 4, 9}, {0, 6, 11}, {1, 8, 13},
         {2, 12, 0}, {0, 14, 2}, {1, 0, 4}, {2, 1, 6},  {0, 3, 8}};
    size_t reg_size_start; /* Initial amount of regular memory allocated */
    size_t arr_size_start; /* Initial amount of array memory allocated */
    size_t blk_size_start; /* Initial amount of block memory allocated */
    size_t fac_size_start; /* Initial amount of factory memory allocated */
    size_t reg_size_final; /* Final amount of regular memory allocated */
    size_t arr_size_final; /* Final amount of array memory allocated */
    size_t blk_size_final; /* Final amount of block memory allocated */
    size_t fac_size_final; /* Final amount of factory memory allocated */
    herr_t ret;            /* Return value */

    /* Output message about test being performed */
    MESSAGE(5, ("Free-list API calls"));

    /* Create dataspace */
    /* (Allocates array free-list nodes) */
    sid = H5Screate_simple(MISC35_SPACE_RANK, dims, NULL);
    CHECK(sid, H5I_INVALID_HID, "H5Screate_simple");

    /* Select sequence of ten points */
    ret = H5Sselect_elements(sid, H5S_SELECT_SET, (size_t)MISC35_NPOINTS, (const hsize_t *)coord);
    CHECK(ret, FAIL, "H5Sselect_elements");

    /* Close dataspace */
    ret = H5Sclose(sid);
    CHECK(ret, FAIL, "H5Sclose");

    /* Retrieve initial free list values */
    ret = H5get_free_list_sizes(&reg_size_start, &arr_size_start, &blk_size_start, &fac_size_start);
    CHECK(ret, FAIL, "H5get_free_list_sizes");

#if !defined H5_NO_FREE_LISTS && !defined H5_USING_MEMCHECKER
    /* All the free list values should be >0 */
    CHECK(reg_size_start, 0, "H5get_free_list_sizes");
    CHECK(arr_size_start, 0, "H5get_free_list_sizes");
    CHECK(blk_size_start, 0, "H5get_free_list_sizes");
    CHECK(fac_size_start, 0, "H5get_free_list_sizes");
#else
    /* All the values should be == 0 */
    VERIFY(reg_size_start, 0, "H5get_free_list_sizes");
    VERIFY(arr_size_start, 0, "H5get_free_list_sizes");
    VERIFY(blk_size_start, 0, "H5get_free_list_sizes");
    VERIFY(fac_size_start, 0, "H5get_free_list_sizes");
#endif

    /* Garbage collect the free lists */
    ret = H5garbage_collect();
    CHECK(ret, FAIL, "H5garbage_collect");

    /* Retrieve free list values again */
    ret = H5get_free_list_sizes(&reg_size_final, &arr_size_final, &blk_size_final, &fac_size_final);
    CHECK(ret, FAIL, "H5get_free_list_sizes");

    /* All the free list values should be <= previous values */
    if (reg_size_final > reg_size_start)
        ERROR("reg_size_final > reg_size_start");
    if (arr_size_final > arr_size_start)
        ERROR("arr_size_final > arr_size_start");
    if (blk_size_final > blk_size_start)
        ERROR("blk_size_final > blk_size_start");
    if (fac_size_final > fac_size_start)
        ERROR("fac_size_final > fac_size_start");

} /* end test_misc35() */

/* Context to pass to 'atclose' callbacks */
static int test_misc36_context;

/* 'atclose' callbacks for test_misc36 */
static void
test_misc36_cb1(void *_ctx)
{
    int   *ctx = (int *)_ctx; /* Set up context pointer */
    bool   is_terminating;    /* Flag indicating the library is terminating */
    herr_t ret;               /* Return value */

    /* Check whether the library thinks it's terminating */
    is_terminating = false;
    ret            = H5is_library_terminating(&is_terminating);
    CHECK(ret, FAIL, "H5is_library_terminating");
    VERIFY(is_terminating, true, "H5is_library_terminating");

    /* Verify correct ordering for 'atclose' callbacks */
    VERIFY(*ctx, 0, "Wrong context value");

    /* Update context value */
    *ctx = 1;
}

static void
test_misc36_cb2(void *_ctx)
{
    int   *ctx = (int *)_ctx; /* Set up context pointer */
    bool   is_terminating;    /* Flag indicating the library is terminating */
    herr_t ret;               /* Return value */

    /* Check whether the library thinks it's terminating */
    is_terminating = false;
    ret            = H5is_library_terminating(&is_terminating);
    CHECK(ret, FAIL, "H5is_library_terminating");
    VERIFY(is_terminating, true, "H5is_library_terminating");

    /* Verify correct ordering for 'atclose' callbacks */
    VERIFY(*ctx, 1, "Wrong context value");

    /* Update context value */
    *ctx = 2;
}

/****************************************************************
**
**  test_misc36(): Exercise H5atclose and H5is_library_terminating
**
****************************************************************/
static void
test_misc36(void)
{
    bool   is_terminating; /* Flag indicating the library is terminating */
    herr_t ret;            /* Return value */

    /* Output message about test being performed */
    MESSAGE(5, ("H5atclose and H5is_library_terminating API calls"));

    /* Check whether the library thinks it's terminating */
    is_terminating = true;
    ret            = H5is_library_terminating(&is_terminating);
    CHECK(ret, FAIL, "H5is_library_terminating");
    VERIFY(is_terminating, false, "H5is_library_terminating");

    /* Shut the library down */
    test_misc36_context = 0;
    H5close();

    /* Check whether the library thinks it's terminating */
    is_terminating = true;
    ret            = H5is_library_terminating(&is_terminating);
    CHECK(ret, FAIL, "H5is_library_terminating");
    VERIFY(is_terminating, false, "H5is_library_terminating");

    /* Check the close context was not changed */
    VERIFY(test_misc36_context, 0, "H5atclose");

    /* Restart the library */
    H5open();

    /* Check whether the library thinks it's terminating */
    is_terminating = true;
    ret            = H5is_library_terminating(&is_terminating);
    CHECK(ret, FAIL, "H5is_library_terminating");
    VERIFY(is_terminating, false, "H5is_library_terminating");

    /* Register the 'atclose' callbacks */
    /* (Note that these will be called in reverse order, which is checked) */
    ret = H5atclose(&test_misc36_cb2, &test_misc36_context);
    CHECK(ret, FAIL, "H5atclose");
    ret = H5atclose(&test_misc36_cb1, &test_misc36_context);
    CHECK(ret, FAIL, "H5atclose");

    /* Shut the library down */
    test_misc36_context = 0;
    H5close();

    /* Check the close context was changed correctly */
    VERIFY(test_misc36_context, 2, "H5atclose");

    /* Restart the library */
    H5open();

    /* Close the library again */
    test_misc36_context = 0;
    H5close();

    /* Check the close context was not changed */
    VERIFY(test_misc36_context, 0, "H5atclose");
} /* end test_misc36() */

/****************************************************************
**
**  test_misc37():
**      Test for seg fault issue when closing the provided test file
**      which has an illegal file size in its cache image.
**      See HDFFV-11052/CVE-2020-10812 for details.
**
****************************************************************/
static void
test_misc37(void)
{
    const char *testfile = H5_get_srcdir_filename(CVE_2020_10812_FILENAME);
    bool        vol_is_native;
    bool        driver_is_default_compatible;
    hid_t       fid;
    herr_t      ret;

    /* Output message about test being performed */
    MESSAGE(5, ("Fix for HDFFV-11052/CVE-2020-10812"));

    /* Check if native VOL is being used */
    CHECK(h5_using_native_vol(H5P_DEFAULT, H5I_INVALID_HID, &vol_is_native), FAIL, "h5_using_native_vol");
    if (!vol_is_native) {
        MESSAGE(5, (" -- SKIPPED --\n"));
        return;
    }

    ret = h5_driver_is_default_vfd_compatible(H5P_DEFAULT, &driver_is_default_compatible);
    CHECK(ret, FAIL, "h5_driver_is_default_vfd_compatible");

    if (!driver_is_default_compatible) {
        MESSAGE(5, (" -- SKIPPED --\n"));
        return;
    }

    fid = H5Fopen(testfile, H5F_ACC_RDONLY, H5P_DEFAULT);
    CHECK(fid, FAIL, "H5Fopen");

    /* This should fail due to the illegal file size.
       It should fail gracefully and not seg fault */
    H5E_BEGIN_TRY
    {
        ret = H5Fclose(fid);
    }
    H5E_END_TRY
    VERIFY(ret, FAIL, "H5Fclose");

} /* end test_misc37() */

/****************************************************************
**
**  test_misc38():
**      Test for seg fault issue when opening dataset with corrupted
**      object header.
**
****************************************************************/
static void
test_misc38(void)
{
    const char *testfile  = H5_get_srcdir_filename(MISC38A_FILE); /* Corrected test file name */
    const char *testfile2 = H5_get_srcdir_filename(MISC38B_FILE); /* Corrected test file name */
    bool        driver_is_default_compatible;
    hid_t       fapl = H5I_INVALID_HID; /* File access property list */
    hid_t       fid  = H5I_INVALID_HID; /* File ID */
    hid_t       did  = H5I_INVALID_HID; /* Dataset ID */
    hid_t       sid  = H5I_INVALID_HID; /* Dataspace ID */
    hid_t       tid  = H5I_INVALID_HID; /* Datatype ID */
    hid_t       gid  = H5I_INVALID_HID; /* Group ID */
    hid_t       aid  = H5I_INVALID_HID; /* Attribute ID */
    size_t      type_size;              /* Size of dataset's datatype */
    uint64_t    rfic_flags;             /* Value of RFIC flags property for FAPL & file */
    herr_t      ret;

    /* Output message about test being performed */
    MESSAGE(5, ("Fix for detecting numeric datatypes with unusually large numbers of unused bits"));

    ret = h5_driver_is_default_vfd_compatible(H5P_DEFAULT, &driver_is_default_compatible);
    CHECK(ret, FAIL, "h5_driver_is_default_vfd_compatible");

    if (!driver_is_default_compatible) {
        printf("-- SKIPPED --\n");
        return;
    }

    fid = H5Fopen(testfile, H5F_ACC_RDONLY, H5P_DEFAULT);
    CHECK(fid, FAIL, "H5Fopen");

    /* This should fail due to the illegal datatype encoding in the corrupted
     * object header.
     * It should fail gracefully and not seg fault
     */
    H5E_BEGIN_TRY
    {
        did = H5Dopen2(fid, MISC38A_DSETNAME, H5P_DEFAULT);
    }
    H5E_END_TRY
    VERIFY(did, H5I_INVALID_HID, "H5Dopen2");

    /* Close file */
    ret = H5Fclose(fid);
    CHECK(ret, FAIL, "H5Fclose");

    /* Create a file access property list */
    fapl = H5Pcreate(H5P_FILE_ACCESS);
    CHECK(fapl, H5I_INVALID_HID, "H5Pcreate");

    /* Get property to allow unusual datatypes to be opened */
    rfic_flags = H5F_RFIC_ALL;
    ret        = H5Pget_relax_file_integrity_checks(fapl, &rfic_flags);
    CHECK(ret, FAIL, "H5Pget_relax_file_integrity_checks");
    VERIFY(rfic_flags, 0, "H5Pget_relax_file_integrity_checks");

    /* Set property to allow unusual datatypes to be opened */
    ret = H5Pset_relax_file_integrity_checks(fapl, H5F_RFIC_UNUSUAL_NUM_UNUSED_NUMERIC_BITS);
    CHECK(ret, FAIL, "H5Pset_relax_file_integrity_checks");

    /* Get property to allow unusual datatypes to be opened */
    rfic_flags = 0;
    ret        = H5Pget_relax_file_integrity_checks(fapl, &rfic_flags);
    CHECK(ret, FAIL, "H5Pget_relax_file_integrity_checks");
    VERIFY(rfic_flags, H5F_RFIC_UNUSUAL_NUM_UNUSED_NUMERIC_BITS, "H5Pget_relax_file_integrity_checks");

    /* Open valid file */
    fid = H5Fopen(testfile2, H5F_ACC_RDONLY, fapl);
    CHECK(fid, H5I_INVALID_HID, "H5Fopen");

    /* Close file access property list */
    ret = H5Pclose(fapl);
    CHECK(ret, FAIL, "H5Pclose");

    /* Open dataset w/unusual datatype
     * It should succeed and not return an error or seg fault
     */
    did = H5Dopen2(fid, MISC38B_DSETNAME, H5P_DEFAULT);
    CHECK(did, H5I_INVALID_HID, "H5Dopen2");

    /* Get the dataset's datatype */
    tid = H5Dget_type(did);
    CHECK(tid, H5I_INVALID_HID, "H5Dget_type");

    type_size = H5Tget_size(tid);
    CHECK(type_size, 0, "H5Tget_size");
    VERIFY(type_size, 1000, "H5Tget_size");

    ret = H5Tclose(tid);
    CHECK(ret, FAIL, "H5Tclose");

    ret = H5Dclose(did);
    CHECK(ret, FAIL, "H5Dclose");

    /* Check that property is handled correctly */
    fapl = H5Fget_access_plist(fid);
    CHECK(fapl, H5I_INVALID_HID, "H5Fget_access_plist");

    /* Get property to allow unusual datatypes to be opened */
    rfic_flags = 0;
    ret        = H5Pget_relax_file_integrity_checks(fapl, &rfic_flags);
    CHECK(ret, FAIL, "H5Pget_relax_file_integrity_checks");
    VERIFY(rfic_flags, H5F_RFIC_UNUSUAL_NUM_UNUSED_NUMERIC_BITS, "H5Pget_relax_file_integrity_checks");

    /* Close file access property list */
    ret = H5Pclose(fapl);
    CHECK(ret, FAIL, "H5Pclose");

    /* Close file */
    ret = H5Fclose(fid);
    CHECK(ret, FAIL, "H5Fclose");

    /* Create objects with unusual datatypes and verify correct behavior */
    for (unsigned u = 0; u < 3; u++) {
        /* Create a file access property list */
        fapl = H5Pcreate(H5P_FILE_ACCESS);
        CHECK(fapl, H5I_INVALID_HID, "H5Pcreate");

        if (1 == u) {
            /* Set property to allow unusual datatypes to be opened */
            ret = H5Pset_relax_file_integrity_checks(fapl, H5F_RFIC_UNUSUAL_NUM_UNUSED_NUMERIC_BITS);
            CHECK(ret, FAIL, "H5Pset_relax_file_integrity_checks");
        }
        else if (2 == u) {
            /* Use a later version of the file format, with checksummed object headers */
            ret = H5Pset_libver_bounds(fapl, H5F_LIBVER_LATEST, H5F_LIBVER_LATEST);
            CHECK(ret, FAIL, "H5Pset_libver_bounds");
        }

        fid = H5Fcreate(MISC38C_FILE, H5F_ACC_TRUNC, H5P_DEFAULT, fapl);
        CHECK(fid, H5I_INVALID_HID, "H5Fcreate");

        /* Close file access property list */
        ret = H5Pclose(fapl);
        CHECK(ret, FAIL, "H5Pclose");

        sid = H5Screate(H5S_SCALAR);
        CHECK(sid, H5I_INVALID_HID, "H5Screate");

        tid = H5Tcopy(H5T_NATIVE_INT);
        CHECK(tid, H5I_INVALID_HID, "H5Tcopy");

        /* Set type to have unusual size, for precision */
        ret = H5Tset_size(tid, 1000);
        CHECK(ret, FAIL, "H5Tset_size");

        /* Create a dataset with the unusual datatype */
        H5E_BEGIN_TRY
        {
            did = H5Dcreate2(fid, MISC38C_DSETNAME, tid, sid, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
        }
        H5E_END_TRY
        if (u > 0) {
            CHECK(did, H5I_INVALID_HID, "H5Dcreate2");

            ret = H5Dclose(did);
            CHECK(ret, FAIL, "H5Dclose");
        }
        else {
            VERIFY(did, H5I_INVALID_HID, "H5Dcreate2");
        }

        gid = H5Gopen2(fid, "/", H5P_DEFAULT);
        CHECK(gid, H5I_INVALID_HID, "H5Gopen2");

        /* Create an attribute with the unusual datatype */
        H5E_BEGIN_TRY
        {
            aid = H5Acreate2(gid, MISC38C_ATTRNAME, tid, sid, H5P_DEFAULT, H5P_DEFAULT);
        }
        H5E_END_TRY
        if (u > 0) {
            CHECK(aid, H5I_INVALID_HID, "H5Acreate2");

            ret = H5Aclose(aid);
            CHECK(ret, FAIL, "H5Aclose");
        }
        else {
            VERIFY(aid, H5I_INVALID_HID, "H5Acreate2");
        }

        ret = H5Gclose(gid);
        CHECK(ret, FAIL, "H5Gclose");

        ret = H5Sclose(sid);
        CHECK(ret, FAIL, "H5Sclose");

        /* Create a committed datatype with the unusual datatype */
        H5E_BEGIN_TRY
        {
            ret = H5Tcommit2(fid, MISC38C_TYPENAME, tid, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
        }
        H5E_END_TRY
        if (u > 0) {
            CHECK(ret, FAIL, "H5Tcommit2");
        }
        else {
            VERIFY(ret, FAIL, "H5Tcommit2");
        }

        if (tid != H5I_INVALID_HID) {
            ret = H5Tclose(tid);
            CHECK(ret, FAIL, "H5Tclose");
        }

        ret = H5Fclose(fid);
        CHECK(ret, FAIL, "H5Fclose");
    }
} /* end test_misc38() */

/****************************************************************
**
**  test_misc39():
**      Test for issue where the type conversion path table cache
**      would grow continuously when variable-length datatypes
**      are involved due to file VOL object comparisons causing
**      the library not to reuse type conversion paths
**
****************************************************************/
static void
test_misc39(void)
{
    H5VL_object_t *file_vol_obj  = NULL;
    const char    *buf[]         = {"attr_value"};
    const char    *array_buf[]   = {"attr_value1", "attr_value2"};
    hsize_t        array_dims[1] = {2};
    herr_t         ret           = SUCCEED;
    hid_t          file_id       = H5I_INVALID_HID;
    hid_t          attr_id1      = H5I_INVALID_HID;
    hid_t          attr_id2      = H5I_INVALID_HID;
    hid_t          attr_id3      = H5I_INVALID_HID;
    hid_t          attr_id4      = H5I_INVALID_HID;
    hid_t          str_type      = H5I_INVALID_HID;
    hid_t          array_type    = H5I_INVALID_HID;
    hid_t          compound_type = H5I_INVALID_HID;
    hid_t          vlen_type     = H5I_INVALID_HID;
    hid_t          space_id      = H5I_INVALID_HID;
    int            init_npaths   = 0;
    char         **rbuf          = NULL;
    char         **arr_rbuf      = NULL;
    bool           vol_is_native;
    typedef struct struct_type {
        const char *buf;
    } struct_type;
    struct_type  cbuf          = {.buf = "attr_value"};
    struct_type *compound_rbuf = NULL;
    hvl_t        vlen_buf      = {.len = 2, .p = array_buf};
    hvl_t       *vlen_rbuf     = NULL;

    /* Output message about test being performed */
    MESSAGE(5, ("Fix for type conversion path table issue"));

    /*
     * Get the initial number of type conversion path table
     * entries that are currently defined
     */
    init_npaths = H5T__get_path_table_npaths();

    file_id = H5Fcreate(MISC39_FILE, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
    CHECK(file_id, H5I_INVALID_HID, "H5Fcreate");

    /* Check if native VOL is being used */
    CHECK(h5_using_native_vol(H5P_DEFAULT, file_id, &vol_is_native), FAIL, "h5_using_native_vol");
    if (!vol_is_native) {
        CHECK(H5Fclose(file_id), FAIL, "H5Fclose");
        MESSAGE(5, (" -- SKIPPED --\n"));
        return;
    }

    /* Retrieve file's VOL object field for further use */
    file_vol_obj = H5F_VOL_OBJ((H5F_t *)H5VL_object(file_id));

    /*
     * Check reference count of file's VOL object field. At this point,
     * the object should have a reference count of 1 since the file
     * was just created.
     */
    VERIFY(file_vol_obj->rc, 1, "checking reference count");

    /* Create a variable-length string type */
    str_type = H5Tcopy(H5T_C_S1);
    CHECK(str_type, H5I_INVALID_HID, "H5Tcopy");

    ret = H5Tset_size(str_type, H5T_VARIABLE);
    CHECK(ret, FAIL, "H5Tset_size");

    /* Create an array type of the string type */
    array_type = H5Tarray_create2(str_type, 1, array_dims);
    CHECK(array_type, H5I_INVALID_HID, "H5Tarray_create2");

    /* Create a compound type of the string type */
    compound_type = H5Tcreate(H5T_COMPOUND, sizeof(compound_type));
    CHECK(compound_type, H5I_INVALID_HID, "H5Tcreate(H5T_COMPOUND, ...)");

    CHECK(H5Tinsert(compound_type, "varstr", HOFFSET(struct_type, buf), str_type), FAIL, "H5Tinsert");

    /* Create a variable-length type of the string type */
    vlen_type = H5Tvlen_create(str_type);
    CHECK(vlen_type, H5I_INVALID_HID, "H5Tvlen_create");

    space_id = H5Screate(H5S_SCALAR);
    CHECK(space_id, H5I_INVALID_HID, "H5Screate");

    /*
     * Check the number of type conversion path table entries currently
     * stored in the cache. It shouldn't have changed yet.
     */
    VERIFY(H5T__get_path_table_npaths(), init_npaths,
           "checking number of type conversion path table entries");

    /* Increments file's VOL object reference count by 1 */
    attr_id1 = H5Acreate2(file_id, "varstr_attribute", str_type, space_id, H5P_DEFAULT, H5P_DEFAULT);
    CHECK(attr_id1, H5I_INVALID_HID, "H5Acreate2");
    /* Increments file's VOL object reference count by 1 */
    attr_id2 = H5Acreate2(file_id, "array_varstr_attribute", array_type, space_id, H5P_DEFAULT, H5P_DEFAULT);
    CHECK(attr_id2, H5I_INVALID_HID, "H5Acreate2");
    /* Increments file's VOL object reference count by 1 */
    attr_id3 =
        H5Acreate2(file_id, "compound_varstr_attribute", compound_type, space_id, H5P_DEFAULT, H5P_DEFAULT);
    CHECK(attr_id3, H5I_INVALID_HID, "H5Acreate2");
    /* Increments file's VOL object reference count by 2 */
    attr_id4 = H5Acreate2(file_id, "vlen_varstr_attribute", vlen_type, space_id, H5P_DEFAULT, H5P_DEFAULT);
    CHECK(attr_id4, H5I_INVALID_HID, "H5Acreate2");

    /*
     * Check the number of type conversion path table entries currently
     * stored in the cache. It shouldn't have changed yet.
     */
    VERIFY(H5T__get_path_table_npaths(), init_npaths,
           "checking number of type conversion path table entries");

    /*
     * Check reference count of file's VOL object field. At this point,
     * the object should have a reference count of 6. Creating the
     * attributes in the file will have caused H5T_set_loc calls that
     * associate each attribute's datatype with the file's VOL object
     * and will have incremented the reference count by 5.
     */
    VERIFY(file_vol_obj->rc, 6, "checking reference count");

    /* Increments file's VOL object reference count by 1 */
    ret = H5Awrite(attr_id1, str_type, buf);
    CHECK(ret, FAIL, "H5Awrite");
    /* Increments file's VOL object reference count by 1 */
    ret = H5Awrite(attr_id2, array_type, array_buf);
    CHECK(ret, FAIL, "H5Awrite");
    /* Increments file's VOL object reference count by 2 */
    ret = H5Awrite(attr_id3, compound_type, &cbuf);
    CHECK(ret, FAIL, "H5Awrite");
    /* Increments file's VOL object reference count by 2 */
    ret = H5Awrite(attr_id4, vlen_type, &vlen_buf);
    CHECK(ret, FAIL, "H5Awrite");

    /*
     * Check the number of type conversion path table entries currently
     * stored in the cache. The H5Awrite calls should have added new
     * type conversion paths. Note that if another test in this file
     * uses the same conversion path, this check may fail and need to
     * be refactored.
     */
    VERIFY(H5T__get_path_table_npaths(), init_npaths + 4,
           "checking number of type conversion path table entries");

    /*
     * Check reference count of file's VOL object field. At this point,
     * the object should have a reference count of 12. Writing to the
     * attributes will have caused H5T_path_find calls that end up
     * incrementing the reference count of the associated file's VOL
     * object.
     */
    VERIFY(file_vol_obj->rc, 12, "checking reference count");

    ret = H5Aclose(attr_id1);
    CHECK(ret, FAIL, "H5Aclose");
    ret = H5Aclose(attr_id2);
    CHECK(ret, FAIL, "H5Aclose");
    ret = H5Aclose(attr_id3);
    CHECK(ret, FAIL, "H5Aclose");
    ret = H5Aclose(attr_id4);
    CHECK(ret, FAIL, "H5Aclose");
    ret = H5Fclose(file_id);
    CHECK(ret, FAIL, "H5Fclose");

    rbuf = malloc(sizeof(char *));
    CHECK_PTR(rbuf, "varstr read buf allocation");
    arr_rbuf = malloc(array_dims[0] * sizeof(char *));
    CHECK_PTR(arr_rbuf, "array varstr read buf allocation");
    compound_rbuf = malloc(sizeof(struct_type));
    CHECK_PTR(compound_rbuf, "compound varstr read buf allocation");
    vlen_rbuf = malloc(sizeof(hvl_t));
    CHECK_PTR(vlen_rbuf, "vlen varstr read buf allocation");

    for (size_t i = 0; i < 10; i++) {
        file_id = H5Fopen(MISC39_FILE, H5F_ACC_RDONLY, H5P_DEFAULT);
        CHECK(file_id, H5I_INVALID_HID, "H5Fopen");

        /* Retrieve file's VOL object field for further use */
        file_vol_obj = H5F_VOL_OBJ((H5F_t *)H5VL_object(file_id));

        /*
         * Check reference count of file's VOL object field. At this point,
         * the object should have a reference count of 1 since the file
         * was just opened.
         */
        VERIFY(file_vol_obj->rc, 1, "checking reference count");

        /* Increments file's VOL object reference count by 1 */
        attr_id1 = H5Aopen(file_id, "varstr_attribute", H5P_DEFAULT);
        CHECK(attr_id1, H5I_INVALID_HID, "H5Aopen");
        /* Increments file's VOL object reference count by 1 */
        attr_id2 = H5Aopen(file_id, "array_varstr_attribute", H5P_DEFAULT);
        CHECK(attr_id2, H5I_INVALID_HID, "H5Aopen");
        /* Increments file's VOL object reference count by 1 */
        attr_id3 = H5Aopen(file_id, "compound_varstr_attribute", H5P_DEFAULT);
        CHECK(attr_id3, H5I_INVALID_HID, "H5Aopen");
        /* Increments file's VOL object reference count by 2 */
        attr_id4 = H5Aopen(file_id, "vlen_varstr_attribute", H5P_DEFAULT);
        CHECK(attr_id4, H5I_INVALID_HID, "H5Aopen");

        /*
         * Check reference count of file's VOL object field. At this point,
         * the object should have a reference count of 6 since opening
         * the attributes will also have associated their datatypes with
         * the file's VOL object.
         */
        VERIFY(file_vol_obj->rc, 6, "checking reference count");

        /* Increments file's VOL object reference count by 1 */
        ret = H5Aread(attr_id1, str_type, rbuf);
        CHECK(ret, FAIL, "H5Aread");
        /* Increments file's VOL object reference count by 1 */
        ret = H5Aread(attr_id2, array_type, arr_rbuf);
        CHECK(ret, FAIL, "H5Aread");
        /* Increments file's VOL object reference count by 2 */
        ret = H5Aread(attr_id3, compound_type, compound_rbuf);
        CHECK(ret, FAIL, "H5Aread");
        /* Increments file's VOL object reference count by 2 */
        ret = H5Aread(attr_id4, vlen_type, vlen_rbuf);
        CHECK(ret, FAIL, "H5Aread");

        /*
         * Check the number of type conversion path table entries currently
         * stored in the cache. Each H5Aread call shouldn't cause this number
         * to keep going up, as the library should remove the cached conversion
         * paths on file close during each iteration. The value should stay at
         * a constant "initial_num_paths + number of H5Aread calls above".
         */
        VERIFY(H5T__get_path_table_npaths(), init_npaths + 4,
               "checking number of type conversion path table entries");

        /*
         * Check reference count of file's VOL object field. At this point,
         * the object should have a reference count of 12. Reading from the
         * attributes will have caused H5T_path_find calls that end up
         * incrementing the reference count of the associated file's VOL
         * object.
         */
        VERIFY(file_vol_obj->rc, 12, "checking reference count");

        ret = H5Treclaim(str_type, space_id, H5P_DEFAULT, rbuf);
        ret = H5Treclaim(array_type, space_id, H5P_DEFAULT, arr_rbuf);
        ret = H5Treclaim(compound_type, space_id, H5P_DEFAULT, compound_rbuf);
        ret = H5Treclaim(vlen_type, space_id, H5P_DEFAULT, vlen_rbuf);

        ret = H5Aclose(attr_id1);
        CHECK(ret, FAIL, "H5Aclose");
        ret = H5Aclose(attr_id2);
        CHECK(ret, FAIL, "H5Aclose");
        ret = H5Aclose(attr_id3);
        CHECK(ret, FAIL, "H5Aclose");
        ret = H5Aclose(attr_id4);
        CHECK(ret, FAIL, "H5Aclose");
        ret = H5Fclose(file_id);
        CHECK(ret, FAIL, "H5Fclose");
    }

    free(rbuf);
    free(arr_rbuf);
    free(compound_rbuf);
    free(vlen_rbuf);

    ret = H5Tclose(str_type);
    CHECK(ret, FAIL, "H5Tclose");
    ret = H5Tclose(array_type);
    CHECK(ret, FAIL, "H5Tclose");
    ret = H5Tclose(compound_type);
    CHECK(ret, FAIL, "H5Tclose");
    ret = H5Tclose(vlen_type);
    CHECK(ret, FAIL, "H5Tclose");
    ret = H5Sclose(space_id);
    CHECK(ret, FAIL, "H5Sclose");
}

/****************************************************************
**
**  test_misc40(): Ensure H5Pset_est_link_info() handles large
**                 values
**
**  H5Pset_est_link_info() values can be set to large values,
**  which could cause the library to attempt to create large
**  object headers that exceed limits and trip asserts in
**  the library.
**
**  This test ensures that the library doesn't error regardless
**  of the values passed to H5Pset_est_link_info() and
**  H5Pset_link_phase_change().
**
****************************************************************/
static void
test_misc40(void)
{
    hid_t  fid  = H5I_INVALID_HID; /* File ID */
    hid_t  gid  = H5I_INVALID_HID; /* Group ID */
    hid_t  fapl = H5I_INVALID_HID; /* File access property list ID */
    hid_t  gcpl = H5I_INVALID_HID; /* Group creation property list ID */
    herr_t ret  = H5I_INVALID_HID; /* Generic return value */

    /* Output message about test being performed */
    MESSAGE(5, ("Ensure H5Pset_est_link_info handles large values\n"));

    /* Compose file access property list
     *
     * NOTE: The bug in question only occurs in new-style groups
     */
    fapl = H5Pcreate(H5P_FILE_ACCESS);
    CHECK(fapl, H5I_INVALID_HID, "H5Pcreate");
    ret = H5Pset_libver_bounds(fapl, H5F_LIBVER_LATEST, H5F_LIBVER_LATEST);
    CHECK(ret, FAIL, "H5Pset_libver_bounds");

    /* Create the file */
    fid = H5Fcreate(MISC40_FILE, H5F_ACC_TRUNC, H5P_DEFAULT, fapl);
    CHECK(fid, H5I_INVALID_HID, "H5Fcreate");

    /* Compose group creation property list */
    gcpl = H5Pcreate(H5P_GROUP_CREATE);
    CHECK(gcpl, H5I_INVALID_HID, "H5Pcreate");

    /* Set the estimated link info values to large numbers */
    ret = H5Pset_est_link_info(gcpl, UINT16_MAX, UINT16_MAX);
    CHECK(ret, FAIL, "H5Pset_est_link_info");

    /* Create a group */
    gid = H5Gcreate2(fid, "foo", H5P_DEFAULT, gcpl, H5P_DEFAULT);
    CHECK(gid, H5I_INVALID_HID, "H5Gcreate2");

    /* Close the group */
    ret = H5Gclose(gid);
    CHECK(ret, FAIL, "H5Gclose");

    /* Close the file. Asserts typically occur here, when the metadata cache
     * objects are flushed.
     */
    ret = H5Fclose(fid);
    CHECK(ret, FAIL, "H5Fclose");

    /* Re-open the file */
    fid = H5Fopen(MISC25C_FILE, H5F_ACC_RDWR, H5P_DEFAULT);
    CHECK(fid, H5I_INVALID_HID, "H5Fopen");

    /* Set the compact/dense value high, to see if we can trick the
     * library into creating a dense group object header that is
     * larger than the maximum allowed size.
     */
    ret = H5Pset_link_phase_change(gcpl, UINT16_MAX, UINT16_MAX);
    CHECK(ret, FAIL, "H5Pset_link_phase_change");

    /* Set the estimated link info values to large numbers */
    ret = H5Pset_est_link_info(gcpl, UINT16_MAX / 2, UINT16_MAX);
    CHECK(ret, FAIL, "H5Pset_est_link_info");

    /* Create another group */
    gid = H5Gcreate2(fid, "bar", H5P_DEFAULT, gcpl, H5P_DEFAULT);
    CHECK(gid, H5I_INVALID_HID, "H5Gcreate2");

    /* Close the group */
    ret = H5Gclose(gid);
    CHECK(ret, FAIL, "H5Gclose");

    /* Close the file */
    ret = H5Fclose(fid);
    CHECK(ret, FAIL, "H5Fclose");

    /* Close the property lists */
    ret = H5Pclose(fapl);
    CHECK(ret, FAIL, "H5Pclose");
    ret = H5Pclose(gcpl);
    CHECK(ret, FAIL, "H5Pclose");

} /* end test_misc40() */

/****************************************************************
**
**  test_misc41(): Test that object creation properties are propagated
**  to intermediate groups.
**
****************************************************************/
static void
test_misc41(void)
{
    hid_t    lcpl        = H5I_INVALID_HID;
    hid_t    gcpl        = H5I_INVALID_HID;
    hid_t    gcpl2       = H5I_INVALID_HID;
    hid_t    def_gcpl    = H5I_INVALID_HID;
    hid_t    dcpl        = H5I_INVALID_HID;
    hid_t    fid         = H5I_INVALID_HID;
    hid_t    gid         = H5I_INVALID_HID;
    hid_t    gid2        = H5I_INVALID_HID;
    hid_t    def_gid     = H5I_INVALID_HID;
    hid_t    did         = H5I_INVALID_HID;
    hid_t    sid         = H5I_INVALID_HID;
    hsize_t  dims[1]     = {10};
    unsigned cr_order    = 0;
    bool     track_times = false;
    herr_t   status;

    /* Output message about test being performed */
    MESSAGE(5, ("Testing object creation properties are propagated to intermediate groups\n"));

    lcpl = H5Pcreate(H5P_LINK_CREATE);
    CHECK(lcpl, FAIL, "H5Pcreate");

    status = H5Pset_create_intermediate_group(lcpl, 1);
    CHECK(status, FAIL, "H5Pset_create_intermediate_group");

    fid = H5Fcreate(MISC41_FILE, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
    CHECK(fid, FAIL, "H5Fcreate");

    /*
     * Verify default creation properties when creating
     * group with intermdiate group
     */

    /* Create groups that use default creation properties */
    def_gid = H5Gcreate2(fid, "def_group1/def_group2", lcpl, H5P_DEFAULT, H5P_DEFAULT);
    CHECK(def_gid, FAIL, "H5Gcreate2");

    status = H5Gclose(def_gid);
    CHECK(status, FAIL, "H5Gclose");

    def_gid = H5Gopen2(fid, "def_group1", H5P_DEFAULT);
    CHECK(def_gid, FAIL, "H5Gopen2");

    def_gcpl = H5Gget_create_plist(def_gid);
    CHECK(def_gcpl, FAIL, "H5Gget_create_plist");

    /* Default is true */
    status = H5Pget_obj_track_times(def_gcpl, &track_times);
    CHECK(status, FAIL, "H5Pget_obj_track_times");
    VERIFY(track_times, true, "H5Pget_obj_track_times");

    /* Default is false */
    status = H5Pget_attr_creation_order(def_gcpl, &cr_order);
    CHECK(status, FAIL, "H5Pget_attr_creation_order");
    VERIFY(cr_order, false, "H5Pget_attr_creation_order");

    status = H5Gclose(def_gid);
    CHECK(status, FAIL, "H5Gclose");

    status = H5Pclose(def_gcpl);
    CHECK(status, FAIL, "H5Pclose");

    /*
     * Verify non-default creation properties when creating
     * group with intermediate groups
     */
    gcpl = H5Pcreate(H5P_GROUP_CREATE);
    CHECK(gcpl, FAIL, "H5Pcreate");

    status = H5Pset_attr_creation_order(gcpl, H5P_CRT_ORDER_TRACKED);
    CHECK(status, FAIL, "H5Pset_attr_creation_order");

    status = H5Pset_obj_track_times(gcpl, false);
    CHECK(status, FAIL, "H5Pset_obj_track_times");

    gid = H5Gcreate2(fid, "group1/group2/group3", lcpl, gcpl, H5P_DEFAULT);
    CHECK(gid, FAIL, "H5Gcreate2");

    status = H5Pclose(gcpl);
    CHECK(status, FAIL, "H5Pclose");

    /* Verify group3 */
    gcpl = H5Gget_create_plist(gid);
    CHECK(gcpl, FAIL, "H5Gget_create_plist");

    status = H5Pget_attr_creation_order(gcpl, &cr_order);
    CHECK(status, FAIL, "H5Pget_attr_creation_order");
    VERIFY(cr_order, H5P_CRT_ORDER_TRACKED, "H5Pget_attr_creation_order");

    status = H5Pget_obj_track_times(gcpl, &track_times);
    CHECK(status, FAIL, "H5Pget_obj_track_times");
    VERIFY(track_times, false, "H5Pget_obj_track_times");

    status = H5Gclose(gid);
    CHECK(status, FAIL, "H5Gclose");
    status = H5Pclose(gcpl);
    CHECK(status, FAIL, "H5Pclose");

    /* Verify group1 */
    gid = H5Gopen2(fid, "group1", H5P_DEFAULT);
    CHECK(gid, FAIL, "H5Gopen2");

    gcpl = H5Gget_create_plist(gid);
    CHECK(gcpl, FAIL, "H5Gget_create_plist");

    status = H5Pget_attr_creation_order(gcpl, &cr_order);
    CHECK(status, FAIL, "H5Pget_attr_creation_order");
    VERIFY(cr_order, H5P_CRT_ORDER_TRACKED, "H5Pget_attr_creation_order");

    status = H5Pget_obj_track_times(gcpl, &track_times);
    CHECK(status, FAIL, "H5Pget_obj_track_times");
    VERIFY(track_times, false, "H5Pget_obj_track_times");

    /* Verify group2 */
    gid2 = H5Gopen2(gid, "group2", H5P_DEFAULT);
    CHECK(gid2, FAIL, "H5Gopen2");

    gcpl2 = H5Gget_create_plist(gid2);
    CHECK(gcpl2, FAIL, "H5Gget_create_plist");

    status = H5Pget_attr_creation_order(gcpl2, &cr_order);
    CHECK(status, FAIL, "H5Pget_attr_creation_order");
    VERIFY(cr_order, H5P_CRT_ORDER_TRACKED, "H5Pget_attr_creation_order");

    status = H5Pget_obj_track_times(gcpl2, &track_times);
    CHECK(status, FAIL, "H5Pget_obj_track_times");
    VERIFY(track_times, false, "H5Pget_obj_track_times");

    status = H5Gclose(gid2);
    CHECK(status, FAIL, "H5Gclose");
    status = H5Pclose(gcpl2);
    CHECK(status, FAIL, "H5Pclose");

    status = H5Gclose(gid);
    CHECK(status, FAIL, "H5Gclose");
    status = H5Pclose(gcpl);
    CHECK(status, FAIL, "H5Pclose");

    /*
     * Verify non-default creation properties when creating
     * dataset with intermediate group
     */
    dcpl = H5Pcreate(H5P_DATASET_CREATE);
    CHECK(dcpl, FAIL, "H5Pcreate");

    status = H5Pset_attr_creation_order(dcpl, H5P_CRT_ORDER_TRACKED);
    CHECK(status, FAIL, "H5Pset_attr_creation_order");

    sid = H5Screate_simple(1, dims, NULL);
    CHECK(sid, FAIL, "H5Screate_simple");

    did = H5Dcreate2(fid, "path1/dname", H5T_NATIVE_INT, sid, lcpl, dcpl, H5P_DEFAULT);
    CHECK(did, FAIL, "H5Dcreate2");

    status = H5Dclose(did);
    CHECK(status, FAIL, "H5Dclose");
    status = H5Sclose(sid);
    CHECK(status, FAIL, "H5Sclose");
    status = H5Pclose(dcpl);
    CHECK(status, FAIL, "H5Pclose");

    /* Verify path1 */
    gid = H5Gopen2(fid, "path1", H5P_DEFAULT);
    CHECK(gid, FAIL, "H5Gopen2");

    gcpl = H5Gget_create_plist(gid);
    CHECK(gcpl, FAIL, "H5Gget_create_plist");

    status = H5Pget_attr_creation_order(gcpl, &cr_order);
    CHECK(status, FAIL, "H5Pget_attr_creation_order");
    VERIFY(cr_order, H5P_CRT_ORDER_TRACKED, "H5Pget_attr_creation_order");

    status = H5Pget_obj_track_times(gcpl, &track_times);
    CHECK(status, FAIL, "H5Pget_obj_track_times");
    VERIFY(track_times, true, "H5Pget_obj_track_times");

    /* Verify dname */
    did = H5Dopen2(gid, "dname", H5P_DEFAULT);
    CHECK(did, FAIL, "H5Dopen2");

    dcpl = H5Dget_create_plist(did);
    CHECK(dcpl, FAIL, "H5Dget_create_plist");

    status = H5Pget_attr_creation_order(dcpl, &cr_order);
    VERIFY(cr_order, H5P_CRT_ORDER_TRACKED, "H5Pget_attr_creation_order");

    status = H5Pget_obj_track_times(dcpl, &track_times);
    CHECK(status, FAIL, "H5Pget_obj_track_times");
    VERIFY(track_times, true, "H5Pget_obj_track_times");

    status = H5Dclose(did);
    CHECK(status, FAIL, "H5Dclose");
    status = H5Pclose(dcpl);
    CHECK(status, FAIL, "H5Pclose");

    status = H5Gclose(gid);
    CHECK(status, FAIL, "H5Gclose");
    status = H5Pclose(gcpl);
    CHECK(status, FAIL, "H5Pclose");

    status = H5Fclose(fid);
    CHECK(status, FAIL, "H5Fclose");

    status = H5Pclose(lcpl);
    CHECK(status, FAIL, "H5Pclose");

} /* end test_misc41() */

/****************************************************************
**
**  test_misc(): Main misc. test routine.
**
****************************************************************/
void
test_misc(void)
{
    bool default_driver = h5_using_default_driver(NULL);

    /* Output message about test being performed */
    MESSAGE(5, ("Testing Miscellaneous Routines\n"));

    test_misc1();  /* Test unlinking a dataset & immediately reusing name */
    test_misc2();  /* Test storing a VL-derived datatype in two different files */
    test_misc3();  /* Test reading from chunked dataset with non-zero fill value */
    test_misc4();  /* Test retrieving the fileno for various objects with H5Oget_info() */
    test_misc5();  /* Test several level deep nested compound & VL datatypes */
    test_misc6();  /* Test object header continuation code */
    test_misc7();  /* Test for sensible datatypes stored on disk */
    test_misc8();  /* Test storage sizes of various types of dataset storage */
    test_misc9();  /* Test for opening (not creating) core files */
    test_misc10(); /* Test for using dataset creation property lists from old files */

    if (default_driver) {
        test_misc11(); /* Test for all properties of a file creation property list being stored */
    }

    test_misc12(); /* Test VL-strings in chunked datasets operating correctly */

    if (default_driver) {
        test_misc13(); /* Test that a user block can be insert in front of file contents */
    }

    test_misc14(); /* Test that deleted dataset's data is removed from sieve buffer correctly */

    if (default_driver) {
        test_misc15(); /* Test that checking a file's access property list more than once works */
    }

    test_misc16(); /* Test array of fixed-length string */
    test_misc17(); /* Test array of ASCII character */
    test_misc18(); /* Test new object header information in H5O_info_t struct */
    test_misc19(); /* Test incrementing & decrementing ref count on IDs */
    test_misc20(); /* Test problems with truncated dimensions in version 2 of storage layout message */

#ifdef H5_HAVE_FILTER_SZIP
    test_misc21();  /* Test that "late" allocation time is treated the same as "incremental", for chunked
                       datasets w/a filters */
    test_misc22();  /* check szip bits per pixel */
#endif              /* H5_HAVE_FILTER_SZIP */
    test_misc23();  /* Test intermediate group creation */
    test_misc24();  /* Test inappropriate API opens of objects */
    test_misc25a(); /* Exercise null object header message merge bug */
    test_misc25b(); /* Exercise null object header message merge bug on existing file */
    test_misc25c(); /* Exercise another null object header message merge bug */
    test_misc26();  /* Test closing property lists with long filter pipelines */
    test_misc27();  /* Test opening file with object that has bad # of object header messages */
    test_misc28();  /* Test that chunks are cached appropriately */
    test_misc29();  /* Test that speculative metadata reads are handled correctly */
    test_misc30();  /* Exercise local heap loading bug where free lists were getting dropped */

    if (default_driver) {
        test_misc31(); /* Test Reentering library through deprecated routines after H5close() */
    }

    test_misc32(); /* Test filter memory allocation functions */
    test_misc33(); /* Test to verify that H5HL_offset_into() returns error if offset exceeds heap block */
    test_misc34(); /* Test behavior of 0 and NULL in H5MM API calls */
    test_misc35(); /* Test behavior of free-list & allocation statistics API calls */
    test_misc36(); /* Exercise H5atclose and H5is_library_terminating */
    test_misc37(); /* Test for seg fault failure at file close */
    test_misc38(); /* Test for seg fault when opening corrupted object header */
    test_misc39(); /* Test for type conversion path table issue */
    test_misc40(); /* Ensure H5Pset_est_link_info() handles large values */
    test_misc41(); /* Test object properties propagated to intermediate groups */

} /* test_misc() */

/*-------------------------------------------------------------------------
 * Function:    cleanup_misc
 *
 * Purpose:    Cleanup temporary test files
 *
 * Return:    none
 *
 *-------------------------------------------------------------------------
 */
void
cleanup_misc(void)
{
    H5E_BEGIN_TRY
    {
        H5Fdelete(MISC1_FILE, H5P_DEFAULT);
        H5Fdelete(MISC2_FILE_1, H5P_DEFAULT);
        H5Fdelete(MISC2_FILE_2, H5P_DEFAULT);
        H5Fdelete(MISC3_FILE, H5P_DEFAULT);
        H5Fdelete(MISC4_FILE_1, H5P_DEFAULT);
        H5Fdelete(MISC4_FILE_2, H5P_DEFAULT);
        H5Fdelete(MISC5_FILE, H5P_DEFAULT);
        H5Fdelete(MISC6_FILE, H5P_DEFAULT);
        H5Fdelete(MISC7_FILE, H5P_DEFAULT);
        H5Fdelete(MISC8_FILE, H5P_DEFAULT);
        H5Fdelete(MISC9_FILE, H5P_DEFAULT);
        H5Fdelete(MISC10_FILE_NEW, H5P_DEFAULT);
        H5Fdelete(MISC11_FILE, H5P_DEFAULT);
        H5Fdelete(MISC12_FILE, H5P_DEFAULT);
        H5Fdelete(MISC13_FILE_1, H5P_DEFAULT);
        H5Fdelete(MISC13_FILE_2, H5P_DEFAULT);
        H5Fdelete(MISC14_FILE, H5P_DEFAULT);
        H5Fdelete(MISC15_FILE, H5P_DEFAULT);
        H5Fdelete(MISC16_FILE, H5P_DEFAULT);
        H5Fdelete(MISC17_FILE, H5P_DEFAULT);
        H5Fdelete(MISC18_FILE, H5P_DEFAULT);
        H5Fdelete(MISC19_FILE, H5P_DEFAULT);
        H5Fdelete(MISC20_FILE, H5P_DEFAULT);
#ifdef H5_HAVE_FILTER_SZIP
        H5Fdelete(MISC21_FILE, H5P_DEFAULT);
        H5Fdelete(MISC22_FILE, H5P_DEFAULT);
#endif /* H5_HAVE_FILTER_SZIP */
        H5Fdelete(MISC23_FILE, H5P_DEFAULT);
        H5Fdelete(MISC24_FILE, H5P_DEFAULT);
        H5Fdelete(MISC25A_FILE, H5P_DEFAULT);
        H5Fdelete(MISC25C_FILE, H5P_DEFAULT);
        H5Fdelete(MISC26_FILE, H5P_DEFAULT);
        H5Fdelete(MISC28_FILE, H5P_DEFAULT);
        H5Fdelete(MISC29_COPY_FILE, H5P_DEFAULT);
        H5Fdelete(MISC30_FILE, H5P_DEFAULT);
#ifndef H5_NO_DEPRECATED_SYMBOLS
        H5Fdelete(MISC31_FILE, H5P_DEFAULT);
#endif /* H5_NO_DEPRECATED_SYMBOLS */
        H5Fdelete(MISC38C_FILE, H5P_DEFAULT);
        H5Fdelete(MISC39_FILE, H5P_DEFAULT);
        H5Fdelete(MISC40_FILE, H5P_DEFAULT);
        H5Fdelete(MISC41_FILE, H5P_DEFAULT);
    }
    H5E_END_TRY
} /* end cleanup_misc() */
