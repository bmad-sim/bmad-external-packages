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
 * Test program: twriteorder
 *
 * Test to verify that the write order is strictly consistent.
 * The SWMR feature requires that the order of write is strictly consistent.
 * "Strict consistency in computer science is the most stringent consistency
 * model.  It says that a read operation has to return the result of the
 * latest write operation which occurred on that data item."--
 * (http://en.wikipedia.org/wiki/Linearizability#Definition_of_linearizability).
 * This is also an alternative form of what POSIX write require that after a
 * write operation has returned success, all reads issued afterward should
 * get the same data the write has written.
 *
 *************************************************************/

/***********************************************************
 *
 * Algorithm
 *
 * The test simulates what SWMR does by writing chained blocks and see if
 * they can be read back correctly.
 * There is a writer process and multiple read processes.
 * The file is divided into 2KB partitions. Then writer writes 1 chained
 * block, each of 1KB big, in each partition after the first partition.
 * Each chained block has this structure:
 * Byte 0-3: offset address of its child block. The last child uses 0 as NULL.
 * Byte 4-1023: some artificial data.
 * The child block address of Block 1 is NULL (0).
 * The child block address of Block 2 is the offset address of Block 1.
 * The child block address of Block n is the offset address of Block n-1.
 * After all n blocks are written, the offset address of Block n is written
 * to the offset 0 of the first partition.
 * Therefore, by the time the offset address of Block n is written to this
 * position, all n chain-linked blocks have been written.
 *
 * The other reader processes will try to read the address value at the
 * offset 0. The value is initially NULL(0). When it changes to non-zero,
 * it signifies the writer process has written all the chain-link blocks
 * and they are ready for the reader processes to access.
 *
 * If the system, in which the writer and reader processes run, the readers
 * will always get all chain-linked blocks correctly. If the order of write
 * is not maintained, some reader processes may found unexpected block data.
 *
 *************************************************************/

#include "h5test.h"

/* This test uses many POSIX things that are not available on
 * Windows.
 */
#if defined(H5_HAVE_FORK) && defined(H5_HAVE_WAITPID)

#define DATAFILE "twriteorder.dat"
/* #define READERS_MAX      10 */ /* max number of readers */
#define BLOCKSIZE_DFT     1024    /* 1KB */
#define PARTITION_DFT     2048    /* 2KB */
#define NLINKEDBLOCKS_DFT 512     /* default 512 */
#define SIZE_BLKADDR      4       /* expected sizeof blkaddr */
#define Hgoto_error(val)                                                                                     \
    do {                                                                                                     \
        ret_value = val;                                                                                     \
        goto done;                                                                                           \
    } while (0)

/* type declarations */
typedef enum part_t {
    UC_READWRITE = 0, /* both writer and reader */
    UC_WRITER,        /* writer only */
    UC_READER         /* reader only */
} part_t;

/* prototypes */
int  create_wo_file(void);
int  write_wo_file(void);
int  read_wo_file(void);
void usage(const char *prog);
int  setup_parameters(int argc, char *const argv[]);
int  parse_option(int argc, char *const argv[]);

/* Global Variable definitions */
const char *progname_g = "twriteorder"; /* program name */
int         write_fd_g;
int         blocksize_g, part_size_g, nlinkedblock_g;
part_t      launch_g;

/* Function definitions */

/* Show help page */
void
usage(const char *prog)
{
    fprintf(stderr, "usage: %s [OPTIONS]\n", prog);
    fprintf(stderr, "  OPTIONS\n");
    fprintf(stderr, "     -h            Print a usage message and exit\n");
    fprintf(stderr, "     -l w|r        launch writer or reader only. [default: launch both]\n");
    fprintf(stderr, "     -b N          Block size [default: %d]\n", BLOCKSIZE_DFT);
    fprintf(stderr, "     -p N          Partition size [default: %d]\n", PARTITION_DFT);
    fprintf(stderr, "     -n N          Number of linked blocks [default: %d]\n", NLINKEDBLOCKS_DFT);
    fprintf(stderr, "     where N is an integer value\n");
    fprintf(stderr, "\n");
}

/* Setup test parameters by parsing command line options.
 * Setup default values if not set by options. */
int
parse_option(int argc, char *const argv[])
{
    int ret_value = 0;
    int c;
    /* command line options: See function usage for a description */
    const char *cmd_options = "hb:l:n:p:";

    /* suppress getopt from printing error */
    opterr = 0;

    while (1) {
        c = getopt(argc, argv, cmd_options);
        if (-1 == c)
            break;

        switch (c) {
            case 'h':
                usage(progname_g);
                exit(EXIT_SUCCESS);
                break;
            case 'b': /* number of planes to write/read */
                if ((blocksize_g = atoi(optarg)) <= 0) {
                    fprintf(stderr, "bad blocksize %s, must be a positive integer\n", optarg);
                    usage(progname_g);
                    Hgoto_error(-1);
                }
                break;
            case 'n': /* number of planes to write/read */
                if ((nlinkedblock_g = atoi(optarg)) < 2) {
                    fprintf(stderr, "bad number of linked blocks %s, must be greater than 1.\n", optarg);
                    usage(progname_g);
                    Hgoto_error(-1);
                }
                break;
            case 'p': /* number of planes to write/read */
                if ((part_size_g = atoi(optarg)) <= 0) {
                    fprintf(stderr, "bad partition size %s, must be a positive integer\n", optarg);
                    usage(progname_g);
                    Hgoto_error(-1);
                }
                break;
            case 'l': /* launch reader or writer only */
                switch (*optarg) {
                    case 'r': /* reader only */
                        launch_g = UC_READER;
                        break;
                    case 'w': /* writer only */
                        launch_g = UC_WRITER;
                        break;
                    default:
                        fprintf(stderr, "launch value(%c) should be w or r only.\n", *optarg);
                        usage(progname_g);
                        Hgoto_error(-1);
                        break;
                } /* end inner switch */
                printf("launch = %d\n", launch_g);
                break;
            case '?':
                fprintf(stderr, "getopt returned '%c'.\n", c);
                usage(progname_g);
                Hgoto_error(-1);
            default:
                fprintf(stderr, "getopt returned unexpected value.\n");
                fprintf(stderr, "Unexpected value is %d\n", c);
                Hgoto_error(-1);
        } /* end outer switch */
    }     /* end while */

    /* verify partition size must be >= blocksize */
    if (part_size_g < blocksize_g) {
        fprintf(stderr, "Blocksize %d should not be bigger than partition size %d\n", blocksize_g,
                part_size_g);
        Hgoto_error(-1);
    }

done:
    /* All done. */
    return ret_value;
}

/* Setup parameters for the test case.
 * Return: 0 succeed; -1 fail.
 */
int
setup_parameters(int argc, char *const argv[])
{
    /* test case defaults */
    blocksize_g    = BLOCKSIZE_DFT;
    part_size_g    = PARTITION_DFT;
    nlinkedblock_g = NLINKEDBLOCKS_DFT;
    launch_g       = UC_READWRITE;

    /* parse options */
    if (parse_option(argc, argv) < 0) {
        return -1;
    }

    /* show parameters and return */
    printf("blocksize = %ld\n", (long)blocksize_g);
    printf("part_size = %ld\n", (long)part_size_g);
    printf("nlinkedblock = %ld\n", (long)nlinkedblock_g);
    printf("launch = %d\n", launch_g);

    return 0;
}

/* Create the test file with initial "empty" file, that is,
 * partition 0 has a null (0) address.
 *
 * Return: 0 succeed; -1 fail.
 */
int
create_wo_file(void)
{
    int               blkaddr     = 0;  /* blkaddress of next linked block */
    h5_posix_io_ret_t bytes_wrote = -1; /* # of bytes written   */

    /* Create the data file */
    if ((write_fd_g = HDopen(DATAFILE, O_RDWR | O_TRUNC | O_CREAT, H5_POSIX_CREATE_MODE_RW)) < 0) {
        printf("WRITER: error from open\n");
        return -1;
    }
    blkaddr = 0;
    /* write it to partition 0 */
    if ((bytes_wrote = HDwrite(write_fd_g, &blkaddr, (size_t)SIZE_BLKADDR)) != SIZE_BLKADDR) {
        printf("blkaddr write failed\n");
        return -1;
    }

    /* File initialized, return success */
    return 0;
}

int
write_wo_file(void)
{
    int               blkaddr;
    int               blkaddr_old = 0;
    int               i;
    char              buffer[BLOCKSIZE_DFT];
    h5_posix_io_ret_t bytes_wrote = -1; /* # of bytes written   */

    /* write block 1, 2, ... */
    for (i = 1; i < nlinkedblock_g; i++) {

        /* calculate where to write this block */
        blkaddr = i * part_size_g + i;

        /* store old block address in byte 0-3 */
        memcpy(&buffer[0], &blkaddr_old, sizeof(blkaddr_old));

        /* fill the rest with the lowest byte of i */
        memset(&buffer[4], i & 0xff, (size_t)(BLOCKSIZE_DFT - 4));

        /* write the block */
        HDlseek(write_fd_g, (HDoff_t)blkaddr, SEEK_SET);
        if ((bytes_wrote = HDwrite(write_fd_g, buffer, (size_t)blocksize_g)) != blocksize_g) {
            printf("blkaddr write failed in partition %d\n", i);
            return -1;
        }

        blkaddr_old = blkaddr;

    } /* end for */

    /* write the last blkaddr in partition 0 */
    HDlseek(write_fd_g, 0, SEEK_SET);
    if ((bytes_wrote = HDwrite(write_fd_g, &blkaddr_old, (size_t)sizeof(blkaddr_old))) !=
        sizeof(blkaddr_old)) {
        printf("blkaddr write failed in partition %d\n", 0);
        return -1;
    }

    /* all writes done. return success. */
    return 0;
}

int
read_wo_file(void)
{
    int               read_fd;
    int               blkaddr    = 0;
    h5_posix_io_ret_t bytes_read = -1; /* # of bytes actually read */
    char              buffer[BLOCKSIZE_DFT];

    /* Open the data file */
    if ((read_fd = HDopen(DATAFILE, O_RDONLY)) < 0) {
        printf("READER: error from open\n");
        return -1;
    }

    /* keep reading the initial block address until it is non-zero before proceeding. */
    while (blkaddr == 0) {
        HDlseek(read_fd, 0, SEEK_SET);
        if ((bytes_read = HDread(read_fd, &blkaddr, (size_t)sizeof(blkaddr))) != sizeof(blkaddr)) {
            printf("blkaddr read failed in partition %d\n", 0);
            return -1;
        }
    }

    /* got a non-zero blkaddr. Proceed down the linked blocks. */
    while (blkaddr != 0) {
        HDlseek(read_fd, (HDoff_t)blkaddr, SEEK_SET);
        if ((bytes_read = HDread(read_fd, buffer, (size_t)blocksize_g)) != blocksize_g) {
            printf("blkaddr read failed in partition %d\n", 0);
            return -1;
        }

        /* retrieve the block address in byte 0-3 */
        memcpy(&blkaddr, &buffer[0], sizeof(blkaddr));
    }

    return 0;
}

/* Overall Algorithm:
 * Parse options from user;
 * Generate/pre-created the test file needed and close it;
 * fork: child processes become the reader processes;
 *       while parent process continues as the writer process;
 * both run till ending conditions are met.
 */
int
main(int argc, char *argv[])
{
    /*pid_t childpid[READERS_MAX];
    int child_ret_value[READERS_MAX];*/
    pid_t childpid = 0;
    int   child_ret_value;
    pid_t mypid, tmppid;
    int   child_status;
    int   child_wait_option = 0;
    int   ret_value         = 0;

    /* initialization */
    if (setup_parameters(argc, argv) < 0) {
        Hgoto_error(1);
    }

    /* ==============================================================*/
    /* UC_READWRITE: create datafile, launch both reader and writer. */
    /* UC_WRITER:    create datafile, skip reader, launch writer.    */
    /* UC_READER:    skip create, launch reader, exit.               */
    /* ==============================================================*/
    /* ============*/
    /* Create file */
    /* ============*/
    if (launch_g != UC_READER) {
        printf("Creating skeleton data file for test...\n");
        if (create_wo_file() < 0) {
            fprintf(stderr, "***encounter error\n");
            Hgoto_error(1);
        }
        else
            printf("File created.\n");
    }
    /* flush output before possible fork */
    fflush(stdout);

    if (launch_g == UC_READWRITE) {
        /* fork process */
        if ((childpid = fork()) < 0) {
            perror("fork");
            Hgoto_error(1);
        }
    }
    mypid = HDgetpid();

    /* ============= */
    /* launch reader */
    /* ============= */
    if (launch_g != UC_WRITER) {
        /* child process launch the reader */
        if (0 == childpid) {
            printf("%d: launch reader process\n", mypid);
            if (read_wo_file() < 0) {
                fprintf(stderr, "read_wo_file encountered error\n");
                exit(EXIT_FAILURE);
            }

            /* Reader is done. Clean up by removing the data file */
            HDremove(DATAFILE);
            exit(EXIT_SUCCESS);
        }
    }

    /* ============= */
    /* launch writer */
    /* ============= */
    /* this process continues to launch the writer */
    if (write_wo_file() < 0) {
        fprintf(stderr, "write_wo_file encountered error\n");
        Hgoto_error(1);
    }

    /* ================================================ */
    /* If readwrite, collect exit code of child process */
    /* ================================================ */
    if (launch_g == UC_READWRITE) {
        if ((tmppid = waitpid(childpid, &child_status, child_wait_option)) < 0) {
            perror("waitpid");
            Hgoto_error(1);
        }
        if (WIFEXITED(child_status)) {
            if ((child_ret_value = WEXITSTATUS(child_status)) != 0) {
                printf("%d: child process exited with non-zero code (%d)\n", mypid, child_ret_value);
                Hgoto_error(2);
            }
        }
        else {
            printf("%d: child process terminated abnormally\n", mypid);
            Hgoto_error(2);
        }
    }

done:
    /* Print result and exit */
    if (ret_value != 0) {
        printf("Error(s) encountered\n");
    }
    else {
        printf("All passed\n");
    }

    return ret_value;
}

#else /* defined(H5_HAVE_FORK && defined(H5_HAVE_WAITPID) */

int
main(void)
{
    fprintf(stderr, "Non-POSIX platform. Skipping.\n");
    return EXIT_SUCCESS;
} /* end main() */

#endif /* defined(H5_HAVE_FORK && defined(H5_HAVE_WAITPID) */
