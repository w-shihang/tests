/****************************************************************************
 * Included Files
 ****************************************************************************/
#include <nuttx/config.h>
#include <string.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <inttypes.h>
#include <fcntl.h>

#include "SocketTest.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/
#define IFF_PROMISC 0x100

/****************************************************************************
 * Public Functions
 ****************************************************************************/
static char gDefaultNetif[IFNAMSIZ] = "eth0";

static void InitIfreq(struct ifreq *ifr)
{
    *ifr = (struct ifreq){{0}};
    //(void)strncpy_s(ifr->ifr_name, sizeof(ifr->ifr_name) - 1, gDefaultNetif, sizeof(ifr->ifr_name) - 1);
    (void)strlcpy(ifr->ifr_name, gDefaultNetif, sizeof(ifr->ifr_name));
    ifr->ifr_name[sizeof(ifr->ifr_name) - 1] = '\0';
}

static char *IfIndex2Name(int fd, unsigned index, char *name)
{
#ifdef CONFIG_NETDEV_IFINDEX
    return if_indextoname(index, name);
#else
    struct ifreq ifr;
    int ret;

    ifr.ifr_ifindex = index;
    ret = ioctl(fd, SIOCGIFNAME, &ifr);
    if (ret < 0) {
        return NULL;
    }
    //ret = strncpy_s(name, IF_NAMESIZE - 1, ifr.ifr_name, IF_NAMESIZE - 1);
    strncpy(name, ifr.ifr_name, IF_NAMESIZE - 1);
    name[IF_NAMESIZE - 1] = '\0';
    return name;
#endif
}

static unsigned IfName2Index(int fd, const char *name)
{
#ifdef CONFIG_NETDEV_IFINDEX
    return if_nametoindex(name);
#else
    struct ifreq ifr;
    int ret;

    //(void)strncpy_s(ifr.ifr_name, sizeof(ifr.ifr_name) - 1, name, sizeof(ifr.ifr_name) - 1);
    (void)strncpy(ifr.ifr_name, name, sizeof(ifr.ifr_name) - 1);
    ifr.ifr_name[sizeof(ifr.ifr_name) - 1] = '\0';
    ret = ioctl(fd, SIOCGIFINDEX, &ifr);
    return ret < 0 ? 0 : ifr.ifr_ifindex;
#endif
}
/****************************************************************************
 * Name: TestNuttxNetSocketTest10
 ****************************************************************************/

static int IoctlTestInternal(int sfd)
{
    struct ifreq ifr = {{0}};
    char ifName[IFNAMSIZ] = {0}, *p = NULL;
    unsigned int loIndex = 0;
    unsigned int lanIndex = 0;
    char lanName[IFNAMSIZ];
    int maxIndex = 256;
    int ret;
    char *ret_s = NULL;

    for (int i = 0; i < maxIndex; ++i) {
        p = IfIndex2Name(sfd, i, ifName);
        if (p) {
            if (strcmp(p, "lo") == 0) {
                loIndex = i;
            } else {
                lanIndex = i;
                strcpy(lanName, p);
                syslog(LOG_INFO,"name of lan:  %s", lanName);
            }
        }
    }

    syslog(LOG_INFO,"ifindex of lo: %u, ifindex of lan: %u", loIndex, lanIndex);
    assert_int_not_equal(loIndex, 0);
    assert_int_not_equal(lanIndex, 0);

    p = IfIndex2Name(sfd, loIndex, ifName);
    syslog(LOG_INFO,"ifindex %u: %s", loIndex, p);
    assert_non_null(p);
    assert_string_equal(p, "lo");

    p = IfIndex2Name(sfd, lanIndex, ifName);
    syslog(LOG_INFO,"ifindex %u: %s", lanIndex, p);
    assert_non_null(p);
    assert_string_equal(p, lanName);

    //ret = strncpy_s(gDefaultNetif, sizeof(gDefaultNetif) -1, p, sizeof(gDefaultNetif) - 1);
    ret_s = strncpy(gDefaultNetif, p, sizeof(gDefaultNetif) - 1);
    assert_non_null(ret_s);
    gDefaultNetif[sizeof(gDefaultNetif) - 1] = '\0';

    ret = (int)IfName2Index(sfd, p);
    syslog(LOG_INFO,"index of %s: %d", p, ret);
    assert_int_not_equal(ret, 0);
    assert_int_equal(ret, lanIndex);

    ifr.ifr_ifindex = lanIndex;
    ret = ioctl(sfd, SIOCGIFNAME, &ifr);
    assert_int_equal(ret, 0);
    syslog(LOG_INFO,"name of ifindex %u: %s", lanIndex, ifr.ifr_name);
    assert_string_equal(ifr.ifr_name, lanName);

    InitIfreq(&ifr);
    ret = ioctl(sfd, SIOCGIFINDEX, &ifr);
    assert_int_equal(ret, 0);
    syslog(LOG_INFO,"index of ifname %s: %d", ifr.ifr_name, ifr.ifr_ifindex);
    assert_int_equal(ifr.ifr_ifindex, lanIndex);

    InitIfreq(&ifr);
    ret = ioctl(sfd, SIOCGIFHWADDR, &ifr);
    assert_int_equal(ret, 0);
    syslog(LOG_INFO,"hwaddr: %02hhX:%02hhX:%02hhX:XX:XX:XX", ifr.ifr_hwaddr.sa_data[0], ifr.ifr_hwaddr.sa_data[1], ifr.ifr_hwaddr.sa_data[2]);

    InitIfreq(&ifr);
    ret = ioctl(sfd, SIOCGIFFLAGS, &ifr);
    assert_int_equal(ret, 0);
    syslog(LOG_INFO,"FLAGS of ifname %s: %#"PRIx32", IFF_PROMISC: %d", ifr.ifr_name, ifr.ifr_flags, !!(ifr.ifr_flags & IFF_PROMISC));

    if (ifr.ifr_flags & IFF_PROMISC) {
        ifr.ifr_flags &= ~(IFF_PROMISC);
    } else {
        ifr.ifr_flags |= IFF_PROMISC;
    }
    syslog(LOG_INFO,"SIOCSIFFLAGS FLAGS: %#"PRIx32, ifr.ifr_flags);
    ret = ioctl(sfd, SIOCSIFFLAGS, &ifr);
    if (ret == -1) {
        assert_int_equal(errno, EPERM);
    } else {
        assert_int_equal(ret, 0);
    }

    InitIfreq(&ifr);
    ret = ioctl(sfd, SIOCGIFFLAGS, &ifr);
    assert_int_equal(ret, 0);
    syslog(LOG_INFO,"FLAGS of ifname %s: %#"PRIx32", IFF_PROMISC: %d", ifr.ifr_name, ifr.ifr_flags, !!(ifr.ifr_flags & IFF_PROMISC));

    ret = fcntl(sfd, F_GETFL, 0);
    assert_int_equal(ret < 0, 0);

    ret = fcntl(sfd, F_SETFL, ret | O_NONBLOCK);
    assert_int_equal(ret < 0, 0);

    return 0;
}

void TestNuttxNetSocketTest10(FAR void **state)
{
    int sfd;

    sfd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    syslog(LOG_INFO,"socket(PF_INET, SOCK_STREAM, IPPROTO_TCP): %d", sfd);
    assert_int_not_equal(sfd, -1);

    (void)IoctlTestInternal(sfd);

    (void)close(sfd);
}
