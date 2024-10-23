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
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: TestNuttxFsRename01
 ****************************************************************************/
void TestNuttxFsRename01(FAR void **state)
{
    int fd, status, ret;
    char buffer[50];
    char filename1[] = "testRenameFile1";
    struct fs_testsuites_state_s *test_state;

    test_state = (struct fs_testsuites_state_s *)*state;

    /* open file */
    fd = open(filename1, O_WRONLY | O_CREAT, 0700);
    assert_true(fd > 0);
    test_state->fd1 = fd;

    /* set memory */
    memset(buffer, '*', 50);

    /* do write */
    ret = write(fd, buffer, 50);
    assert_int_in_range(ret, 1, 50);

    /* do rename */
    status = rename(filename1, "newNameFile1");
    assert_int_equal(status, 0);
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: TestNuttxFsRename02
 ****************************************************************************/
void TestNuttxFsRename02(FAR void **state)
{
    int status;

    /* make directory */
    status = mkdir("testdir1", 0700);
    assert_int_equal(status, 0);

    /* rename directory */
    status = rename("testdir1", "newtestdir1");
    assert_int_equal(status, 0);
}