/****************************************************************************
 * Included Files
 ****************************************************************************/
#include <nuttx/config.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/vfs.h>
#include <dirent.h>
#include <syslog.h>
#include <sys/stat.h>
#include <stdint.h>
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include "fstest.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/
#define _1k 1024

/****************************************************************************
 * Private Functions
 ****************************************************************************/

/****************************************************************************
 * Name: TestNuttxFsPrintStatfs
 ****************************************************************************/
__attribute__((unused)) static void TestNuttxFsPrintStatfs(struct statfs *buf)
{
    syslog(LOG_INFO, "statfs buffer:\n");
    syslog(LOG_INFO, "  f_type:     %u \n", (unsigned)buf->f_type);
    syslog(LOG_INFO, "  f_bsize:    %zuk\n", (size_t)buf->f_bsize / _1k);
    syslog(LOG_INFO, "  f_blocks:   %llu \n", (unsigned long long)buf->f_blocks);
    syslog(LOG_INFO, "  f_bfree:    %llu \n", (unsigned long long)buf->f_bfree);
    syslog(LOG_INFO, "  f_bavail:   %llu \n", (unsigned long long)buf->f_bavail);
    syslog(LOG_INFO, "  f_files:    %llu \n", (unsigned long long)buf->f_files);
    syslog(LOG_INFO, "  f_ffree:    %llu \n", (unsigned long long)buf->f_ffree);
    syslog(LOG_INFO, "  f_namelen:  %zu \n", (size_t)buf->f_namelen);
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: TestNuttxFsStatfs01
 ****************************************************************************/
void TestNuttxFsStatfs01(FAR void **state)
{
    struct statfs diskInfo;
    /* call statfs() */
    char* buf = getcwd(NULL, 0);
    int ret = statfs(buf, &diskInfo);
    free(buf);
    assert_int_equal(ret, 0);
}