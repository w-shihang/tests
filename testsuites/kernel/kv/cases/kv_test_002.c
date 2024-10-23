/****************************************************************************
 * Included Files
 ****************************************************************************/
#include <nuttx/config.h>
#include <stdlib.h>
#include <inttypes.h>
#include <syslog.h>
#include <stdio.h>
#include <time.h>
#include <stdint.h>
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <string.h>
#include "KvTest.h"
#include "kvdb.h"

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: TestNuttxKv02
 ****************************************************************************/
void TestNuttxKv02(FAR void **state)
{
    int ret;
    int invalid_value_len = PROP_VALUE_MAX + 200;
    char key[TEST_KEY_LENGTH] = {0};
    char *value = NULL;

    value = (char *)malloc(invalid_value_len * sizeof(char));
    assert_non_null(value);

    /* test key */
    sprintf(key, "test_key_%s", __func__);
    memset(value, 0, invalid_value_len);
    memset(value, 0x67, invalid_value_len - 2);

    ret = property_set(key, value);

    free(value);
    assert_int_not_equal(ret, 0);
    property_delete(key);
}