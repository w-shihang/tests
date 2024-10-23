/****************************************************************************
 * Included Files
 ****************************************************************************/
#include <nuttx/config.h>
#include <stdlib.h>
#include <inttypes.h>
#include <syslog.h>
#include <time.h>
#include <stdio.h>
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
 * Name: TestNuttxKv05
 ****************************************************************************/
void TestNuttxKv05(FAR void **state)
{
    int ret;
    char key[TEST_KEY_LENGTH] = {0};

    for (int i = 1; i < 10; i++)
    {
        sprintf(key, "test_key_%s_%d", __func__, i);
        /* delete the data that doesn't exist */
        ret = property_delete(key);
        assert_int_not_equal(ret, 0);
    }
}