//
//  _errno.h
//  libsystem
//
//  Created by Dietmar Planitzer on 9/6/23.
//  Copyright © 2023 Dietmar Planitzer. All rights reserved.
//

#ifndef __SYS_ERRNO_H
#define __SYS_ERRNO_H 1

#define _EOK            0
#ifndef __posix_errors_are_defined
#define __posix_errors_are_defined 1
#define ENOMEM          1
#define ENOMEDIUM       2
#define EDISKCHANGE     3
#define ETIMEDOUT       4
#define ENODEV          5
#define ERANGE          6
#define EINTR           7
#define EAGAIN          8
#define EWOULDBLOCK     9
#define EPIPE           10
#define EBUSY           11
#define ENOSYS          12
#define EINVAL          13
#define EIO             14
#define EPERM           15
#define EDEADLK         16
#define EDOM            17
#define ENOEXEC         18
#define E2BIG           19
#define ENOENT          20
#define ENOTBLK         21
#define EBADF           22
#define ECHILD          23
#define ESRCH           24
#define ESPIPE          25
#define ENOTDIR         26
#define ENAMETOOLONG    27
#define EACCESS         28
#define EROFS           29
#define ENOSPC          30
#define EEXIST          31
#define EOVERFLOW       32
#define EFBIG           33
#define EISDIR          34
#define ENOTIOCTLCMD    35
#define EILSEQ          36

#define __EFIRST    1
#define __ELAST     36
#endif  /* __posix_errors_are_defined */

typedef int _Errno_t;

#endif /* __SYS_ERRNO_H */
