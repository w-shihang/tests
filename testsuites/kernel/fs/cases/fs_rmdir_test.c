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
#include "nsh.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/
#define PARENTDIR1 "parentDirName"

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: TestNuttxFsRmdir01
 ****************************************************************************/
void TestNuttxFsRmdir01(FAR void **state)
{
    int status;
    int fd;
    char str[5];
    char testFileName[20] = {0};
    char testDirName[20] = {0};
    char absoluteDirectory[100] = {0};
    char currentPath[100] = {0};
    char parentDirectory[PATH_MAX] = {0};

    unsigned long long size;
    size = cm_get_partition_available_size();
    if (size == (unsigned long long)-1)
    {
        fail_msg("Failed to obtain partition information !\n");
    }
    /* Stop the test if the available space of the partition is less than 160K */
    if (size < 163840)
    {
        syslog(LOG_WARNING, "Partitioned free space not enough !\n");
        syslog(LOG_WARNING, "Test case (%s) exits early !\n", __func__);
    }
    else
    {
        /* create directory */
        status = mkdir(PARENTDIR1, 0700);
        assert_int_equal(status, 0);

        /* get test path */
        getcwd(currentPath, sizeof(currentPath));

        strcpy(absoluteDirectory, currentPath);
        strcat(currentPath, "/");
        strcat(currentPath, PARENTDIR1);

        strcpy(parentDirectory, currentPath);

        chdir(currentPath);

        /* get test path */
        getcwd(currentPath, sizeof(currentPath));

        /* create 10 2-level subfolders */
        for (int i = 0; i < 10; i++)
        {
            itoa(i, str, 10);
            status = mkdir(str, 0700);
            assert_int_equal(status, 0);
        }

        /* switch to directory 5 */
        itoa(5, str, 10);

        /* enter sub-directory */
        strcat(currentPath, "/");
        strcat(currentPath, str);
        chdir(currentPath);

        /* get test path */
        getcwd(currentPath, sizeof(currentPath));

        /* make directory */
        status = mkdir("test_3_dir_1", 0700);
        assert_int_equal(status, 0);

        /* make directory */
        status = mkdir("test_3_dir_2", 0700);
        assert_int_equal(status, 0);

        /* switch to directory 8 */
        itoa(8, str, 10);

        /* enter sub-directory */
        memset(currentPath, 0, sizeof(currentPath));
        strcpy(currentPath, parentDirectory);
        strcat(currentPath, "/");
        strcat(currentPath, str);
        chdir(currentPath);

        /* get test path */
        getcwd(currentPath, sizeof(currentPath));

        for (int j = 1; j <= 10; j++)
        {
            sprintf(testFileName, "test_3_file_%d", j);

            /* creat a test file */
            fd = creat(testFileName, 0700);
            assert_true(fd > 0);
            close(fd);

            /* set memory */
            memset(testFileName, 0, sizeof(testFileName));
        }

        /* switch to directory 2 */
        itoa(2, str, 10);

        /* enter sub-directory */
        memset(currentPath, 0, sizeof(currentPath));
        strcpy(currentPath, parentDirectory);
        strcat(currentPath, "/");
        strcat(currentPath, str);
        chdir(currentPath);

        /* get test path */
        getcwd(currentPath, sizeof(currentPath));

        for (int k = 1; k <= 5; k++)
        {
            sprintf(testFileName, "test_3_file_%d", k);
            sprintf(testDirName, "test_3_dir_%d", k);

            /* create a test file */
            fd = creat(testFileName, 0700);
            assert_true(fd > 0);
            close(fd);
            /* make  directory */
            status = mkdir(testDirName, 0700);
            assert_int_equal(status, 0);

            /* set memory */
            memset(testFileName, 0, sizeof(testFileName));
            memset(testDirName, 0, sizeof(testDirName));
        }

        /* wwitch to the test absolute directory */
        chdir(absoluteDirectory);

        /* call the recursive delete interface */
        cm_unlink_recursive(parentDirectory);
    }
}

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/
#define PARENTDIR2 "parentDirName2"

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: TestNuttxFsRmdir02
 ****************************************************************************/
void TestNuttxFsRmdir02(FAR void **state)
{
    int status;
    int ret;
    char str[20] = {0};
    char absoluteDirectory[20] = {0};
    char parentDirectory[PATH_MAX] = {0};
    char temporaryPath[300] = {0};
    unsigned long long size;

    size = cm_get_partition_available_size();
    if (size == (unsigned long long)-1)
    {
        fail_msg("Failed to obtain partition information !\n");
    }
    /* Stop the test if the available space of the partition is less than 98K */
    if (size < 98304)
    {
        syslog(LOG_WARNING, "Partitioned free space not enough !\n");
        syslog(LOG_WARNING, "Test case (%s) exits early !\n", __func__);
    }
    else
    {

        getcwd(absoluteDirectory, sizeof(absoluteDirectory));

        /* create directory */
        status = mkdir(PARENTDIR2, 0700);
        assert_int_equal(status, 0);

        strcpy(parentDirectory, absoluteDirectory);
        strcat(parentDirectory, "/");
        strcat(parentDirectory, PARENTDIR2);

        /* switch to test PARENTDIR*/
        chdir(parentDirectory);

        /* create a 6-level directory in a loop */
        for (int i = 0; i < 6; i++)
        {
            /* get current path */
            getcwd(temporaryPath, sizeof(temporaryPath));
            strcat(temporaryPath, "/");

            /* do snprintf */
            ret = snprintf(str, 20, "test_dir_%d", i);
            assert_true(ret > 0);

            strcat(temporaryPath, str);
            /* make directory */
            status = mkdir(temporaryPath, 0700);
            assert_int_equal(status, 0);

            chdir(temporaryPath);

            /* set memory */
            memset(temporaryPath, 0, sizeof(temporaryPath));
            memset(str, 0, sizeof(str));
        }

        /* wwitch to the test absolute directory */
        chdir(absoluteDirectory);

        /* call the recursive delete interface */
        cm_unlink_recursive(parentDirectory);
    }
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: TestNuttxFsRmdir03
 ****************************************************************************/
void TestNuttxFsRmdir03(FAR void **state)
{
    int status;
    DIR *dir = NULL;
    char str[5];
    char buf[20] = {0};
    struct dirent *ptr;

    unsigned long long size;
    size = cm_get_partition_available_size();
    if (size == (unsigned long long)-1)
    {
        fail_msg("Failed to obtain partition information !\n");
    }
    /* Stop the test if the available space of the partition is less than 80K */
    if (size < 81920)
    {
        syslog(LOG_WARNING, "Partitioned free space not enough !\n");
        syslog(LOG_WARNING, "Test case (%s) exits early !\n", __func__);
    }
    else
    {

        for (int i = 0; i < 5; i++)
        {
            itoa(i, str, 10);

            /* make directory */
            status = mkdir(str, 0700);
            assert_int_equal(status, 0);
        }
        getcwd(buf, sizeof(buf));

        /* open directory */
        dir = opendir(buf);
        while ((ptr = readdir(dir)) != NULL)
        {
            status = rmdir(ptr->d_name);
        }

        /* close directory flow */
        assert_int_equal(closedir(dir), 0);
    }
}
