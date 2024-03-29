//
//  ULock.c
//  kernel
//
//  Created by Dietmar Planitzer on 3/21/24.
//  Copyright © 2024 Dietmar Planitzer. All rights reserved.
//

#include "ULock.h"


errno_t ULock_Create(ULockRef _Nullable * _Nonnull pOutSelf)
{
    decl_try_err();
    ULockRef self;

    try(Object_Create(ULock, &self));
    Lock_InitWithOptions(&self->lock, kLockOption_InterruptibleLock);
    *pOutSelf = self;
    return EOK;

catch:
    *pOutSelf = NULL;
    return err;
}

void ULock_deinit(ULockRef _Nonnull self)
{
    Lock_Deinit(&self->lock);
}

CLASS_METHODS(ULock, Object,
OVERRIDE_METHOD_IMPL(deinit, ULock, Object)
);
