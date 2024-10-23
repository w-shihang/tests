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
#include <stdint.h>
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include "fstest.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/
#define TESTFILE "testOpenFile"

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: TestNuttxFsOpen01
 ****************************************************************************/

void TestNuttxFsOpen01(FAR void **state)
{
    int fd, ret;
    char s[] = "test data!", buffer[50] = {0};
    struct fs_testsuites_state_s *test_state;

    test_state = (struct fs_testsuites_state_s *)*state;

    /* open file */
    fd = open(TESTFILE, O_WRONLY | O_CREAT, 0700);
    assert_true(fd > 0);

    /* do write */
    int ret2 = write(fd, s, sizeof(s));
    close(fd);
    assert_int_in_range(ret2, 1, sizeof(s));

    /* open file*/
    fd = open(TESTFILE, O_RDONLY);
    assert_true(fd > 0);
    test_state->fd1 = fd;

    /* do read*/
    ret = read(fd, buffer, sizeof(buffer));
    assert_true(ret > 0);
}
