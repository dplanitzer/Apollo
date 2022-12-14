//
//  Console.h
//  Apollo
//
//  Created by Dietmar Planitzer on 2/9/21.
//  Copyright © 2021 Dietmar Planitzer. All rights reserved.
//

#ifndef Console_h
#define Console_h

#include "Foundation.h"
#include "Geometry.h"
#include "GraphicsDriver.h"
#include "Lock.h"


typedef enum _LineBreakMode {
    kLineBreakMode_Clip = 0,
    kLineBreakMode_WrapCharacter
} LineBreakMode;


// If set then the family of console print() functions automatically scrolls the
// console up if otherwise the function would end up printing below the bottom
// edge of the console screen.
#define CONSOLE_FLAG_AUTOSCROLL_TO_BOTTOM   0x01


typedef struct _Console {
    GraphicsDriverRef _Nonnull  pGDevice;
    Int8                        x;
    Int8                        y;
    Int8                        cols;       // 80
    Int8                        rows;       // 25
    UInt8                       flags;
    Int8                        lineBreakMode;
    Int8                        tabWidth;   // 8
    Lock                        lock;       // XXX currently protecting DrawStringWithFormat() only!
} Console;


extern Console* _Nonnull Console_GetMain(void);

extern Console* _Nullable Console_Create(GraphicsDriverRef _Nonnull pGDevice);
extern void Console_Destroy(Console* _Nullable pConsole);

extern Rect Console_GetBounds(Console* _Nonnull pConsole);

extern void Console_ClearScreen(Console* _Nonnull pConsole);
extern void Console_ClearLine(Console* _Nonnull pConsole, Int y);

extern void Console_CopyRect(Console* _Nonnull pConsole, Rect srcRect, Point dstLoc);
extern void Console_FillRect(Console* _Nonnull pConsole, Rect rect, Character ch);
extern void Console_ScrollBy(Console* _Nonnull pConsole, Rect clipRect, Point dXY);

extern void Console_MoveCursor(Console* _Nonnull pConsole, Int dx, Int dy);
extern void Console_MoveCursorTo(Console* _Nonnull pConsole, Int x, Int y);

extern void Console_DrawCharacter(Console* _Nonnull pConsole, Character ch);
extern void Console_DrawString(Console* _Nonnull pConsole, const Character* _Nonnull str);
extern void Console_DrawStringWithFormat(Console* _Nonnull pConsole, const Character* _Nonnull format, ...);
extern void Console_vDrawStringWithFormat(Console* _Nonnull pConsole, const Character* _Nonnull format, va_list ap);

#endif /* Console_h */
