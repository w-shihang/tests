/****************************************************************************
 * Included Files
 ****************************************************************************/
#include <nuttx/config.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/vfs.h>
#include <dirent.h>
#include <syslog.h>
#include <sys/stat.h>
#include <stdint.h>
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include "fstest.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/
#define MAXSIZE 1024
#define test_file "test_unlink_file01"

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: TestNuttxFsUnlink01
 ****************************************************************************/
void TestNuttxFsUnlink01(FAR void **state)
{
    struct fs_testsuites_state_s *test_state;

    test_state = (struct fs_testsuites_state_s *)*state;

    /* open file */
    int fd = open(test_file, O_RDWR | O_CREAT, 0777);
    assert_true(fd > 0);
    test_state->fd1 = fd;

    char buf[MAXSIZE] = {0};

    /* set memory */
    memset(buf, 65, MAXSIZE);

    /* do write */
    int size = write(fd, buf, MAXSIZE);
    assert_int_in_range(size, 1, MAXSIZE);

    close(fd);

    /* delete test file */
    int ret = unlink(test_file);
    assert_int_equal(ret, 0);

    /* check if the file was deleted successfully */
    fd = open(test_file, O_RDONLY);
    assert_int_equal(fd, -1);
}