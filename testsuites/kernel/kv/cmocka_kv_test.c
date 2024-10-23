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

/****************************************************************************
 * Included Files
 ****************************************************************************/
#include <nuttx/config.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdint.h>
#include <stdio.h>
#include "KvTest.h"
#include <cmocka.h>

/****************************************************************************
 * Name: cmocka_sched_test_main
 ****************************************************************************/
int main(int argc, char *argv[])
{
  /* Add Test Cases */
  const struct CMUnitTest NuttxKvdbTestSuites[] = {
#ifdef CONFIG_KVDB_TEMPORARY_STORAGE
      cmocka_unit_test_setup_teardown(TestNuttxKv01, TestNuttxKvsetup, TestNuttxKvteardown),
      cmocka_unit_test_setup_teardown(TestNuttxKv02, TestNuttxKvsetup, TestNuttxKvteardown),
      cmocka_unit_test_setup_teardown(TestNuttxKv03, TestNuttxKvsetup, TestNuttxKvteardown),
      cmocka_unit_test_setup_teardown(TestNuttxKv04, TestNuttxKvsetup, TestNuttxKvteardown),
      cmocka_unit_test_setup_teardown(TestNuttxKv05, TestNuttxKvsetup, TestNuttxKvteardown),
      cmocka_unit_test_setup_teardown(TestNuttxKv06, TestNuttxKvsetup, TestNuttxKvteardown),
      cmocka_unit_test_setup_teardown(TestNuttxKv07, TestNuttxKvsetup, TestNuttxKvteardown),
      cmocka_unit_test_setup_teardown(TestNuttxKv08, TestNuttxKvsetup, TestNuttxKvteardown),
      cmocka_unit_test_setup_teardown(TestNuttxKv09, TestNuttxKvsetup, TestNuttxKvteardown),
      cmocka_unit_test_setup_teardown(TestNuttxKv10, TestNuttxKvsetup, TestNuttxKvteardown),
      cmocka_unit_test_setup_teardown(TestNuttxKv11, TestNuttxKvsetup, TestNuttxKvteardown),
      cmocka_unit_test_setup_teardown(TestNuttxKv12, TestNuttxKvsetup, TestNuttxKvteardown),
      cmocka_unit_test_setup_teardown(TestNuttxKv13, TestNuttxKvsetup, TestNuttxKvteardown),
#endif
      cmocka_unit_test_setup_teardown(TestNuttxKv14, TestNuttxKvsetup, TestNuttxKvteardown),
#ifdef CONFIG_KVDB_TEMPORARY_STORAGE
      cmocka_unit_test_setup_teardown(TestNuttxKv15, TestNuttxKvsetup, TestNuttxKvteardown),
      cmocka_unit_test_setup_teardown(TestNuttxKv16, TestNuttxKvsetup, TestNuttxKvteardown),
      cmocka_unit_test_setup_teardown(TestNuttxKv17, TestNuttxKvsetup, TestNuttxKvteardown),
      cmocka_unit_test_setup_teardown(TestNuttxKv18, TestNuttxKvsetup, TestNuttxKvteardown),
      cmocka_unit_test_setup_teardown(TestNuttxKv19, TestNuttxKvsetup, TestNuttxKvteardown),
      cmocka_unit_test_setup_teardown(TestNuttxKv20, TestNuttxKvsetup, TestNuttxKvteardown),
#endif
#ifdef CONFIG_NSH_ROMFSETC
      cmocka_unit_test_setup_teardown(TestNuttxKv21, TestNuttxKvsetup, TestNuttxKvteardown),
#endif
#ifdef CONFIG_KVDB_TEMPORARY_STORAGE
      cmocka_unit_test_setup_teardown(TestNuttxKv22, TestNuttxKvsetup, TestNuttxKvteardown),
      cmocka_unit_test_setup_teardown(TestNuttxKv23, TestNuttxKvsetup, TestNuttxKvteardown),
#endif
      cmocka_unit_test_setup_teardown(TestNuttxKv24, TestNuttxKvsetup, TestNuttxKvteardown),
      cmocka_unit_test_setup_teardown(TestNuttxKv25, TestNuttxKvsetup, TestNuttxKvteardown),
      cmocka_unit_test_setup_teardown(TestNuttxKv26, TestNuttxKvsetup, TestNuttxKvteardown),
      cmocka_unit_test_setup_teardown(TestNuttxKv27, TestNuttxKvsetup, TestNuttxKvteardown),
      cmocka_unit_test_setup_teardown(TestNuttxKv28, TestNuttxKvsetup, TestNuttxKvteardown),
      cmocka_unit_test_setup_teardown(TestNuttxKv29, TestNuttxKvsetup, TestNuttxKvteardown),
      cmocka_unit_test_setup_teardown(TestNuttxKv30, TestNuttxKvsetup, TestNuttxKvteardown),
  };

  /* Run Test cases */
  cmocka_run_group_tests(NuttxKvdbTestSuites, NULL, NULL);
  return 0;
}