/****************************************************************************
 * Included Files
 ****************************************************************************/
#include <nuttx/config.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <inttypes.h>
#include <syslog.h>
#include <stdint.h>
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <time.h>
#include <pthread.h>
#include "KvTest.h"
#include "kvdb.h"

/****************************************************************************
 * Private Functions
 ****************************************************************************/

/****************************************************************************
 * Name: store_data
 ****************************************************************************/
pthread_mutex_t mutex_kv;

static void *store_persist_data(void *arg)
{

    char key[TEST_KEY_LENGTH] = {0};
    char value[TEST_VALUE_LENGTH] = {0};
    int ret;
    for (int k = 0; k < 100; k++)
    {
        sprintf(key, "persist.kv30_%d_%d", gettid(), k);
        sprintf(value, "test_value_kv30_%d_%d", gettid(), k);
        ret = property_set(key, value);
        if (ret != 0)
        {
            syslog(LOG_ERR, "Failed to insert data. The test thread number:%d Key:%s value:%s\n", gettid(), key, value);
            pthread_mutex_lock(&mutex_kv);
            (*((int *)arg)) = -1;
            pthread_mutex_unlock(&mutex_kv);
            return NULL;
        }
        ret = property_commit();
        if (ret != 0)
        {
            syslog(LOG_ERR, "Failed to commit after inserting data. The test thread number:%d Key:%s value:%s\n", gettid(), key, value);
            pthread_mutex_lock(&mutex_kv);
            (*((int *)arg)) = -1;
            pthread_mutex_unlock(&mutex_kv);
            return NULL;
        }

        ret = property_delete(key);
        if (ret != 0)
        {
            syslog(LOG_ERR, "Failed to delete data. The test thread number:%d Key:%s value:%s\n", gettid(), key, value);
            pthread_mutex_lock(&mutex_kv);
            (*((int *)arg)) = -1;
            pthread_mutex_unlock(&mutex_kv);
            return NULL;
        }
        ret = property_commit();
        if (ret != 0)
        {
            syslog(LOG_ERR, "Failed to commit after deleting data. The test thread number:%d Key:%s value:%s\n", gettid(), key, value);
            pthread_mutex_lock(&mutex_kv);
            (*((int *)arg)) = -1;
            pthread_mutex_unlock(&mutex_kv);
            return NULL;
        }
    }
    return NULL;
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: TestNuttxKv30
 ****************************************************************************/
void TestNuttxKv30(FAR void **state)
{
    pthread_t nthread[3];
    int status;
    int num_thread = 3;
    int test_flag = 0;

    pthread_mutex_init(&mutex_kv, NULL);

    for (int i = 0; i < num_thread; i++)
    {
        /* creat test thread */
        status = pthread_create(&nthread[i], NULL, store_persist_data, &test_flag);
        assert_int_equal(status, 0);
    }

    for (int j = 0; j < num_thread; j++) {
        pthread_join(nthread[j], NULL);
    }

    pthread_mutex_destroy(&mutex_kv);

    /* Check whether any child threads failed to run */
    assert_int_equal(test_flag, 0);
}