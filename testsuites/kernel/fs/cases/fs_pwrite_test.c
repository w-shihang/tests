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
#include <stdint.h>
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include "fstest.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/
#define TEST_FILE "pwrite_file"
#define BUF_SIZE 4

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: TestNuttxFsPwrite01
 ****************************************************************************/
void TestNuttxFsPwrite01(FAR void **state)
{
    int fd;
    char *buf;
    struct fs_testsuites_state_s *test_state;

    test_state = (struct fs_testsuites_state_s *)*state;

    /* open file */
    fd = open(TEST_FILE, O_RDWR | O_CREAT, 0777);
    assert_true(fd > 0);
    test_state->fd1 = fd;

    /* malloc memory*/
    buf = malloc(BUF_SIZE * sizeof(char));
    assert_non_null(buf);
    test_state->ptr = buf;

    /* set memory */
    memset(buf, 'A', BUF_SIZE);

    /* do write */
    int ret = write(fd, buf, BUF_SIZE);
    assert_int_in_range(ret, 1, BUF_SIZE);

    /* reset file pos use fd */
    lseek(fd, 0, SEEK_SET);

    /* set memory */
    memset(buf, 'B', BUF_SIZE);

    /* do pwrite */
    pwrite(fd, buf, BUF_SIZE >> 1, 0);

    /* set memory */
    memset(buf, 'C', BUF_SIZE);

    /* do pwrite */
    pwrite(fd, buf, BUF_SIZE >> 2, 0);

    /* do read */
    assert_int_in_range(read(fd, buf, BUF_SIZE), 1, BUF_SIZE);

    /* check buf*/
    assert_true(strncmp(buf, "CBAA", BUF_SIZE) == 0);
}