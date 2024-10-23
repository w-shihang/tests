/****************************************************************************
 * Included Files
 ****************************************************************************/
#include <nuttx/config.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <stdio.h>
#include <time.h>
#include <syslog.h>
#include <stdint.h>
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
 * Name: TestNuttxKv08
 ****************************************************************************/
void TestNuttxKv08(FAR void **state)
{
    char key[TEST_KEY_LENGTH] = {0};
    char data[TEST_VALUE_LENGTH] = {0};
    char value[TEST_VALUE_LENGTH] = {0};
    int ret;

    for (int i = 0; i < 100; i++)
    {
        sprintf(key, "test_key_%s_%d", __func__, i);
        sprintf(data, "test_data_%s_new", __func__);
        ret = property_set(key, data);
        assert_int_equal(ret, 0);
        property_get(key, value, "");
        assert_int_equal(strcmp(value, data), 0);
        property_delete(key);
    }
}