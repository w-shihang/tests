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
 * Name: TestNuttxKv16
 ****************************************************************************/
void TestNuttxKv16(FAR void **state)
{
    char key[TEST_KEY_LENGTH] = {0};
    int setbool_ret;

    sprintf(key, "test_key_%s", __func__);
    setbool_ret = property_set_bool(key, 1);
    assert_int_equal(setbool_ret, 0);
    property_delete(key);

    sprintf(key, "test_key_%s", __func__);
    setbool_ret = property_set_bool(key, 0);
    assert_int_equal(setbool_ret, 0);
    property_delete(key);
}