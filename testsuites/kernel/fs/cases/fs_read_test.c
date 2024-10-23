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
#define TESTFILE "testRead01File1"

/****************************************************************************
 * Private Functions
 ****************************************************************************/

/****************************************************************************
 * Name: TestNuttxFsRead01
 ****************************************************************************/
void TestNuttxFsRead01(FAR void **state)
{
	int fd, size;
	char s[] = "Test!", buffer[80];
	struct fs_testsuites_state_s *test_state;

	test_state = (struct fs_testsuites_state_s *)*state;

	fd = open(TESTFILE, O_WRONLY | O_CREAT, 0777);
	assert_true(fd > 0);
	test_state->fd1 = fd;

	size = write(fd, s, sizeof(s));
	assert_int_equal(size, sizeof(s));

	close(fd);
	fd = open(TESTFILE, O_RDONLY, 0777);
	assert_true(fd > 0);
	test_state->fd1 = fd;
	size = read(fd, buffer, sizeof(buffer));
	assert_int_equal(size, sizeof(s));
}
