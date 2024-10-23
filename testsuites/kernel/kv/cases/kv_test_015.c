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
 * Name: TestNuttxKv15
 ****************************************************************************/
void TestNuttxKv15(FAR void **state)
{
    char key[TEST_KEY_LENGTH] = {0};
    char set_data[TEST_VALUE_LENGTH] = {0};
    int set_ret;

    sprintf(key, "test_key_%s", __func__);
    sprintf(set_data, "test_data_%s_new", __func__);
    set_ret = property_set(key, set_data);
    assert_int_equal(set_ret, 0);

    set_ret = property_delete(key);
    assert_int_equal(set_ret, 0);
}