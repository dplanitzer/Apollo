//
//  _math.h
//  libsystem
//
//  Created by Dietmar Planitzer on 9/6/23.
//  Copyright © 2023 Dietmar Planitzer. All rights reserved.
//

#ifndef __SYS_MATH_H
#define __SYS_MATH_H 1

#include <System/abi/_dmdef.h>

#define __abs(x) (((x) < 0) ? -(x) : (x))
#ifndef __DISKIMAGE__
#define __min(x, y) (((x) < (y) ? (x) : (y)))
#define __max(x, y) (((x) > (y) ? (x) : (y)))
#endif
#define __clamped(v, lw, up) ((v) < (lw) ? (lw) : ((v) > (up) ? (up) : (v)))

#define __Ceil_PowerOf2(x, mask)   (((x) + (mask-1)) & ~(mask-1))
#define __Floor_PowerOf2(x, mask) ((x) & ~(mask-1))

#define __Ceil_Ptr_PowerOf2(x, mask)     (void*)(__Ceil_PowerOf2((__uintptr_t)(x), (__uintptr_t)(mask)))
#define __Floor_Ptr_PowerOf2(x, mask)     (void*)(__Floor_PowerOf2((__uintptr_t)(x), (__uintptr_t)(mask)))

#endif /* __SYS_MATH_H */
