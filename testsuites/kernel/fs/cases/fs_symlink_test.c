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
#define TEST_FILE "test_file"
#define PATH_MAX_SIZE 64

/****************************************************************************
 * Private Data
 ****************************************************************************/
static char *path;

/****************************************************************************
 * Private Functions
 ****************************************************************************/

/****************************************************************************
 * Name: TestNuttxFsRunTest
 ****************************************************************************/

static int TestNuttxFsRunTest(void)
{
    int ret;
    /* test symlink */
    char buf[64] = {0};
    ret = symlink(path, "/file_link");
    // syslog(LOG_INFO, "the symlink return : %d\n", ret);
    if (ret != 0)
    {
        return ERROR;
    }
    else
    {
        ret = readlink("/file_link", buf, PATH_MAX_SIZE);
        // syslog(LOG_INFO, "buf = %s\n", buf);
        unlink("/file_link");
    }
    return OK;
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: TestNuttxFsSymlink01
 ****************************************************************************/
void TestNuttxFsSymlink01(FAR void **state)
{
    int fd, ret;
    char* buf;
    struct fs_testsuites_state_s *test_state;

    test_state = (struct fs_testsuites_state_s *)*state;

    /* malloc memory */
    path = malloc(PATH_MAX_SIZE * sizeof(char));
    assert_non_null(path);
    test_state->ptr = path;

    /* set memory */
    buf = getcwd(NULL, 0);
    memset(path, '\0', PATH_MAX_SIZE);
    sprintf(path, "%s/symlink_test_file", buf);
    free(buf);

    /* open file*/
    fd = open(path, O_WRONLY | O_CREAT, 0700);
    assert_true(fd > 0);
    test_state->fd1 = fd;

    /* do run test */
    ret = TestNuttxFsRunTest();

    /* do remove */
    unlink(path);

    assert_int_equal(ret, OK);
}
