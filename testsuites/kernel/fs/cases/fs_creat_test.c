/****************************************************************************
 * Included Files
 ****************************************************************************/
#include <nuttx/config.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
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
#define TESTFILENAME "creatTestFile"

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: TestNuttxFsCreat01
 ****************************************************************************/
void TestNuttxFsCreat01(FAR void **state)
{
	int fd, ret;
	char buf[20] = {0};
	struct fs_testsuites_state_s *test_state;

	test_state = (struct fs_testsuites_state_s *)*state;

	sprintf(test_state->filename, "%s", TESTFILENAME);

	memset(buf, 'A', sizeof(buf));

	/* creat a test file */
	fd = creat(test_state->filename, 0700);
	assert_true(fd > 0);
	test_state->fd1 = fd;

	/* do write */
	ret = write(fd, buf, sizeof(buf));
	assert_int_in_range(ret, 1, 20);
}