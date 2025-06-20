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
 * Main driver of the Parallel HDF5 tests
 */

#include "testphdf5.h"

#ifndef PATH_MAX
#define PATH_MAX 512
#endif /* !PATH_MAX */

/* global variables */
int dim0;
int dim1;
int chunkdim0;
int chunkdim1;
int nerrors   = 0;               /* errors count */
int ndatasets = 300;             /* number of datasets to create*/
int ngroups   = 512;             /* number of groups to create in root
                                  * group. */
int facc_type       = FACC_MPIO; /*Test file access type */
int dxfer_coll_type = DXFER_COLLECTIVE_IO;

/* other option flags */

/* FILENAME and filenames must have the same number of names.
 * Use PARATESTFILE in general and use a separated filename only if the file
 * created in one test is accessed by a different test.
 * filenames[0] is reserved as the file name for PARATESTFILE.
 */
#define NFILENAME    2
#define PARATESTFILE filenames[0]
const char *FILENAME[NFILENAME] = {"ParaTest", NULL};
char       *filenames[NFILENAME];
hid_t       fapl; /* file access property list */

#ifdef USE_PAUSE
/* pause the process for a moment to allow debugger to attach if desired. */
/* Will pause more if greenlight file is not present but will eventually */
/* continue. */
#include <sys/types.h>
#include <sys/stat.h>

void
pause_proc(void)
{

    int       pid;
    h5_stat_t statbuf;
    char      greenlight[] = "go";
    int       maxloop      = 10;
    int       loops        = 0;
    int       time_int     = 10;

    /* mpi variables */
    int  mpi_size, mpi_rank;
    int  mpi_namelen;
    char mpi_name[MPI_MAX_PROCESSOR_NAME];

    pid = getpid();
    MPI_Comm_size(MPI_COMM_WORLD, &mpi_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &mpi_rank);
    MPI_Get_processor_name(mpi_name, &mpi_namelen);

    if (MAINPROCESS) {
        memset(&statbuf, 0, sizeof(h5_stat_t));
        while ((HDstat(greenlight, &statbuf) == -1) && loops < maxloop) {
            if (!loops++) {
                printf("Proc %d (%*s, %d): to debug, attach %d\n", mpi_rank, mpi_namelen, mpi_name, pid, pid);
            }
            printf("waiting(%ds) for file %s ...\n", time_int, greenlight);
            fflush(stdout);
            HDsleep(time_int);

            memset(&statbuf, 0, sizeof(h5_stat_t));
        }
    }
    MPI_Barrier(MPI_COMM_WORLD);
}

/* Use the Profile feature of MPI to call the pause_proc() */
int
MPI_Init(int *argc, char ***argv)
{
    int ret_code;
    ret_code = PMPI_Init(argc, argv);
    pause_proc();
    return (ret_code);
}
#endif /* USE_PAUSE */

/*
 * Show command usage
 */
static void
usage(void)
{
    printf("    [-r] [-w] [-m<n_datasets>] [-n<n_groups>] "
           "[-o] [-f <prefix>] [-d <dim0> <dim1>]\n");
    printf("\t-m<n_datasets>"
           "\tset number of datasets for the multiple dataset test\n");
    printf("\t-n<n_groups>"
           "\tset number of groups for the multiple group test\n");
    printf("\t-f <prefix>\tfilename prefix\n");
    printf("\t-2\t\tuse Split-file together with MPIO\n");
    printf("\t-d <factor0> <factor1>\tdataset dimensions factors. Defaults (%d,%d)\n", ROW_FACTOR,
           COL_FACTOR);
    printf("\t-c <dim0> <dim1>\tdataset chunk dimensions. Defaults (dim0/10,dim1/10)\n");
    printf("\n");
}

/*
 * parse the command line options
 */
