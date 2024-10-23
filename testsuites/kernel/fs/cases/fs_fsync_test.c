/****************************************************************************
 * Included Files
 ****************************************************************************/
#include <nuttx/config.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/statfs.h>
#include <time.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include "fstest.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/
#define TESTFILE "FsyncTestFile"
#define BUF "testData123#$%*-=/ sdafasd37575sasdfasdf356345634563456ADSFASDFASDFQWREdf4as5df4as5dfsd ###"

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: TestNuttxFsFsync01
 ****************************************************************************/

void TestNuttxFsFsync01(FAR void **state)
{
    // clock_t start, finish;
    int fd, rval, ret;
    struct fs_testsuites_state_s *test_state;

    test_state = (struct fs_testsuites_state_s *)*state;

    /* open file */
    fd = open(TESTFILE, O_RDWR | O_CREAT, 0700);
    assert_true(fd > 0);
    test_state->fd1 = fd;

    for (int i = 0; i < 20; i++)
    {
        /* do write */
        rval = write(fd, BUF, sizeof(BUF));
        assert_int_equal(rval, sizeof(BUF));

        /* refresh to storage */
        ret = fsync(fd);
        assert_int_equal(ret, 0);
    }
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: TestNuttxFsFsync02
 ****************************************************************************/

void TestNuttxFsFsync02(FAR void **state)
{
    int fd, ret;
    char *buf = NULL;
    int bufsize = 4096;
    ssize_t writen = 0;
    struct statfs statfsbuf;
    struct fs_testsuites_state_s *test_state;

    test_state = (struct fs_testsuites_state_s *)*state;

    /* delete test file */
    unlink(TESTFILE);

    /* open file */
    fd = open(TESTFILE, O_CREAT | O_RDWR, 0777);
    assert_true(fd > 0);
    test_state->fd1 = fd;

    /* call fstatfs() */
    ret = fstatfs(fd, &statfsbuf);
    assert_int_equal(ret, 0);

    bufsize = statfsbuf.f_bsize;

    syslog(LOG_INFO, "the bsize = %d\n", statfsbuf.f_bsize);

    /* malloc memory */
    buf = malloc(bufsize * sizeof(char));
    assert_non_null(buf);
    test_state->ptr = buf;

    /* set memory */
    memset(buf, 0x66, bufsize);

    /* do write */
    writen = write(fd, buf, bufsize);
    assert_int_in_range(writen, 1, bufsize);

    /* refresh to storage */
    fsync(fd);

    /* call fstatfs() */
    ret = fstatfs(fd, &statfsbuf);
    assert_int_equal(ret, 0);
}