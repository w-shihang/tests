/****************************************************************************
 * Included Files
 ****************************************************************************/
#include <nuttx/config.h>
#include <stdlib.h>
#include <inttypes.h>
#include <stdio.h>
#include <syslog.h>
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
 * Name: TestNuttxKv18
 ****************************************************************************/
void TestNuttxKv18(FAR void **state)
{
    char key[TEST_KEY_LENGTH] = {0};
    int64_t setint64_data1 = INT64_MIN + 1;
    int64_t setint64_data2 = -6727346527346527834;
    int64_t setint64_data3 = 0;
    int64_t setint64_data4 = 8757357537575373757;
    int64_t setint64_data5 = 9223372036854775807;
    int setint64_ret;

    sprintf(key, "test_key_%s_1", __func__);
    setint64_ret = property_set_int64(key, setint64_data1);
    assert_int_equal(setint64_ret, 0);
    property_delete(key);

    sprintf(key, "test_key_%s_2", __func__);
    setint64_ret = property_set_int64(key, setint64_data2);
    assert_int_equal(setint64_ret, 0);
    property_delete(key);

    sprintf(key, "test_key_%s_3", __func__);
    setint64_ret = property_set_int64(key, setint64_data3);
    assert_int_equal(setint64_ret, 0);
    property_delete(key);

    sprintf(key, "test_key_%s_4", __func__);
    setint64_ret = property_set_int64(key, setint64_data4);
    assert_int_equal(setint64_ret, 0);
    property_delete(key);

    sprintf(key, "test_key_%s_5", __func__);
    setint64_ret = property_set_int64(key, setint64_data5);
    assert_int_equal(setint64_ret, 0);
    property_delete(key);
}