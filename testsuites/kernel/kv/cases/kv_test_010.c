/****************************************************************************
 * Included Files
 ****************************************************************************/
#include <nuttx/config.h>
#include <stdlib.h>
#include <inttypes.h>
#include <syslog.h>
#include <stdio.h>
#include <string.h>
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
 * Name: TestNuttxKv10
 ****************************************************************************/
void TestNuttxKv10(FAR void **state)
{
    char key[TEST_KEY_LENGTH] = {0};
    char get_data[TEST_VALUE_LENGTH] = {0};
    char get_value[TEST_VALUE_LENGTH] = {0};
    int ret;

    sprintf(key, "test_key_%s", __func__);
    sprintf(get_data, "test_data_%s_old", __func__);
    ret = property_set(key, get_data);
    assert_int_equal(ret, 0);

    sprintf(get_value, "test_data_%s_new_data", __func__);
    ret = property_get(key, get_value, NULL);
    assert_int_equal(ret, strlen(get_value));
    property_delete(key);
}