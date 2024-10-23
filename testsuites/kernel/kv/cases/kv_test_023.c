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
 * Name: TestNuttxKv23
 ****************************************************************************/
void TestNuttxKv23(FAR void **state){

    int ret;
    char key[50] = {0}, data[50] = {0}, value[50] = {0};
    for (int i = 1; i <= 1000; i++)
    {
        /* test key */
        sprintf(key, "test_key_%d", i);
        /* test data */
        sprintf(data, "test_data_%d", i);

        ret = property_set_buffer(key, data, 50);
        assert_int_equal(ret, 0);

        ret = property_get_buffer(key, value, 50);
        assert_int_equal(ret, 50);
        assert_int_equal(strcmp(value, data), 0);
        
        ret = property_delete(key);
        assert_int_equal(ret, 0);
    }

}