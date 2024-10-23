#include <nuttx/config.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <syslog.h>

#define NAMELEN 50
static int len = 8192;
static int test_flag = 1;
static void init_payload(int *data, int seed)
{
    int c = seed + 20000;
    for (int i = 0; i < len; i++)
    {
        data[i] = (c++);
    }
}

static int check_payload(const int *data, int seed)
{
    int c = seed + 20000;
    for (int i = 0; i < len; i++)
    {
        if (data[i] != (c++))
        {
            syslog(LOG_ERR, "check data error, actual %d, except %d\n", data[i], c - 1);
            return -1;
        }
    }
    return 0;
}

static void *write_pthread(void *data)
{
    char *path = (char *)data;
    int testcount = 10000;
    int *payload = malloc(len * sizeof(int));
    if (payload == NULL)
    {
        test_flag = 0;
        syslog(LOG_ERR, "malloc fail\n");
        return NULL;
    }
    while (testcount-- > 0 && test_flag != 0)
    {
        if(testcount % 20 == 0)
        {
            syslog(LOG_INFO, "%s count is %d\n", path, testcount);
        }
        FILE *file = fopen(path, "w+");
        if (file == NULL)
        {
            syslog(LOG_ERR, "open %s for write fail, errno=%d\n", path, errno);
            test_flag = 0;
            break;
        }
        init_payload(payload, testcount);
        int wt = 0;
        int ret;
        while (wt < len)
        {
            ret = fwrite(payload + wt, sizeof(int), len - wt, file);
            wt += ret;
            if (ret <= 0)
            {
                break;
            }
        }
        if (wt != len)
        {
            test_flag = 0;
            fclose(file);
            syslog(LOG_ERR, "write error, errno=%d\n", errno);
            break;
        }
        fclose(file);
        memset(payload, 0, len * sizeof(int));
        file = fopen(path, "r");
        if (file == NULL)
        {
            test_flag = 0;
            syslog(LOG_ERR, "open %s for read fail, errno=%d\n", path, errno);
            break;
        }
        wt = 0;
        while (wt < len)
        {
            ret = fread(payload + wt, sizeof(int), len - wt, file);
            wt += ret;
            if (ret <= 0)
            {
                break;
            }
        }
        if (wt != len)
        {
            test_flag = 0;
            fclose(file);
            syslog(LOG_ERR, "read error, errno=%d\n", errno);
            break;
        }
        fclose(file);
        if(check_payload(payload, testcount) != 0)
        {
            test_flag = 0;
        }
    }
    unlink(path);
    free(payload);
    return NULL;
}

static void show_usage(void)
{
    syslog(LOG_WARNING, "Usage: CMD [-n <nPthread>] [-l <data len>] [-d <test dir>]\n"
                        "\t\t-n: set nPthread, default 5\n"
                        "\t\t-l: set file data len, default 8192\n"
                        "\t\t-d: set test dir\n");
    exit(1);
}

int main(int argc, FAR char *argv[])
{
    if (argc == 1)
    {
        show_usage();
    }
    int nPthread = 5;
    len = 8192;
    test_flag = 1;
    int o;
    char *path = NULL;
    while ((o = getopt(argc, argv, "n:l:d:")) != EOF)
    {
        switch (o)
        {
        case 'n':
            nPthread = atoi(optarg);
            break;
        case 'l':
            len = atoi(optarg);
            break;
        case 'd':
            path = optarg;
            break;
        default:
            show_usage();
            break;
        }
    }
    if (strlen(path) == 0)
    {
        syslog(LOG_ERR, "please set test dir\n");
        return -1;
    }
    char filename[nPthread][NAMELEN];
    memset(filename, 0, sizeof(filename));
    pthread_t tid[nPthread];
    for (int i = 0; i < nPthread; i++)
    {
        if ((snprintf(filename[i], NAMELEN, "%s/test%d", path, i)) < 0)
        {
            syslog(LOG_ERR, "snprintf fail\n");
            continue;
        }
        syslog(LOG_INFO, "test filename %s\n", filename[i]);
        if (pthread_create(&tid[i], NULL, write_pthread, &filename[i]))
        {
            syslog(LOG_ERR, "pthread_create fail\n");
            continue;
        }
    }
    for (int i = 0; i < nPthread; i++)
    {
        pthread_join(tid[i], NULL);
    }
    if (test_flag == 1)
    {
        syslog(LOG_INFO, "TEST PASS\n");
    }
    else
    {
        syslog(LOG_INFO, "TEST FAIL\n");
    }
    return 0;
}