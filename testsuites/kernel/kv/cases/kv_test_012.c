/****************************************************************************
 * Included Files
 ****************************************************************************/
#include <nuttx/config.h>
#include <stdlib.h>
#include <inttypes.h>
#include <syslog.h>
#include <stdint.h>
#include <stdio.h>
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
 * Name: TestNuttxKv12
 ****************************************************************************/
void TestNuttxKv12(FAR void **state)
{
    char key[TEST_KEY_LENGTH] = {0};
    int32_t getint32_data1 = -2102323233;
    int32_t getint32_data2 = 1054545457;
    int32_t getint32_value;
    int getint32_ret;
    sprintf(key, "test_key_%s", __func__);
    getint32_ret = property_set_int32(key, getint32_data1);
    assert_int_equal(getint32_ret, 0);

    getint32_value = property_get_int32(key, 0);
    assert_int_equal(getint32_value, getint32_data1);
    property_delete(key);

    getint32_ret = property_set_int32(key, getint32_data2);
    assert_int_equal(getint32_ret, 0);

    getint32_value = property_get_int32(key, 0);
    assert_int_equal(getint32_value, getint32_data2);
    property_delete(key);
}