//
//  RealtimeClock.h
//  Apollo
//
//  Created by Dietmar Planitzer on 2/11/21.
//  Copyright © 2021 Dietmar Planitzer. All rights reserved.
//

#ifndef RealtimeClock_h
#define RealtimeClock_h

#include "Atomic.h"
#include "Foundation.h"
#include "Lock.h"
#include "SystemDescription.h"


// A specific date in the Gregorian calendar
typedef struct _GregorianDate {
    Int8    second;         // 0 - 59
    Int8    minute;         // 0 - 59
    Int8    hour;           // 0 - 23
    Int8    dayOfWeek;      // 1 - 7 with Sunday == 1
    Int8    day;            // 1 - 31
    Int8    month;          // 1 - 12
    Int16   year;           // absolute Gregorian year
} GregorianDate;


// The realtime clock
typedef struct _RealtimeClock {
    Int     type;
    Lock    lock;
    // XXX not fully implemented yet
} RealtimeClock;


// 00:00:00 Thursday, 1 January 1970 UTC
extern const GregorianDate  GREGORIAN_DATE_EPOCH;

extern Bool GregorianDate_Equals(const GregorianDate* _Nonnull a, const GregorianDate* _Nonnull b);


extern RealtimeClock* _Nullable RealtimeClock_Create(const SystemDescription* _Nonnull pSysDesc);
extern void RealtimeClock_Destroy(RealtimeClock* _Nullable pClock);

extern void RealtimeClock_GetDate(RealtimeClock* _Nonnull pClock, GregorianDate* _Nonnull pDate);
extern void RealtimeClock_SetDate(RealtimeClock* _Nonnull pClock, const GregorianDate* _Nonnull pDate);

extern Int RealtimeClock_ReadNonVolatileData(RealtimeClock* _Nonnull pClock, ErrorCode* _Nonnull pError, Byte* _Nonnull pBuffer, Int nBytes);
extern Int RealtimeClock_WriteNonVolatileData(RealtimeClock* _Nonnull pClock, ErrorCode* _Nonnull pError, const Byte* _Nonnull pBuffer, Int nBytes);

#endif /* RealtimeClock_h */
