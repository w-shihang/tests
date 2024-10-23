/****************************************************************************
 * Included Files
 ****************************************************************************/
#include <nuttx/config.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <dirent.h>
#include <syslog.h>
#include <stdint.h>
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include "fstest.h"

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: TestNuttxFsMkdir01
 ****************************************************************************/

void TestNuttxFsMkdir01(FAR void **state)
{
    int status;

    /* do mkdir */
    status = mkdir("testdir1", 0700);
    assert_int_equal(status, 0);

    /* do rmdir */
    assert_int_equal(rmdir("testdir1"), 0);

    /* do mkdir */
    status = mkdir("234123412341234", 0700);
    assert_int_equal(status, 0);

    /* do rmdir */
    assert_int_equal(rmdir("234123412341234"), 0);

    /* do mkdir */
    status = mkdir("asdfasdfASDFASDF", 0700);
    assert_int_equal(status, 0);

    /* do rmdir */
    assert_int_equal(rmdir("asdfasdfASDFASDF"), 0);

    /* do mkdir */
    status = mkdir("ASDFASD@%#%54365465654#@%#%@#", 0700);
    assert_int_equal(status, 0);

    /* do rmdir */
    assert_int_equal(rmdir("ASDFASD@%#%54365465654#@%#%@#"), 0);
}