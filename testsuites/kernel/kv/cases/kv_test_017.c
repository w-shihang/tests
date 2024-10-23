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
 * Name: TestNuttxKv17
 ****************************************************************************/
void TestNuttxKv17(FAR void **state)
{
    char key[TEST_KEY_LENGTH] = {0};
    int32_t setint32_data1 = -2147483648;
    int32_t setint32_data2 = -2135555587;
    int32_t setint32_data3 = 0;
    int32_t setint32_data4 = 1135555587;
    int32_t setint32_data5 = 2147483647;
    int setint32_ret;

    sprintf(key, "test_key_%s_1", __func__);
    setint32_ret = property_set_int32(key, setint32_data1);
    assert_int_equal(setint32_ret, 0);
    property_delete(key);

    sprintf(key, "test_key_%s_2", __func__);
    setint32_ret = property_set_int32(key, setint32_data2);
    assert_int_equal(setint32_ret, 0);
    property_delete(key);

    sprintf(key, "test_key_%s_3", __func__);
    setint32_ret = property_set_int32(key, setint32_data3);
    assert_int_equal(setint32_ret, 0);
    property_delete(key);

    sprintf(key, "test_key_%s_4", __func__);
    setint32_ret = property_set_int32(key, setint32_data4);
    assert_int_equal(setint32_ret, 0);
    property_delete(key);

    sprintf(key, "test_key_%s__5", __func__);
    setint32_ret = property_set_int32(key, setint32_data5);
    assert_int_equal(setint32_ret, 0);
    property_delete(key);
}