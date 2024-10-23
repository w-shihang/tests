/****************************************************************************
 * Included Files
 ****************************************************************************/
#include <nuttx/config.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
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
#define TEST_FILE "readlink_test_file"
#define PATH_MAX_SIZE 64

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: TestNuttxFsReadlink01
 ****************************************************************************/
void TestNuttxFsReadlink01(FAR void **state)
{
    int ret, fd;

    /* test symlink */
    char path[PATH_MAX_SIZE] = {0};
    char buf[PATH_MAX_SIZE] = {0};
    struct fs_testsuites_state_s *test_state;

    test_state = (struct fs_testsuites_state_s *)*state;

    /* creat file */
    fd = creat(TEST_FILE, 0700);
    assert_true(fd > 0);
    test_state->fd1 = fd;

    getcwd(path, sizeof(path));
    strcat(path, "/");
    strcat(path, TEST_FILE);

    /* creating a soft connection */
    ret = symlink(path, "/file_link");
    assert_int_equal(ret, 0);

    /* read link */
    ret = readlink("/file_link", buf, PATH_MAX_SIZE);
    assert_true(ret == strlen(path));

    /* delete test file */
    assert_int_equal(unlink("/file_link"), 0);
}