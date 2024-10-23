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
 * Name: TestNuttxKv24
 ****************************************************************************/

void TestNuttxKv24(FAR void **state){
    int ret;
    char key[50] = {0}, data[50] = {0}, value[50] = {0};

    /* test key */
    strcpy(key, "persist.kv24_buffer");
    /* test data */
    strcpy(value, "test_value_kv24");

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

    strcat(value, "2");
    ret = property_set_buffer(key, value, 50);
    assert_int_equal(ret, 0);
    ret = property_commit();
    if (ret != 0)
    {
        syslog(LOG_ERR, "Failed to commit after updating data. Key:%s Value:%s", key, value);
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

    ret = property_get_buffer(key, data, 50);
    assert_int_not_equal(ret, 0);

}
