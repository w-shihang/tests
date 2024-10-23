/****************************************************************************
 * Included Files
 ****************************************************************************/
#include <nuttx/config.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <syslog.h>
#include <stdint.h>
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <sys/epoll.h>
#include "fstest.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/
#define I_FILE1 "poll_test1"
#define I_FILE2 "poll_test2"
/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: TestNuttxFsPoll01
 ****************************************************************************/
void TestNuttxFsPoll01(FAR void **state)
{
    int Poll01_fd1, Poll01_fd2, Poll01_ret;
    struct pollfd Poll01_fds[5];
    struct fs_testsuites_state_s *test_state;

    test_state = (struct fs_testsuites_state_s *)*state;

    Poll01_fd1 = open(I_FILE1, O_RDONLY | O_CREAT);
    assert_true(Poll01_fd1>=0);
    test_state->fd1=Poll01_fd1;

    Poll01_fds[0].fd = Poll01_fd1;
    Poll01_fds[0].events = POLLOUT;

    Poll01_fd2 = open(I_FILE2, O_RDWR | O_CREAT);
    assert_true(Poll01_fd2>=0);
    test_state->fd2=Poll01_fd2;

    Poll01_fds[1].fd = Poll01_fd2;
    Poll01_fds[1].events = POLLIN;

    Poll01_ret = poll(Poll01_fds, 2, 5);
    assert_int_equal(Poll01_ret,2);

    close(Poll01_fd1);
    close(Poll01_fd2);
}