static int
parse_options(int argc, char **argv)
{
    int mpi_size, mpi_rank; /* mpi variables */

    MPI_Comm_size(MPI_COMM_WORLD, &mpi_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &mpi_rank);

    /* setup default chunk-size. Make sure sizes are > 0 */

    chunkdim0 = (dim0 + 9) / 10;
    chunkdim1 = (dim1 + 9) / 10;

    while (--argc) {
        if (**(++argv) != '-') {
            break;
        }
        else {
            switch (*(*argv + 1)) {
                case 'm':
                    ndatasets = atoi((*argv + 1) + 1);
                    if (ndatasets < 0) {
                        nerrors++;
                        return (1);
                    }
                    break;
                case 'n':
                    ngroups = atoi((*argv + 1) + 1);
                    if (ngroups < 0) {
                        nerrors++;
                        return (1);
                    }
                    break;
                case 'f':
                    if (--argc < 1) {
                        nerrors++;
                        return (1);
                    }
                    if (**(++argv) == '-') {
                        nerrors++;
                        return (1);
                    }
                    paraprefix = *argv;
                    break;
                case 'i': /* Collective MPI-IO access with independent IO  */
                    dxfer_coll_type = DXFER_INDEPENDENT_IO;
                    break;
                case '2': /* Use the split-file driver with MPIO access */
                    /* Can use $HDF5_METAPREFIX to define the */
                    /* meta-file-prefix. */
                    facc_type = FACC_MPIO | FACC_SPLIT;
                    break;
                case 'd': /* dimensizes */
                    if (--argc < 2) {
                        nerrors++;
                        return (1);
                    }
                    dim0 = atoi(*(++argv)) * mpi_size;
                    argc--;
                    dim1 = atoi(*(++argv)) * mpi_size;
                    /* set default chunkdim sizes too */
                    chunkdim0 = (dim0 + 9) / 10;
                    chunkdim1 = (dim1 + 9) / 10;
                    break;
                case 'c': /* chunk dimensions */
                    if (--argc < 2) {
                        nerrors++;
                        return (1);
                    }
                    chunkdim0 = atoi(*(++argv));
                    argc--;
                    chunkdim1 = atoi(*(++argv));
                    break;
                case 'h': /* print help message--return with nerrors set */
                    return (1);
                default:
                    printf("Illegal option(%s)\n", *argv);
                    nerrors++;
                    return (1);
            }
        }
    } /*while*/

    /* check validity of dimension and chunk sizes */
    if (dim0 <= 0 || dim1 <= 0) {
        printf("Illegal dim sizes (%d, %d)\n", dim0, dim1);
        nerrors++;
        return (1);
    }
    if (chunkdim0 <= 0 || chunkdim1 <= 0) {
        printf("Illegal chunkdim sizes (%d, %d)\n", chunkdim0, chunkdim1);
        nerrors++;
        return (1);
    }

    /* Make sure datasets can be divided into equal portions by the processes */
    if ((dim0 % mpi_size) || (dim1 % mpi_size)) {
        if (MAINPROCESS)
            printf("dim0(%d) and dim1(%d) must be multiples of processes(%d)\n", dim0, dim1, mpi_size);
        nerrors++;
        return (1);
    }

    /* compose the test filenames */
    {
        int i, n;

        n = sizeof(FILENAME) / sizeof(FILENAME[0]) - 1; /* exclude the NULL */

        for (i = 0; i < n; i++)
            if (h5_fixname(FILENAME[i], fapl, filenames[i], PATH_MAX) == NULL) {
                printf("h5_fixname failed\n");
                nerrors++;
                return (1);
            }
        if (MAINPROCESS) {
            printf("Test filenames are:\n");
            for (i = 0; i < n; i++)
                printf("    %s\n", filenames[i]);
        }
    }

    return (0);
}

/*
 * Create the appropriate File access property list
 */
hid_t
create_faccess_plist(MPI_Comm comm, MPI_Info info, int l_facc_type)
{
    hid_t  ret_pl = H5I_INVALID_HID;
    herr_t ret;      /* generic return value */
    int    mpi_rank; /* mpi variables */

    /* need the rank for error checking macros */
    MPI_Comm_rank(MPI_COMM_WORLD, &mpi_rank);

    ret_pl = H5Pcreate(H5P_FILE_ACCESS);
    VRFY((ret_pl >= 0), "H5Pcreate succeeded");

    if (l_facc_type == FACC_DEFAULT)
        return (ret_pl);

    if (l_facc_type == FACC_MPIO) {
        /* set Parallel access with communicator */
        ret = H5Pset_fapl_mpio(ret_pl, comm, info);
        VRFY((ret >= 0), "H5Pset_fapl_mpio succeeded");
        ret = H5Pset_all_coll_metadata_ops(ret_pl, true);
        VRFY((ret >= 0), "H5Pset_all_coll_metadata_ops succeeded");
        ret = H5Pset_coll_metadata_write(ret_pl, true);
        VRFY((ret >= 0), "H5Pset_coll_metadata_write succeeded");
        return (ret_pl);
    }

    if (l_facc_type == (FACC_MPIO | FACC_SPLIT)) {
        hid_t mpio_pl;

        mpio_pl = H5Pcreate(H5P_FILE_ACCESS);
        VRFY((mpio_pl >= 0), "H5Pcreate succeeded");
        /* set Parallel access with communicator */
        ret = H5Pset_fapl_mpio(mpio_pl, comm, info);
        VRFY((ret >= 0), "H5Pset_fapl_mpio succeeded");

        /* setup file access template */
        ret_pl = H5Pcreate(H5P_FILE_ACCESS);
        VRFY((ret_pl >= 0), "H5Pcreate succeeded");
        /* set Parallel access with communicator */
        ret = H5Pset_fapl_split(ret_pl, ".meta", mpio_pl, ".raw", mpio_pl);
        VRFY((ret >= 0), "H5Pset_fapl_split succeeded");
        H5Pclose(mpio_pl);
        return (ret_pl);
    }

    /* unknown file access types */
    return (ret_pl);
}

