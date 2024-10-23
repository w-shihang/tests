/****************************************************************************
 * Included Files
 ****************************************************************************/
#include <nuttx/config.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <dirent.h>
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
 * Name: TestNuttxFsOpendir01
 ****************************************************************************/
void TestNuttxFsOpendir01(FAR void **state)
{
    DIR *dir;
    struct dirent *ptr;
    int ret;

    ret = mkdir("testopendir1", 0777);
    assert_int_equal(ret, 0);

    ret = mkdir("testopendir1/dir123", 0777);
    assert_int_equal(ret, 0);

    /* do opendir */
    dir = opendir("testopendir1");
    assert_non_null(dir);

    while ((ptr = readdir(dir)) != NULL)
    {
        if (strncmp(ptr->d_name, ".", 1) == 0)
            continue;
        if (strncmp(ptr->d_name, "..", 2) == 0)
            continue;
        if (strncmp(ptr->d_name, "dir123", 6) != 0)
        {
            closedir(dir);
            assert_true(0);
        }
    }

    /* close dir*/
    assert_int_equal(closedir(dir), 0);
}

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/
#define TEST_NUM 1000

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: TestNuttxFsOpendir02
 ****************************************************************************/
void TestNuttxFsOpendir02(FAR void **state)
{
    int ret;
    DIR *dir;
    struct dirent *ptr;

    /* mkdir for test */
    ret = mkdir("testopendir2", 0777);
    assert_int_equal(ret, 0);

    /* mkdir for test */
    ret = mkdir("testopendir2/dir_test2", 0777);
    assert_int_equal(ret, 0);

    for (int i = 0; i < TEST_NUM; i++)
    {
        /* open fir for test */
        dir = opendir("testopendir2");
        assert_true(dir != NULL);
        while ((ptr = readdir(dir)) != NULL)
        {
            if (strcmp(ptr->d_name, ".") == 0 || strcmp(ptr->d_name, "..") == 0)
            {
                continue;
            }
            else if (strcmp(ptr->d_name, "dir_test2") == 0)
            {
                break;
            }
        }
        /*close dir*/
        assert_int_equal(closedir(dir), 0);
    }
}