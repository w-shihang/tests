/****************************************************************************
 * Included Files
 ****************************************************************************/
#include <nuttx/config.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <syslog.h>
#include <stdint.h>
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <errno.h>
#include <cmocka.h>
#include "fstest.h"

#define TESTFILENAME "stream01Testfile"

/****************************************************************************
 * Name: stream
 * Example description:
     1. open a file with "a+".
     2. Write some strings to the file.
     3. Check if the file pointer is offset.
 * Test item: fopen() fseek() ftell()
 * Expect results: TEST PASSED
 ****************************************************************************/
void TestNuttxFsAppend01(FAR void **state)
{
    FILE *fd;
    int pos;
    char readstring[80];
    size_t ret;
    fd = fopen(TESTFILENAME, "a+");
    if (fd == NULL)
    {
        syslog(LOG_ERR, "Unable to open file %s, errno %d\n", TESTFILENAME, errno);
        assert_true(1 == 0);
    }
    fprintf(fd, "This is a test of the append.\n");
    pos = ftell(fd);
    if (fseek(fd, 0, SEEK_END) < 0)
    {
        syslog(LOG_ERR, "fseek fail, errno %d\n", errno);
        fclose(fd);
        assert_true(1 == 0);
    }
    if (ftell(fd) != pos)
    {
        syslog(LOG_ERR, "Error opening for append ... data not at EOF\n");
        fclose(fd);
        assert_true(1 == 0);
    }
    if (fseek(fd, -30, SEEK_END) < 0)
    {
        syslog(LOG_ERR, "fseek fail, errno %d\n", errno);
        fclose(fd);
        assert_true(1 == 0);
    }
    readstring[30] = '\0';
    ret = fread(readstring, 1, 30, fd);
    readstring[ret] = 0;
    fclose(fd);
    if (strcmp(readstring, "This is a test of the append.\n") != 0)
    {
        syslog(LOG_ERR, "strcmp fail\n");
        assert_true(1 == 0);
    }
}
