/****************************************************************************
 * Included Files
 ****************************************************************************/
#include <nuttx/config.h>
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <syslog.h>
#include <stdint.h>
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdio.h>
#include <cmocka.h>
#include <time.h>
#include <string.h>
#include "KvTest.h"
#include "kvdb.h"

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: TestNuttxKv20
 ****************************************************************************/
void TestNuttxKv20(FAR void **state)
{
    char data[TEST_VALUE_LENGTH] = {0};
    struct timespec t_start;
    struct timespec t_end;
    long timedif;
    int ret;
    char key[32] = {0};
    int test_round = 3000;

    sprintf(data, "test_data_%s", __func__);
    clock_gettime(CLOCK_MONOTONIC, &t_start);
    for (int i = 0; i < test_round; i++)
    {
        sprintf(key, "%s%s-%d", KV_TEST_PREFIX, __func__, i);
        ret = property_set(key, data);
        assert_int_equal(ret, 0);
    }
    clock_gettime(CLOCK_MONOTONIC, &t_end);
    timedif = 1000000 * (t_end.tv_sec - t_start.tv_sec) + (t_end.tv_nsec - t_start.tv_nsec) / 1000;
    syslog(LOG_INFO, "set data:%d takes:%ld microseconds\n", test_round, timedif);

    for (int k = 0; k < test_round; k++)
    {
        sprintf(key, "%s%s-%d", KV_TEST_PREFIX, __func__, k);
        ret = property_delete(key);
        assert_int_equal(ret, 0);
    }
}