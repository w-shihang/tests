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
#include <stdint.h>
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <stdio.h>
#include "fstest.h"

/****************************************************************************
 * Name: cmocka_fs_test_main
 ****************************************************************************/
int main(int argc, char *argv[])
{
  /* Add Test Cases */
  const struct CMUnitTest NuttxFsTestSuites[] = {
      cmocka_unit_test_setup_teardown(TestNuttxFsCreat01, TestNuttxFsTestGroupSetUp, TestNuttxFsTestGroupTearDown),
      cmocka_unit_test_setup_teardown(TestNuttxFsDup01, TestNuttxFsTestGroupSetUp, TestNuttxFsTestGroupTearDown),
      //cmocka_unit_test_setup_teardown(TestNuttxFsDup201, TestNuttxFsTestGroupSetUp, TestNuttxFsTestGroupTearDown),
      cmocka_unit_test_setup_teardown(TestNuttxFsFcntl01, TestNuttxFsTestGroupSetUp, TestNuttxFsTestGroupTearDown),
#ifndef CONFIG_ARCH_SIM
      cmocka_unit_test_setup_teardown(TestNuttxFsFcntl02, TestNuttxFsTestGroupSetUp, TestNuttxFsTestGroupTearDown),
      cmocka_unit_test_setup_teardown(TestNuttxFsFcntl03, TestNuttxFsTestGroupSetUp, TestNuttxFsTestGroupTearDown),
#endif
      cmocka_unit_test_setup_teardown(TestNuttxFsFstat01, TestNuttxFsTestGroupSetUp, TestNuttxFsTestGroupTearDown),
#ifndef CONFIG_ARCH_SIM
      cmocka_unit_test_setup_teardown(TestNuttxFsFstat02, TestNuttxFsTestGroupSetUp, TestNuttxFsTestGroupTearDown),
#endif
      cmocka_unit_test_setup_teardown(TestNuttxFsFstatfs01, TestNuttxFsTestGroupSetUp, TestNuttxFsTestGroupTearDown),
      cmocka_unit_test_setup_teardown(TestNuttxFsFsync01, TestNuttxFsTestGroupSetUp, TestNuttxFsTestGroupTearDown),
      cmocka_unit_test_setup_teardown(TestNuttxFsFsync02, TestNuttxFsTestGroupSetUp, TestNuttxFsTestGroupTearDown),
      cmocka_unit_test_setup_teardown(TestNuttxFsGetfilep01, TestNuttxFsTestGroupSetUp, TestNuttxFsTestGroupTearDown),
      cmocka_unit_test_setup_teardown(TestNuttxFsMkdir01, TestNuttxFsTestGroupSetUp, TestNuttxFsTestGroupTearDown),
      cmocka_unit_test_setup_teardown(TestNuttxFsOpen01, TestNuttxFsTestGroupSetUp, TestNuttxFsTestGroupTearDown),
      cmocka_unit_test_setup_teardown(TestNuttxFsOpendir01, TestNuttxFsTestGroupSetUp, TestNuttxFsTestGroupTearDown),
      cmocka_unit_test_setup_teardown(TestNuttxFsOpendir02, TestNuttxFsTestGroupSetUp, TestNuttxFsTestGroupTearDown),
      cmocka_unit_test_setup_teardown(TestNuttxFsPread01, TestNuttxFsTestGroupSetUp, TestNuttxFsTestGroupTearDown),
      cmocka_unit_test_setup_teardown(TestNuttxFsPwrite01, TestNuttxFsTestGroupSetUp, TestNuttxFsTestGroupTearDown),
#ifndef CONFIG_ARCH_SIM
      cmocka_unit_test_setup_teardown(TestNuttxFsRead01, TestNuttxFsTestGroupSetUp, TestNuttxFsTestGroupTearDown),
#endif
      cmocka_unit_test_setup_teardown(TestNuttxFsReaddir01, TestNuttxFsTestGroupSetUp, TestNuttxFsTestGroupTearDown),
      cmocka_unit_test_setup_teardown(TestNuttxFsReadlink01, TestNuttxFsTestGroupSetUp, TestNuttxFsTestGroupTearDown),
      cmocka_unit_test_setup_teardown(TestNuttxFsRename01, TestNuttxFsTestGroupSetUp, TestNuttxFsTestGroupTearDown),
      cmocka_unit_test_setup_teardown(TestNuttxFsRename02, TestNuttxFsTestGroupSetUp, TestNuttxFsTestGroupTearDown),
      cmocka_unit_test_setup_teardown(TestNuttxFsRewinddir01, TestNuttxFsTestGroupSetUp, TestNuttxFsTestGroupTearDown),
      cmocka_unit_test_setup_teardown(TestNuttxFsRmdir01, TestNuttxFsTestGroupSetUp, TestNuttxFsTestGroupTearDown),
      cmocka_unit_test_setup_teardown(TestNuttxFsRmdir02, TestNuttxFsTestGroupSetUp, TestNuttxFsTestGroupTearDown),
      cmocka_unit_test_setup_teardown(TestNuttxFsRmdir03, TestNuttxFsTestGroupSetUp, TestNuttxFsTestGroupTearDown),
      cmocka_unit_test_setup_teardown(TestNuttxFsSeek01, TestNuttxFsTestGroupSetUp, TestNuttxFsTestGroupTearDown),
      cmocka_unit_test_setup_teardown(TestNuttxFsSeek02, TestNuttxFsTestGroupSetUp, TestNuttxFsTestGroupTearDown),
#ifndef CONFIG_ARCH_SIM
      cmocka_unit_test_setup_teardown(TestNuttxFsStat01, TestNuttxFsTestGroupSetUp, TestNuttxFsTestGroupTearDown),
#endif
      cmocka_unit_test_setup_teardown(TestNuttxFsStatfs01, TestNuttxFsTestGroupSetUp, TestNuttxFsTestGroupTearDown),
      cmocka_unit_test_setup_teardown(TestNuttxFsSymlink01, TestNuttxFsTestGroupSetUp, TestNuttxFsTestGroupTearDown),
      cmocka_unit_test_setup_teardown(TestNuttxFsTruncate01, TestNuttxFsTestGroupSetUp, TestNuttxFsTestGroupTearDown),
      cmocka_unit_test_setup_teardown(TestNuttxFsUnlink01, TestNuttxFsTestGroupSetUp, TestNuttxFsTestGroupTearDown),
      cmocka_unit_test_setup_teardown(TestNuttxFsWrite01, TestNuttxFsTestGroupSetUp, TestNuttxFsTestGroupTearDown),
      cmocka_unit_test_setup_teardown(TestNuttxFsWrite02, TestNuttxFsTestGroupSetUp, TestNuttxFsTestGroupTearDown),
      cmocka_unit_test_setup_teardown(TestNuttxFsWrite03, TestNuttxFsTestGroupSetUp, TestNuttxFsTestGroupTearDown),
      cmocka_unit_test_setup_teardown(TestNuttxFsAppend01, TestNuttxFsTestGroupSetUp, TestNuttxFsTestGroupTearDown),
      cmocka_unit_test_setup_teardown(TestNuttxFsSendfile01, TestNuttxFsTestGroupSetUp, TestNuttxFsTestGroupTearDown),
      cmocka_unit_test_setup_teardown(TestNuttxFsSendfile02, TestNuttxFsTestGroupSetUp, TestNuttxFsTestGroupTearDown),
      cmocka_unit_test_setup_teardown(TestNuttxFsStream01, TestNuttxFsTestGroupSetUp, TestNuttxFsTestGroupTearDown),
      cmocka_unit_test_setup_teardown(TestNuttxFsStream02, TestNuttxFsTestGroupSetUp, TestNuttxFsTestGroupTearDown),
      cmocka_unit_test_setup_teardown(TestNuttxFsStream03, TestNuttxFsTestGroupSetUp, TestNuttxFsTestGroupTearDown),
      cmocka_unit_test_setup_teardown(TestNuttxFsStream04, TestNuttxFsTestGroupSetUp, TestNuttxFsTestGroupTearDown),
      cmocka_unit_test_setup_teardown(TestNuttxFsEventfd, TestNuttxFsTestGroupSetUp, TestNuttxFsTestGroupTearDown),
      cmocka_unit_test_setup_teardown(TestNuttxFsPoll01, TestNuttxFsTestGroupSetUp, TestNuttxFsTestGroupTearDown),
  };
  /* Run Test cases */
  cmocka_run_group_tests(NuttxFsTestSuites, NULL, NULL);
  return 0;
}
