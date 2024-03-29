//
//  Assert.h
//  kernel
//
//  Created by Dietmar Planitzer on 8/17/23.
//  Copyright © 2023 Dietmar Planitzer. All rights reserved.
//

#ifndef Assert_h
#define Assert_h

#include <klib/Types.h>
#include <hal/Platform.h>


extern _Noreturn fatal(const char* _Nonnull format, ...);
extern _Noreturn fatalError(const char* _Nonnull filename, int line, int err);
extern _Noreturn fatalAbort(const char* _Nonnull filename, int line);
extern _Noreturn fatalAssert(const char* _Nonnull filename, int line);
extern _Noreturn _fatalException(const ExceptionStackFrame* _Nonnull pFrame);


#define abort() fatalAbort(__func__, __LINE__)


#if DEBUG
#define assert(cond)   if ((cond) == 0) { fatalAssert(__func__, __LINE__); }
#else
#define assert(cond)
#endif

#endif /* Assert_h */
