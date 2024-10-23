/****************************************************************************
 * Included Files
 ****************************************************************************/
#include <nuttx/config.h>
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <syslog.h>
#include <stdint.h>
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdio.h>
#include <cmocka.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include "KvTest.h"
#include "kvdb.h"

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: TestNuttxKv29
 ****************************************************************************/
void TestNuttxKv29(FAR void **state){

    int ret;
    char key[50] = {0}, value[50] = {0}, data[50] = {0};
    for (int i = 1; i <= 300; i++)
    {
        /* test key */
        sprintf(key, "persist.kv29_%d", i);
        /* test data */
        sprintf(value, "test_value_kv29_%d", i);

        ret = property_set(key, value);
        assert_int_equal(ret, 0);

        ret = property_commit();
        if (ret != 0)
        {
            syslog(LOG_ERR, "Failed to commit after inserting data. Key:%s value:%s\n", key, value);
            assert_int_equal(ret, 0);
        }

        ret = property_get(key, data, "");
        assert_int_equal(ret, strlen(value));

        ret = property_delete(key);
        assert_int_equal(ret, 0);

        ret = property_commit();
        if (ret != 0)
        {
            syslog(LOG_ERR, "Failed to commit after deleting data. Key:%s value:%s\n", key, value);
            assert_int_equal(ret, 0);
        }
    }

}