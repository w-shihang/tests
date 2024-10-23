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
#define TESTFILE "fileSeekTest"

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: TestNuttxFsSeek01
 ****************************************************************************/
void TestNuttxFsSeek01(FAR void **state)
{
    FILE *fp;
    char c[] = "This is fseek test !";
    char buffer[sizeof(c)];
    int ret;
    struct fs_testsuites_state_s *test_state;

    test_state = (struct fs_testsuites_state_s *)*state;

    /* open file*/
    fp = fopen(TESTFILE, "w+");
    assert_non_null(fp);
    test_state->fd1 = fileno(fp);

    /* do fwrite */
    fwrite(c, strlen(c) + 1, 1, fp);

    /* reset file pos use fp */
    ret = fseek(fp, 8, SEEK_SET);
    assert_int_equal(ret, 0);

    /* do fread */
    ret = fread(buffer, 1, strlen(c) + 1, fp);
    buffer[ret] = 0;
    assert_int_equal(strcmp(buffer, "fseek test !"), 0);

    /* reset file pos use fp */
    ret = fseek(fp, 5, SEEK_SET);
    assert_int_equal(ret, 0);

    /* do read */
    ret = fread(buffer, 1, strlen(c) + 1, fp);
    buffer[ret] = 0;
    assert_int_equal(strcmp(buffer, "is fseek test !"), 0);

    /* reset file pos use fp */
    ret = fseek(fp, 14, SEEK_SET);
    assert_int_equal(ret, 0);

    /* do fread */
    ret = fread(buffer, 1, strlen(c) + 1, fp);
    buffer[ret] = 0;
    assert_int_equal(strcmp(buffer, "test !"), 0);
    fclose(fp);
}

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/
#define WRITE_STR "abcdefg"
#define TFILE "tfile"

/****************************************************************************
 * Private Data
 ****************************************************************************/
static int fd;
static struct tcase
{
    off_t off;
    int whence;
    char *wname;
    off_t exp_off;
    ssize_t exp_size;
    char *exp_data;
} tcases[] = {
    {4, SEEK_SET, "SEEK_SET", 4, 3, "efg"},
    {-2, SEEK_CUR, "SEEK_CUR", 5, 2, "fg"},
    {-4, SEEK_END, "SEEK_END", 3, 4, "defg"},
    {0, SEEK_END, "SEEK_END", 7, 0, NULL},
};

/****************************************************************************
 * Private Functions
 ****************************************************************************/

/****************************************************************************
 * Name: TestNuttxFsVerifyLseek
 ****************************************************************************/
static int TestNuttxFsVerifyLseek(unsigned int n)
{
    char read_buf[64];
    struct tcase *tc = &tcases[n];
    int ret;
    /* do read */
    ssize_t sn = read(fd, read_buf, sizeof(read_buf));
    if (sn < 0)
    {
        return -1;
    }

    /* set memory */
    memset(read_buf, 0, sizeof(read_buf));

    /* do lseek */
    ret = lseek(fd, tc->off, tc->whence);
    if (ret == (off_t)-1)
    {
        syslog(LOG_ERR, "lseek(%s, %lld, %s) failed\n", TFILE, (long long)tc->off, tc->wname);
        return -1;
    }

    if (ret != tc->exp_off)
    {
        syslog(LOG_ERR, "lseek(%s, %lld, %s) returned %d, expected %lld\n", TFILE, (long long)tc->off, tc->wname, ret, (long long)tc->exp_off);
        return -1;
    }

    /* do read*/
    sn = read(fd, read_buf, tc->exp_size);
    if (sn < 0)
    {
        return -1;
    }
    if (tc->exp_data && strcmp(read_buf, tc->exp_data))
    {
        syslog(LOG_ERR, "lseek(%s, %lld, %s) read incorrect data\n", TFILE, (long long)tc->off, tc->wname);
        return -1;
    }
    else
    {
        return 0;
    }
}

/****************************************************************************
 * Private Functions
 ****************************************************************************/

/****************************************************************************
 * Name: TestNuttxFsSet
 ****************************************************************************/
static void TestNuttxFsSet(void)
{
    fd = open(TFILE, O_RDWR | O_CREAT, 0700);
    assert_true(fd >= 0);
    write(fd, WRITE_STR, sizeof(WRITE_STR) - 1);
}

/****************************************************************************
 * Private Functions
 ****************************************************************************/

/****************************************************************************
 * Name: TestNuttxFsClean
 ****************************************************************************/
static void TestNuttxFsClean(void)
{
    if (fd > 0)
        close(fd);
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: TestNuttxFsSeek02
 ****************************************************************************/
void TestNuttxFsSeek02(FAR void **state)
{
    int ret;
    struct fs_testsuites_state_s *test_state;

    test_state = (struct fs_testsuites_state_s *)*state;
    TestNuttxFsSet();
    test_state->fd1 = fd;
    /* do verify lseek*/
    ret = TestNuttxFsVerifyLseek(0);
    assert_int_equal(ret, 0);

    /* do verify lseek*/
    ret = TestNuttxFsVerifyLseek(1);
    assert_int_equal(ret, 0);

    /* do verify lseek*/
    ret = TestNuttxFsVerifyLseek(2);
    assert_int_equal(ret, 0);

    /* do verify lseek*/
    ret = TestNuttxFsVerifyLseek(3);
    assert_int_equal(ret, 0);

    /* do clean*/
    TestNuttxFsClean();
}