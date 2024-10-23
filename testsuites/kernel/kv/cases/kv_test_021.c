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
 * Name: TestNuttxKv21
 ****************************************************************************/

void TestNuttxKv21(FAR void **state){

    int ret = 0;
    char key[TEST_KEY_LENGTH] = {0};
    char value[TEST_VALUE_LENGTH] = {0};
    char data[TEST_VALUE_LENGTH] = {0};

    strcpy(key, "persist.test.kv.reload");
    strcpy(value, "test");

    // get key
    ret = property_get(key, data, "");
    assert_int_equal(ret, strlen(value));
    assert_int_equal(strcmp(value, data), 0);
    
    // delete persist key
    ret = property_delete(key);
    assert_int_equal(ret, 0);

    property_reload();

    // get key
    ret = property_get(key, data, "");
    assert_int_equal(ret, strlen(value));
    assert_int_equal(strcmp(value, data), 0);
}
