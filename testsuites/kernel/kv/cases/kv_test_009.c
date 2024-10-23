/****************************************************************************
 * Included Files
 ****************************************************************************/
#include <nuttx/config.h>
#include <stdlib.h>
#include <inttypes.h>
#include <syslog.h>
#include <stdio.h>
#include <time.h>
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
 * Name: TestNuttxKv09
 ****************************************************************************/
void TestNuttxKv09(FAR void **state)
{
    char key[TEST_KEY_LENGTH] = {0};
    char delete_data[TEST_VALUE_LENGTH] = {0};
    int ret;

    for (int i = 0; i < 10; i++)
    {
        sprintf(key, "test_key_%s_%d", __func__, i);
        sprintf(delete_data, "test_data_%s_new", __func__);
        ret = property_set(key, delete_data);
        assert_int_equal(ret, 0);
        ret = property_delete(key);
        assert_int_equal(ret, 0);
    }
}