/****************************************************************************
 * Included Files
 ****************************************************************************/
#include <nuttx/config.h>
#include <stdio.h>
#include <string.h>
#include <utime.h>
#include <time.h>
#include <syslog.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include "fstest.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/
#define TEST_FILE "stat_test_file"
#define BUF_SIZE 1024

/****************************************************************************
 * Private Functions
 ****************************************************************************/

/****************************************************************************
 * Name: TestNuttxFsPrintTime
 ****************************************************************************/
__attribute__((unused)) static void TestNuttxFsPrintTime(struct tm *TM)
{
	syslog(LOG_INFO, "    tm_year: %d\n", TM->tm_year + 1900);
	syslog(LOG_INFO, "    tm_mon: %d\n", TM->tm_mon);
	syslog(LOG_INFO, "    tm_mday: %d\n", TM->tm_mday);
	syslog(LOG_INFO, "    tm_hour: %d\n", TM->tm_hour);
	syslog(LOG_INFO, "    tm_min: %d\n", TM->tm_min);
	syslog(LOG_INFO, "    tm_sec: %d\n", TM->tm_sec);
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: TestNuttxFsStat01
 ****************************************************************************/
void TestNuttxFsStat01(FAR void **state)
{
	int fd;
	int ret, ret2;
	struct stat file_s;
	char buf[BUF_SIZE] = {0};
	struct tm *tm_1 = NULL, *tm_2 = NULL;
	int year1, year2, month1, month2, day1, day2, hour1, hour2, min1, min2;
	time_t t_1, t_2;
	struct fs_testsuites_state_s *test_state;

	test_state = (struct fs_testsuites_state_s *)*state;

	/* set memory */
	memset(buf, 65, BUF_SIZE);

	/* open file */
	fd = open(TEST_FILE, O_RDWR | O_CREAT, 0777);
	assert_true(fd > 0);
	test_state->fd1 = 0;

	/* do write*/
	ret2 = write(fd, buf, BUF_SIZE);
	assert_int_in_range(ret2, 1, 1024);

	close(fd);

	/* get system time */
	time(&t_1);
	tm_1 = gmtime(&t_1);
	assert_non_null(tm_1);

	/* set time */
	year1 = tm_1->tm_year;
	month1 = tm_1->tm_mon;
	day1 = tm_1->tm_mday;
	hour1 = tm_1->tm_hour;
	min1 = tm_1->tm_min;

	/* get file info */
	ret = stat(TEST_FILE, &file_s);
	assert_int_equal(ret, 0);

	/* output stat struct information */
	t_2 = file_s.st_mtime;
	tm_2 = gmtime(&t_2);

	assert_non_null(tm_2);

	/* set time */
	year2 = tm_2->tm_year;
	month2 = tm_2->tm_mon;
	day2 = tm_2->tm_mday;
	hour2 = tm_2->tm_hour;
	min2 = tm_2->tm_min;

	/* compare time and size */

	assert_int_equal(year1, year2);
	assert_int_equal(month1, month2);
	assert_int_equal(day1, day2);
	assert_int_equal(hour1, hour2);
	assert_int_equal(min1, min2);
	assert_int_equal(file_s.st_size, BUF_SIZE);
}
