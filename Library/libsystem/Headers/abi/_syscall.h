//
//  _syscall.h
//  libsystem
//
//  Created by Dietmar Planitzer on 9/2/23.
//  Copyright © 2023 Dietmar Planitzer. All rights reserved.
//

#ifndef __ABI_SYSCALLS_H
#define __ABI_SYSCALLS_H 1

#define SC_read                 0   // ssize_t read(int fd, const char * _Nonnull buffer, size_t count)
#define SC_write                1   // ssize_t write(int fd, const char * _Nonnull buffer, size_t count)
#define SC_sleep                2   // errno_t sleep(const struct timespec * _Nonnull delay)
#define SC_dispatch_async       3   // errno_t dispatch_async(void * _Nonnull pUserClosure)
#define SC_alloc_address_space  4   // errno_t alloc_address_space(int nbytes, void **pOutMem)
#define SC_exit                 5   // _Noreturn exit(int status)
#define SC_spawn_process        6   // errno_t spawnp(struct __spawn_arguments_t * _Nonnull args, pid_t * _Nullable rpid)
#define SC_getpid               7   // pid_t getpid(void)
#define SC_getppid              8   // pid_t getppid(void)
#define SC_getpargs             9   // struct __process_arguments_t * _Nonnull getpargs(void)
#define SC_open                 10  // errno_t open(const char * _Nonnull name, int options, int* _Nonnull fd)
#define SC_close                11  // errno_t close(int fd)
#define SC_waitpid              12  // errno_t waitpid(pid_t pid, struct __waitpid_result_t * _Nullable result)
#define SC_seek                 13  // errno_t seek(int fd, off_t offset, off_t * _Nullable newpos, int whence)
#define SC_getcwd               14  // errno_t getcwd(char* buffer, size_t bufferSize)
#define SC_setcwd               15  // errno_t setcwd(const char* _Nonnull path)
#define SC_getuid               16  // uid_t getuid(void)
#define SC_getumask             17  // mode_t getumask(void)
#define SC_setumask             18  // void setumask(mode_t mask)
#define SC_mkdir                19  // errno_t mkdir(const char* _Nonnull path, mode_t mode)
#define SC_getfileinfo          20  // errno_t getfileinfo(const char* _Nonnull path, struct _file_info_t* _Nonnull info)
#define SC_opendir              21  // errno_t opendir(const char* _Nonnull path, int* _Nonnull fd)
#define SC_setfileinfo          22  // errno_t setfileinfo(const char* _Nonnull path, struct _mutable_file_info_t* _Nonnull info)
#define SC_access               23  // errno_t access(const char* _Nonnull path, int mode)
#define SC_fgetfileinfo         24  // fgetfileinfo(int fd, struct _file_info_t* _Nonnull info)
#define SC_fsetfileinfo         25  // fsetfileinfo(int fd, struct _mutable_file_info_t* _Nonnull info)
#define SC_unlink               26  // errno_t unlink(const char* path)
#define SC_rename               27  // errno_t rename(const char* _Nonnull oldpath, const char* _Nonnull newpath)
#define SC_ioctl                28  // errno_t ioctl(int fd, int cmd, ...)
#define SC_truncate             29  // errno_t truncate(const char* _Nonnull path, off_t length)
#define SC_ftruncate            30  // errno_t ftruncate(int fd, off_t length)
#define SC_mkfile               31  // errno_t creat(const char* _Nonnull path, int options, int permissions, int* _Nonnull fd)

#endif /* __ABI_SYSCALLS_H */