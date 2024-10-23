/****************************************************************************
 * Included Files
 ****************************************************************************/
#include <nuttx/config.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>
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
 * Name: TestNuttxKv01
 ****************************************************************************/
void TestNuttxKv01(FAR void **state)
{
    int ret;
    char key[TEST_KEY_LENGTH] = {0}, data[TEST_VALUE_LENGTH] = {0}, value[TEST_VALUE_LENGTH] = {0};
    for (int i = 1; i <= 1000; i++)
    {
        /* test key */
        sprintf(key, "test_key_%s_%d", __func__, i);
        /* test data */
        sprintf(data, "test_data_%s_%d", __func__, i);
        ret = property_set(key, data);
        assert_int_equal(ret, 0);

        ret = property_get(key, value, "");
        property_delete(key);

        assert_int_equal(ret, strlen(data));
        assert_int_equal(strcmp(value, data), 0);
    }
}