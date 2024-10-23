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
 * Name: TestNuttxKv26
 ****************************************************************************/
void TestNuttxKv26(FAR void **state)
{
    int ret;
    char key[TEST_KEY_LENGTH] = {0};
    int8_t getbool_ret;

    for (int i = 1; i <= 300; i++)
    {
        sprintf(key, "persist.kv26_bool_%d", i);
        ret = property_set_bool(key, 1);
        assert_int_equal(ret, 0);
        ret = property_commit();
        if (ret != 0)
        {
            syslog(LOG_ERR, "Failed to commit after inserting data. Key:%s", key);
            assert_int_equal(ret, 0);
        }

        getbool_ret = property_get_bool(key, 0);
        assert_int_equal(getbool_ret, 1);

        ret = property_delete(key);
        assert_int_equal(ret, 0);
        ret = property_commit();
        if (ret != 0)
        {
            syslog(LOG_ERR, "Failed to commit after deleting data. Key:%s", key);
            assert_int_equal(ret, 0);
        }
    }
}
