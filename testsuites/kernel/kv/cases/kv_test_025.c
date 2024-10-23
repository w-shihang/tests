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
 * Name: TestNuttxKv25
 ****************************************************************************/
void TestNuttxKv25(FAR void **state)
{
    int ret;
    char key[50] = {0}, data[50] = {0}, value[50] = {0};

    for (int i = 1; i <= 300; i++)
    {
        /* test key */
        sprintf(key, "persist.kv25_%d", i);
        /* test data */
        sprintf(value, "test_value_kv25_%d_", i);

        ret = property_set_buffer(key, value, 50);
        assert_int_equal(ret, 0);
        ret = property_commit();
        if (ret != 0)
        {
            syslog(LOG_ERR, "Failed to commit after inserting data. Key:%s Value:%s", key, value);
            assert_int_equal(ret, 0);
        }

        ret = property_get_buffer(key, data, 50);
        assert_int_equal(ret, 50);
        assert_int_equal(strcmp(value, data), 0);

        ret = property_delete(key);
        assert_int_equal(ret, 0);
        ret = property_commit();
        if (ret != 0)
        {
            syslog(LOG_ERR, "Failed to commit after deleting data. Key:%s Value:%s", key, value);
            assert_int_equal(ret, 0);
        }
    }
}
