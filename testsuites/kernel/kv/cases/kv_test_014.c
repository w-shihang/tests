/****************************************************************************
 * Included Files
 ****************************************************************************/
#include <nuttx/config.h>
#include <stdlib.h>
#include <inttypes.h>
#include <syslog.h>
#include <stdint.h>
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <string.h>
#include <cmocka.h>
#include <time.h>
#include "KvTest.h"
#include "kvdb.h"

struct kv_struct
{
    char key[TEST_KEY_LENGTH];
    char value[TEST_VALUE_LENGTH];
    struct kv_struct *next;
};

static struct kv_struct *head;
static struct kv_struct *tail;
static void callback(const char* key, const char* value, void* cookie);
static void freedata(void);

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: TestNuttxKv14
 ****************************************************************************/
void TestNuttxKv14(FAR void **state)
{
    int ret = 0;
    char data[TEST_VALUE_LENGTH] = {0};

    head = (struct kv_struct *)malloc(sizeof(struct kv_struct));
    tail = head;
    property_list(callback, NULL);

    for(struct kv_struct *item = head->next; item != NULL; item = item->next){
        syslog(LOG_INFO, "%s: %s\n", item->key, item->value);

        ret = property_get(item->key, data, "");
        assert_int_equal(ret, strlen(item->value));
        assert_int_equal(strcmp(item->value, data), 0);
    }

    freedata();
}

static void callback(const char* key, const char* value, void* cookie){

    struct kv_struct *data = (struct kv_struct *)malloc(sizeof(struct kv_struct));
    strcpy(data->key, key);
    strcpy(data->value, value);
    data->next = NULL;
    
    tail->next = data;
    tail = data;
}

static void freedata(void){

    struct kv_struct *tmp;
    
    while (head){
        tmp = head;
        head = head->next;
        free(tmp);
    };
}