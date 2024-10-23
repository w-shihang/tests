/*
 * Copyright (C) 2020 Xiaomi Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __SYSCALLTEST_H
#define __SYSCALLTEST_H

/****************************************************************************
 * Included Files
 ****************************************************************************/
#include <nuttx/config.h>
#include <setjmp.h>
#include <cmocka.h>

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/
/* The test files generated during the 'syscall-test' are stored in this directory */
#define TEST_KEY_LENGTH PROP_NAME_MAX - 1
#define TEST_VALUE_LENGTH PROP_VALUE_MAX - 1
#define MOUNT_DIR CONFIG_TESTS_TESTSUITES_MOUNT_DIR
#define PTHREAD_STACK_SIZE CONFIG_DEFAULT_TASK_STACKSIZE
#define DEFAULT_STACKSIZE CONFIG_DEFAULT_TASK_STACKSIZE
#define TASK_PRIORITY SCHED_PRIORITY_DEFAULT
#define KV_TEST_PREFIX "KVtest."
/****************************************************************************
 * Public Function Prototypes
 ****************************************************************************/
char *CM_random_string(int length);
void Cm_Kv_get_a_random_key(char *buf, int size);

int TestNuttxKvsetup(void **state);
int TestNuttxKvteardown(void **state);

/* test case function */
/* cases/kv_test_001.c *****************************************************/
void TestNuttxKv01(FAR void **state);
/* cases/kv_test_002.c *****************************************************/
void TestNuttxKv02(FAR void **state);
/* cases/kv_test_003.c *****************************************************/
void TestNuttxKv03(FAR void **state);
/* cases/kv_test_004.c *****************************************************/
void TestNuttxKv04(FAR void **state);
/* cases/kv_test_005.c *****************************************************/
void TestNuttxKv05(FAR void **state);
/* cases/kv_test_006.c *****************************************************/
void TestNuttxKv06(FAR void **state);
/* cases/kv_test_007.c *****************************************************/
void TestNuttxKv07(FAR void **state);
/* cases/kv_test_008.c *****************************************************/
void TestNuttxKv08(FAR void **state);
/* cases/kv_test_009.c *****************************************************/
void TestNuttxKv09(FAR void **state);
/* cases/kv_test_010.c *****************************************************/
void TestNuttxKv10(FAR void **state);
/* cases/kv_test_011.c *****************************************************/
void TestNuttxKv11(FAR void **state);
/* cases/kv_test_012.c *****************************************************/
void TestNuttxKv12(FAR void **state);
/* cases/kv_test_013.c *****************************************************/
void TestNuttxKv13(FAR void **state);
/* cases/kv_test_014.c *****************************************************/
void TestNuttxKv14(FAR void **state);
/* cases/kv_test_015.c *****************************************************/
void TestNuttxKv15(FAR void **state);
/* cases/kv_test_016.c *****************************************************/
void TestNuttxKv16(FAR void **state);
/* cases/kv_test_017.c *****************************************************/
void TestNuttxKv17(FAR void **state);
/* cases/kv_test_018.c *****************************************************/
void TestNuttxKv18(FAR void **state);
/* cases/kv_test_019.c *****************************************************/
void TestNuttxKv19(FAR void **state);
/* cases/kv_test_020.c *****************************************************/
void TestNuttxKv20(FAR void **state);
/* cases/kv_test_021.c *****************************************************/
void TestNuttxKv21(FAR void **state);
/* cases/kv_test_022.c *****************************************************/
void TestNuttxKv22(FAR void **state);
/* cases/kv_test_023.c *****************************************************/
void TestNuttxKv23(FAR void **state);
/* cases/kv_test_024.c *****************************************************/
void TestNuttxKv24(FAR void **state);
/* cases/kv_test_025.c *****************************************************/
void TestNuttxKv25(FAR void **state);
/* cases/kv_test_026.c *****************************************************/
void TestNuttxKv26(FAR void **state);
/* cases/kv_test_027.c *****************************************************/
void TestNuttxKv27(FAR void **state);
/* cases/kv_test_028.c *****************************************************/
void TestNuttxKv28(FAR void **state);
/* cases/kv_test_029.c *****************************************************/
void TestNuttxKv29(FAR void **state);
/* cases/kv_test_030.c *****************************************************/
void TestNuttxKv30(FAR void **state);
#endif