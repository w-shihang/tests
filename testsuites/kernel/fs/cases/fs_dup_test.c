/****************************************************************************
 * Included Files
 ****************************************************************************/
#include <nuttx/config.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <syslog.h>
#include <errno.h>
#include <stdint.h>
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include "fstest.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/
#define TESTFILENAME "testDupFile"

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: TestNuttxFsDup01
 ****************************************************************************/
void TestNuttxFsDup01(FAR void **state)
{
    int fd, newfd, rval;
    char bufFD[5] = "hello";
    char bufNewFD[8] = "littleFS";
    char readBuf[20] = "";
    struct fs_testsuites_state_s *test_state;

    test_state = (struct fs_testsuites_state_s *)*state;
    /* open file */
    fd = open(TESTFILENAME, O_RDWR | O_CREAT | O_APPEND, 0700);
    assert_true(fd > 0);
    test_state->fd1 = fd;

    /* do write */
    assert_int_in_range(write(fd, bufFD, sizeof(bufFD)), 1, sizeof(bufFD));

    /* refresh to storage */
    assert_int_equal(fsync(fd), 0);

    /* do dup */
    newfd = dup(fd);
    close(fd);
    assert_int_not_equal(newfd, -1);
    test_state->fd2 = newfd;

    /* check if file pos is shared */
    off_t currpos = lseek(newfd, 0, SEEK_CUR);
    assert_int_equal(currpos, 5);

    /* write newfd after dup */
    rval = write(newfd, bufNewFD, sizeof(bufNewFD));
    assert_int_in_range(rval, 1, sizeof(bufNewFD));

    /* refresh to storage */
    assert_int_equal(fsync(newfd), 0);

    /* reset file pos use newfd */
    off_t ret = lseek(newfd, 0, SEEK_SET);
    assert_int_equal(ret, 0);

    /* do double check */
    rval = read(newfd, readBuf, 20);
    assert_int_in_range(rval, 1, 20);

    /* check readBuf*/
    assert_int_equal(strncmp(readBuf, "hellolittleFS", 13), 0);
}
