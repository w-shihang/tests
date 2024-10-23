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
#define TESTFILE "testWriteFile"
#define MAXLEN 1024

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: TestNuttxFsWrite01
 ****************************************************************************/
void TestNuttxFsWrite01(FAR void **state)
{
    int out, rval;
    char buffer[1024];
    struct fs_testsuites_state_s *test_state;

    test_state = (struct fs_testsuites_state_s *)*state;

    /* open file */
    out = open(TESTFILE, O_WRONLY | O_CREAT, 0700);
    assert_true(out > 0);
    test_state->fd1 = out;

    /* set memory */
    memset(buffer, '*', MAXLEN);

    /* do write */
    rval = write(out, buffer, MAXLEN);
    assert_int_in_range(rval, 1, MAXLEN);
}

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/
#define TESTFILENAME "loopTestFile"

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: TestNuttxFsWrite02
 ****************************************************************************/
void TestNuttxFsWrite02(FAR void **state)
{
    int rval;
    FILE *fp;
    long offset;
    char content[15] = "asdfgtgtrf";
    struct fs_testsuites_state_s *test_state;

    test_state = (struct fs_testsuites_state_s *)*state;

    /* fopen file */
    fp = fopen(TESTFILENAME, "a+");
    assert_non_null(fp);
    test_state->fd1 = fileno(fp);

    /* do fwrite */
    rval = fwrite(content, 1, 10, fp);
    assert_int_in_range(rval, 1, 10);

    /* refresh to storage */
    fsync(fileno(fp));

    /* do ffkush */
    fflush(fp);

    /* get ftell */
    offset = ftell(fp);
    assert_true(offset == 10);
    fclose(fp);
}

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/
#define TESTFILENAME3 "loopTestFile"

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: TestNuttxFsWrite03
 ****************************************************************************/
void TestNuttxFsWrite03(FAR void **state)
{
    int rval;
    FILE *fp;
    long offset;
    char content[15] = "asdfgtgtrf";
    struct fs_testsuites_state_s *test_state;

    test_state = (struct fs_testsuites_state_s *)*state;

    /* fopen file */
    fp = fopen(TESTFILENAME3, "a+");
    assert_non_null(fp);
    test_state->fd1 = fileno(fp);

    /* do fwrite */
    rval = fwrite(content, 1, 10, fp);
    assert_int_in_range(rval, 1, 10);

    /* refresh to storage */
    fsync(fileno(fp));

    /* get ftell */
    offset = ftell(fp);
    assert_true(offset == 10);
    fclose(fp);
}
