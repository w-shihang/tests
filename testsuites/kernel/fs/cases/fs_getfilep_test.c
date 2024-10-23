/****************************************************************************
 * Included Files
 ****************************************************************************/
#include <nuttx/config.h>
#include <stdio.h>
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
#define TEST_FILE "fstat_test_file"
#define BUF_SIZE 512

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: TestNuttxFsGetfilep01
 ****************************************************************************/

void TestNuttxFsGetfilep01(FAR void **state)
{
    FAR struct file *filep;
    int ret, fd;
    char *buf = NULL;
    FILE *fp;
    struct fs_testsuites_state_s *test_state;

    test_state = (struct fs_testsuites_state_s *)*state;

    /* create a file for testing */
    fd = creat(TEST_FILE, 0700);
    assert_true(fd > 0);
    test_state->fd1 = fd;

    /* fdopen file */
    fp = fdopen(fd, "r+");
    assert_non_null(fp);

    /* get struct file */
    ret = fs_getfilep(fileno(fp), &filep);
    assert_int_equal(ret, 0);

    /* malloc memory */
    buf = malloc(BUF_SIZE);
    assert_non_null(buf);
    test_state->ptr = buf;

    /* set memory */
    memset(buf, 'A', BUF_SIZE);

    /* do write*/
    ret = write(fileno(fp), buf, BUF_SIZE);
    assert_int_equal(ret, BUF_SIZE);

    /* do fflush */
    fflush(fp);

    /* do fsync */
    fsync(fileno(fp));

    /* put filep */
    fs_putfilep(filep);

    /* get struct file again*/
    ret = fs_getfilep(fileno(fp), &filep);
    assert_int_equal(ret, 0);

    assert_int_equal(filep->f_pos, BUF_SIZE);

    test_state->fd2 = fileno(fp);

    /* put filep */
    fs_putfilep(filep);

    assert_int_equal(fclose(fp), 0);
}