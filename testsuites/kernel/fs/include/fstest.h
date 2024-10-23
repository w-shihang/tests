#ifndef __TEST_H
#define __TEST_H

#include <nuttx/config.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <libgen.h>
#include <dirent.h>
#include <sys/stat.h>
#include <syslog.h>

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/
#define TEST_PASS 0
#define TEST_FAIL -1

#define MAX_PATH 300

/* The test files generated during the 'fs-test' are stored in this directory */
#define FS_TEST_DIR "fs_test_dir"
#define MOUNT_DIR CONFIG_TESTS_TESTSUITES_MOUNT_DIR

#define PATH_SIZE 128

struct fs_testsuites_state_s
{
    char filename[PATH_SIZE];
    char *ptr;
    int fd1;
    int fd2;
};

/****************************************************************************
 * Public Function Prototypes
 ****************************************************************************/
unsigned long long cm_get_partition_available_size(void);
int cm_unlink_recursive(FAR char *path);
int TestNuttxFsTestGroupSetUp(FAR void **state);
int TestNuttxFsTestGroupTearDown(FAR void **state);

/* cases/fs_creat_test.c ************************************************/
void TestNuttxFsCreat01(FAR void **state);

/* cases/fs_dup_test.c ************************************************/
void TestNuttxFsDup01(FAR void **state);

/* cases/fs_dup2_test.c ************************************************/
void TestNuttxFsDup201(FAR void **state);

/* cases/fs_fcntl_test.c ************************************************/
void TestNuttxFsFcntl01(FAR void **state);
void TestNuttxFsFcntl02(FAR void **state);
void TestNuttxFsFcntl03(FAR void **state);

/* cases/fs_fstat_test.c ************************************************/
void TestNuttxFsFstat01(FAR void **state);
void TestNuttxFsFstat02(FAR void **state);

/* cases/fs_fstatfs_test.c ************************************************/
void TestNuttxFsFstatfs01(FAR void **state);

/* cases/fs_fsync_test.c ************************************************/
void TestNuttxFsFsync01(FAR void **state);
void TestNuttxFsFsync02(FAR void **state);

/* cases/fs_getfilep_test.c ************************************************/
void TestNuttxFsGetfilep01(FAR void **state);

/* cases/fs_mkdir_test.c ************************************************/
void TestNuttxFsMkdir01(FAR void **state);

/* cases/fs_open_test.c ************************************************/
void TestNuttxFsOpen01(FAR void **state);

/* cases/fs_opendir_test.c ************************************************/
void TestNuttxFsOpendir01(FAR void **state);
void TestNuttxFsOpendir02(FAR void **state);

/* cases/fs_pread_test.c ************************************************/
void TestNuttxFsPread01(FAR void **state);

/* cases/fs_pwrite_test.c ************************************************/
void TestNuttxFsPwrite01(FAR void **state);

/* cases/fs_read_test.c ************************************************/
void TestNuttxFsRead01(FAR void **state);

/* cases/fs_readdir_test.c ************************************************/
void TestNuttxFsReaddir01(FAR void **state);

/* cases/fs_readlink_test.c ************************************************/
void TestNuttxFsReadlink01(FAR void **state);

/* cases/fs_rename_test.c ************************************************/
void TestNuttxFsRename01(FAR void **state);
void TestNuttxFsRename02(FAR void **state);

/* cases/fs_rewinddir_test.c ************************************************/
void TestNuttxFsRewinddir01(FAR void **state);

/* cases/fs_rmdir_test.c ************************************************/
void TestNuttxFsRmdir01(FAR void **state);
void TestNuttxFsRmdir02(FAR void **state);
void TestNuttxFsRmdir03(FAR void **state);

/* cases/fs_seek_test.c ************************************************/
void TestNuttxFsSeek01(FAR void **state);
void TestNuttxFsSeek02(FAR void **state);

/* cases/fs_stat_test.c ************************************************/
void TestNuttxFsStat01(FAR void **state);

/* cases/fs_statfs_test.c ************************************************/
void TestNuttxFsStatfs01(FAR void **state);

/* cases/fs_symlink_test.c ************************************************/
void TestNuttxFsSymlink01(FAR void **state);

/* cases/fs_truncate_test.c ************************************************/
void TestNuttxFsTruncate01(FAR void **state);

/* cases/fs_unlink_test.c ************************************************/
void TestNuttxFsUnlink01(FAR void **state);

/* cases/fs_write_test.c ************************************************/
void TestNuttxFsWrite01(FAR void **state);
void TestNuttxFsWrite02(FAR void **state);
void TestNuttxFsWrite03(FAR void **state);

/* cases/fs_append_test.c ************************************************/
void TestNuttxFsAppend01(FAR void **state);

/* cases/fs_sendfile_test.c ************************************************/
void TestNuttxFsSendfile01(FAR void **state);
void TestNuttxFsSendfile02(FAR void **state);

/* cases/fs_stream_test.c ************************************************/
void TestNuttxFsStream01(FAR void **state);
void TestNuttxFsStream02(FAR void **state);
void TestNuttxFsStream03(FAR void **state);
void TestNuttxFsStream04(FAR void **state);

/* cases/fs_eventfd_test.c ************************************************/
void TestNuttxFsEventfd(FAR void **state);

/* fs_poll_test.c ************************************************/
void TestNuttxFsPoll01(FAR void **state);

#endif
