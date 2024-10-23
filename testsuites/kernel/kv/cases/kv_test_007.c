/****************************************************************************
 * Included Files
 ****************************************************************************/
#include <nuttx/config.h>
#include <stdlib.h>
#include <inttypes.h>
#include <syslog.h>
#include <string.h>
#include <time.h>
#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include "KvTest.h"
#include "kvdb.h"

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: TestNuttxKv07
 ****************************************************************************/
void TestNuttxKv07(FAR void **state)
{
    char key[TEST_KEY_LENGTH] = {0};
    char old_data_string[TEST_VALUE_LENGTH] = {0};
    char new_data_string[TEST_VALUE_LENGTH] = {0};
    char value[TEST_VALUE_LENGTH] = {0};
    int32_t data_32, value_32;
    bool data_bool = false;
    bool value_bool;
    int ret;

    sprintf(key, "test_key_%s", __func__);
    sprintf(old_data_string, "test_data_%s_new", __func__);
    ret = property_set(key, old_data_string);
    assert_int_equal(ret, 0);

    sprintf(new_data_string, "test_data_%s_old", __func__);
    ret = property_set(key, new_data_string);
    assert_int_equal(ret, 0);

    property_get(key, value, "");
    assert_int_equal(strcmp(value, new_data_string), 0);
    property_delete(key);

    sprintf(key, "test_key_%s", __func__);
    data_32 = rand() % INT32_MAX + 1;
    ret = property_set_int32(key, data_32);
    assert_int_equal(ret, 0);

    value_32 = property_get_int32(key, 0);
    assert_int_equal(value_32, data_32);

#ifdef CONFIG_LIBC_LONG_LONG
    int64_t data_64, value_64;
    data_64 = rand() % INT64_MAX + 1;
    ret = property_set_int64(key, data_64);
    assert_int_equal(ret, 0);

    value_64 = property_get_int64(key, 0);
    assert_int_equal(value_64, data_64);
#endif

    ret = property_set_bool(key, data_bool);
    assert_int_equal(ret, 0);

    value_bool = property_get_bool(key, 0);
    assert_int_equal(value_bool, data_bool);

    property_delete(key);
}