int
main(int argc, char **argv)
{
    int             mpi_size, mpi_rank; /* mpi variables */
    int             mpi_code;
    H5Ptest_param_t ndsets_params, ngroups_params;
    H5Ptest_param_t collngroups_params;
    H5Ptest_param_t io_mode_confusion_params;
    H5Ptest_param_t rr_obj_flush_confusion_params;
#ifdef H5_HAVE_TEST_API
    int required = MPI_THREAD_MULTIPLE;
    int provided;
#endif

#ifndef H5_HAVE_WIN32_API
    /* Un-buffer the stdout and stderr */
    setbuf(stderr, NULL);
    setbuf(stdout, NULL);
#endif

#ifdef H5_HAVE_TEST_API
    /* Attempt to initialize with MPI_THREAD_MULTIPLE if possible */
    if (MPI_SUCCESS != (mpi_code = MPI_Init_thread(&argc, &argv, required, &provided))) {
        printf("MPI_Init_thread failed with error code %d\n", mpi_code);
        return -1;
    }
#else
    if (MPI_SUCCESS != (mpi_code = MPI_Init(&argc, &argv))) {
        printf("MPI_Init failed with error code %d\n", mpi_code);
        return -1;
    }
#endif

    if (MPI_SUCCESS != (mpi_code = MPI_Comm_rank(MPI_COMM_WORLD, &mpi_rank))) {
        printf("MPI_Comm_rank failed with error code %d\n", mpi_code);
        MPI_Finalize();
        return -1;
    }

#ifdef H5_HAVE_TEST_API
    /* Warn about missing MPI_THREAD_MULTIPLE support */
    if ((provided < required) && MAINPROCESS)
        printf("** MPI doesn't support MPI_Init_thread with MPI_THREAD_MULTIPLE **\n");
#endif

    if (MPI_SUCCESS != (mpi_code = MPI_Comm_size(MPI_COMM_WORLD, &mpi_size))) {
        if (MAINPROCESS)
            printf("MPI_Comm_size failed with error code %d\n", mpi_code);
        MPI_Finalize();
        return -1;
    }

    mpi_rank_framework_g = mpi_rank;

    dim0 = ROW_FACTOR * mpi_size;
    dim1 = COL_FACTOR * mpi_size;

    if (MAINPROCESS) {
        printf("===================================\n");
        printf("PHDF5 TESTS START\n");
        printf("===================================\n");
    }

    /* Attempt to turn off atexit post processing so that in case errors
     * happen during the test and the process is aborted, it will not get
     * hung in the atexit post processing in which it may try to make MPI
     * calls.  By then, MPI calls may not work.
     */
    if (H5dont_atexit() < 0) {
        printf("Failed to turn off atexit processing. Continue.\n");
    };
    H5open();
    h5_show_hostname();

    memset(filenames, 0, sizeof(filenames));
    for (int i = 0; i < NFILENAME; i++) {
        if (NULL == (filenames[i] = malloc(PATH_MAX))) {
            printf("couldn't allocate filename array\n");
            MPI_Abort(MPI_COMM_WORLD, -1);
        }
    }

    /* Set up file access property list with parallel I/O access */
    fapl = H5Pcreate(H5P_FILE_ACCESS);
    VRFY((fapl >= 0), "H5Pcreate succeeded");

    vol_cap_flags_g = H5VL_CAP_FLAG_NONE;

    /* Get the capability flag of the VOL connector being used */
    VRFY((H5Pget_vol_cap_flags(fapl, &vol_cap_flags_g) >= 0), "H5Pget_vol_cap_flags succeeded");

    /* Initialize testing framework */
    TestInit(argv[0], usage, parse_options);

    /* Tests are generally arranged from least to most complexity... */
    AddTest("mpiodup", test_fapl_mpio_dup, NULL, "fapl_mpio duplicate", NULL);
    AddTest("getdxplmpio", test_get_dxpl_mpio, NULL, "dxpl_mpio get", PARATESTFILE);

    AddTest("split", test_split_comm_access, NULL, "dataset using split communicators", PARATESTFILE);
    AddTest("h5oflusherror", test_oflush, NULL, "H5Oflush failure", PARATESTFILE);

    AddTest("page_buffer", test_page_buffer_access, NULL, "page buffer usage in parallel", PARATESTFILE);

    AddTest("props", test_file_properties, NULL, "Coll Metadata file property settings", PARATESTFILE);

    AddTest("delete", test_delete, NULL, "MPI-IO VFD file delete", PARATESTFILE);

    AddTest("invlibverassert", test_invalid_libver_bounds_file_close_assert, NULL,
            "Invalid libver bounds assertion failure", PARATESTFILE);

    AddTest("evictparassert", test_evict_on_close_parallel_unsupp, NULL, "Evict on close in parallel failure",
            PARATESTFILE);
    AddTest("fapl_preserve", test_fapl_preserve_hints, NULL, "preserve MPI I/O hints after fapl closed",
            PARATESTFILE);

    AddTest("idsetw", dataset_writeInd, NULL, "dataset independent write", PARATESTFILE);
    AddTest("idsetr", dataset_readInd, NULL, "dataset independent read", PARATESTFILE);

    AddTest("cdsetw", dataset_writeAll, NULL, "dataset collective write", PARATESTFILE);
    AddTest("cdsetr", dataset_readAll, NULL, "dataset collective read", PARATESTFILE);

    AddTest("eidsetw", extend_writeInd, NULL, "extendible dataset independent write", PARATESTFILE);
    AddTest("eidsetr", extend_readInd, NULL, "extendible dataset independent read", PARATESTFILE);
    AddTest("ecdsetw", extend_writeAll, NULL, "extendible dataset collective write", PARATESTFILE);
    AddTest("ecdsetr", extend_readAll, NULL, "extendible dataset collective read", PARATESTFILE);
    AddTest("eidsetw2", extend_writeInd2, NULL, "extendible dataset independent write #2", PARATESTFILE);
    AddTest("selnone", none_selection_chunk, NULL, "chunked dataset with none-selection", PARATESTFILE);
    AddTest("calloc", test_chunk_alloc, NULL, "parallel extend Chunked allocation on serial file",
            PARATESTFILE);
    AddTest("chkallocser2par", test_chunk_alloc_incr_ser_to_par, NULL,
            "chunk allocation from serial to parallel file access", PARATESTFILE);
    AddTest("fltread", test_filter_read, NULL, "parallel read of dataset written serially with filters",
            PARATESTFILE);

#ifdef H5_HAVE_FILTER_DEFLATE
    AddTest("cmpdsetr", compress_readAll, NULL, "compressed dataset collective read", PARATESTFILE);
#endif /* H5_HAVE_FILTER_DEFLATE */

    AddTest("zerodsetr", zero_dim_dset, NULL, "zero dim dset", PARATESTFILE);

    ndsets_params.name  = PARATESTFILE;
    ndsets_params.count = ndatasets;
    AddTest("ndsetw", multiple_dset_write, NULL, "multiple datasets write", &ndsets_params);

    ngroups_params.name  = PARATESTFILE;
    ngroups_params.count = ngroups;
    AddTest("ngrpw", multiple_group_write, NULL, "multiple groups write", &ngroups_params);
    AddTest("ngrpr", multiple_group_read, NULL, "multiple groups read", &ngroups_params);

    AddTest("compact", compact_dataset, NULL, "compact dataset test", PARATESTFILE);

    collngroups_params.name  = PARATESTFILE;
    collngroups_params.count = ngroups;
    /* combined cngrpw and ingrpr tests because ingrpr reads file created by cngrpw. */
    AddTest("cngrpw-ingrpr", collective_group_write_independent_group_read, NULL,
            "collective grp/dset write - independent grp/dset read", &collngroups_params);
#ifndef H5_HAVE_WIN32_API
    AddTest("bigdset", big_dataset, NULL, "big dataset test", PARATESTFILE);
#else
    printf("big dataset test will be skipped on Windows (JIRA HDDFV-8064)\n");
#endif
    AddTest("fill", dataset_fillvalue, NULL, "dataset fill value", PARATESTFILE);

    AddTest("cchunk1", coll_chunk1, NULL, "simple collective chunk io", PARATESTFILE);
    AddTest("cchunk2", coll_chunk2, NULL, "noncontiguous collective chunk io", PARATESTFILE);
    AddTest("cchunk3", coll_chunk3, NULL, "multi-chunk collective chunk io", PARATESTFILE);
    AddTest("cchunk4", coll_chunk4, NULL, "collective chunk io with partial non-selection ", PARATESTFILE);

    if ((mpi_size < 3) && MAINPROCESS) {
        printf("Collective chunk IO optimization APIs ");
        printf("needs at least 3 processes to participate\n");
        printf("Collective chunk IO API tests will be skipped \n");
    }
    AddTest((mpi_size < 3) ? "-cchunk5" : "cchunk5", coll_chunk5, NULL,
            "linked chunk collective IO without optimization", PARATESTFILE);
    AddTest((mpi_size < 3) ? "-cchunk6" : "cchunk6", coll_chunk6, NULL,
            "multi-chunk collective IO with direct request", PARATESTFILE);
    AddTest((mpi_size < 3) ? "-cchunk7" : "cchunk7", coll_chunk7, NULL,
            "linked chunk collective IO with optimization", PARATESTFILE);
    AddTest((mpi_size < 3) ? "-cchunk8" : "cchunk8", coll_chunk8, NULL,
            "linked chunk collective IO transferring to multi-chunk", PARATESTFILE);
    AddTest((mpi_size < 3) ? "-cchunk9" : "cchunk9", coll_chunk9, NULL,
            "multiple chunk collective IO with optimization", PARATESTFILE);
    AddTest((mpi_size < 3) ? "-cchunk10" : "cchunk10", coll_chunk10, NULL,
            "multiple chunk collective IO transferring to independent IO", PARATESTFILE);

    /* irregular collective IO tests*/
    AddTest("ccontw", coll_irregular_cont_write, NULL, "collective irregular contiguous write", PARATESTFILE);
    AddTest("ccontr", coll_irregular_cont_read, NULL, "collective irregular contiguous read", PARATESTFILE);
    AddTest("cschunkw", coll_irregular_simple_chunk_write, NULL, "collective irregular simple chunk write",
            PARATESTFILE);
    AddTest("cschunkr", coll_irregular_simple_chunk_read, NULL, "collective irregular simple chunk read",
            PARATESTFILE);
    AddTest("ccchunkw", coll_irregular_complex_chunk_write, NULL, "collective irregular complex chunk write",
            PARATESTFILE);
    AddTest("ccchunkr", coll_irregular_complex_chunk_read, NULL, "collective irregular complex chunk read",
            PARATESTFILE);

    AddTest("null", null_dataset, NULL, "null dataset test", PARATESTFILE);

    io_mode_confusion_params.name  = PARATESTFILE;
    io_mode_confusion_params.count = 0; /* value not used */

    AddTest("I/Omodeconf", io_mode_confusion, NULL, "I/O mode confusion test -- hangs quickly on failure",
            &io_mode_confusion_params);

    if ((mpi_size < 3) && MAINPROCESS) {
        printf("rr_obj_hdr_flush_confusion test needs at least 3 processes.\n");
        printf("rr_obj_hdr_flush_confusion test will be skipped \n");
    }
    if (mpi_size > 2) {
        rr_obj_flush_confusion_params.name  = PARATESTFILE;
        rr_obj_flush_confusion_params.count = 0; /* value not used */
        AddTest("rrobjflushconf", rr_obj_hdr_flush_confusion, NULL,
                "round robin object header flush confusion test", &rr_obj_flush_confusion_params);
    }

    AddTest("alnbg1", chunk_align_bug_1, NULL, "Chunk allocation with alignment bug.", PARATESTFILE);

    AddTest("tldsc", lower_dim_size_comp_test, NULL,
            "test lower dim size comp in span tree to mpi derived type", PARATESTFILE);

    AddTest("lccio", link_chunk_collective_io_test, NULL, "test mpi derived type management", PARATESTFILE);

    AddTest("actualio", actual_io_mode_tests, NULL, "test actual io mode proprerty", PARATESTFILE);

    AddTest("nocolcause", no_collective_cause_tests, NULL, "test cause for broken collective io",
            PARATESTFILE);

    AddTest("edpl", test_plist_ed, NULL, "encode/decode Property Lists", NULL);

    AddTest("extlink", external_links, NULL, "test external links", NULL);

    if ((mpi_size < 2) && MAINPROCESS) {
        printf("File Image Ops daisy chain test needs at least 2 processes.\n");
        printf("File Image Ops daisy chain test will be skipped \n");
    }
    AddTest((mpi_size < 2) ? "-fiodc" : "fiodc", file_image_daisy_chain_test, NULL,
            "file image ops daisy chain", NULL);

    /* Atomicity operations are not supported for OpenMPI versions < major
     * version 5 and will sporadically fail.
     */
#if defined(OPEN_MPI) && defined(OMPI_MAJOR_VERSION) && (OMPI_MAJOR_VERSION < 5)
    if (MAINPROCESS)
        printf("OpenMPI major version is < 5. Atomicity tests will be skipped due to support for atomicity "
               "operations not being implemented.\n");
#else
    if ((mpi_size < 2) && MAINPROCESS) {
        printf("Atomicity tests need at least 2 processes to participate\n");
        printf("8 is more recommended.. Atomicity tests will be skipped \n");
    }
    else if (facc_type != FACC_MPIO && MAINPROCESS) {
        printf("Atomicity tests will not work with a non MPIO VFD\n");
    }
    else if (mpi_size >= 2 && facc_type == FACC_MPIO) {
        AddTest("atomicity", dataset_atomicity, NULL, "dataset atomic updates", PARATESTFILE);
    }
#endif

    AddTest("denseattr", test_dense_attr, NULL, "Store Dense Attributes", PARATESTFILE);

    AddTest("noselcollmdread", test_partial_no_selection_coll_md_read, NULL,
            "Collective Metadata read with some ranks having no selection", PARATESTFILE);
    AddTest("MC_coll_MD_read", test_multi_chunk_io_addrmap_issue, NULL,
            "Collective MD read with multi chunk I/O (H5D__chunk_addrmap)", PARATESTFILE);
    AddTest("LC_coll_MD_read", test_link_chunk_io_sort_chunk_issue, NULL,
            "Collective MD read with link chunk I/O (H5D__sort_chunk)", PARATESTFILE);
    AddTest("GH_coll_MD_wr", test_collective_global_heap_write, NULL,
            "Collective MD write of global heap data", PARATESTFILE);
    AddTest("COLLIO_INDMDWR", test_coll_io_ind_md_write, NULL,
            "Collective I/O with Independent metadata writes", PARATESTFILE);

    /* Display testing information */
    TestInfo(argv[0]);

    /* setup file access property list */
    H5Pset_fapl_mpio(fapl, MPI_COMM_WORLD, MPI_INFO_NULL);

    /* Parse command line arguments */
    TestParseCmdLine(argc, argv);

    if (dxfer_coll_type == DXFER_INDEPENDENT_IO && MAINPROCESS) {
        printf("===================================\n"
               "   Using Independent I/O with file set view to replace collective I/O \n"
               "===================================\n");
    }

    /* Perform requested testing */
    PerformTests();

    /* make sure all processes are finished before final report, cleanup
     * and exit.
     */
    MPI_Barrier(MPI_COMM_WORLD);

    /* Display test summary, if requested */
    if (MAINPROCESS && GetTestSummary())
        TestSummary();

    /* Clean up test files */
    h5_delete_all_test_files(FILENAME, fapl);
    H5Pclose(fapl);

    nerrors += GetTestNumErrs();

    /* Gather errors from all processes */
    {
        int temp;
        MPI_Allreduce(&nerrors, &temp, 1, MPI_INT, MPI_MAX, MPI_COMM_WORLD);
        nerrors = temp;
    }

    if (MAINPROCESS) { /* only process 0 reports */
        printf("===================================\n");
        if (nerrors)
            printf("***PHDF5 tests detected %d errors***\n", nerrors);
        else
            printf("PHDF5 tests finished with no errors\n");
        printf("===================================\n");
    }

    for (int i = 0; i < NFILENAME; i++) {
        free(filenames[i]);
        filenames[i] = NULL;
    }

    /* close HDF5 library */
    H5close();

    /* Release test infrastructure */
    TestShutdown();

    /* MPI_Finalize must be called AFTER H5close which may use MPI calls */
    MPI_Finalize();

    /* cannot just return (nerrors) because exit code is limited to 1byte */
    return (nerrors != 0);
}
