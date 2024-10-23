/****************************************************************************
 * Included Files
 ****************************************************************************/
#include <nuttx/config.h>
#include <stdlib.h>
#include <inttypes.h>
#include <syslog.h>
#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <time.h>
#include "KvTest.h"
#include "kvdb.h"

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: TestNuttxKv13
 ****************************************************************************/
void TestNuttxKv13(FAR void **state)
{
    char key[TEST_KEY_LENGTH] = {0};
    int64_t getint64_data1 = INT64_MAX;
    int64_t getint64_data2 = INT64_MIN;
    int64_t getint64_value;
    int getint64_ret;

    sprintf(key, "test_key_%s", __func__);
    getint64_ret = property_set_int64(key, getint64_data1);
    assert_int_equal(getint64_ret, 0);

    getint64_value = property_get_int64(key, 0);
    assert_int_equal(getint64_value, getint64_data1);
    property_delete(key);

    sprintf(key, "test_key_%s", __func__);
    getint64_ret = property_set_int64(key, getint64_data2);
    assert_int_equal(getint64_ret, 0);

    getint64_value = property_get_int64(key, 0);
    assert_int_equal(getint64_value, getint64_data2);
    property_delete(key);
}