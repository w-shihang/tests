/****************************************************************************
 * Included Files
 ****************************************************************************/
#include <nuttx/config.h>
#include <stdlib.h>
#include <inttypes.h>
#include <time.h>
#include <stdio.h>
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
 * Name: TestNuttxKv04
 ****************************************************************************/
void TestNuttxKv04(FAR void **state)
{
    int ret;
    char key[TEST_KEY_LENGTH] = {0};
    /* test key */
    sprintf(key, "test_key_%s", __func__);
    ret = property_set(key, "asdadsasdasd");
    assert_int_equal(ret, 0);

    ret = property_set(key, NULL);
    assert_int_equal(ret, 0);

    property_delete(key);
}