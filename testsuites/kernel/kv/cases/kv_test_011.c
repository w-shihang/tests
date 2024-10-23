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
#include <time.h>
#include <cmocka.h>
#include <time.h>
#include "KvTest.h"
#include "kvdb.h"

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: TestNuttxKv11
 ****************************************************************************/
void TestNuttxKv11(FAR void **state)
{
    char key[TEST_KEY_LENGTH] = {0};
    int8_t getbool_ret;
    sprintf(key, "test_key_%s", __func__);
    property_set_bool(key, 1);
    getbool_ret = property_get_bool(key, 0);
    assert_int_equal(getbool_ret, 1);
    property_delete(key);
}