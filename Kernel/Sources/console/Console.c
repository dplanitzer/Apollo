//
//  Console.c
//  kernel
//
//  Created by Dietmar Planitzer on 2/9/21.
//  Copyright © 2021 Dietmar Planitzer. All rights reserved.
//

#include "ConsolePriv.h"
#include <User.h>
#include <System/IOChannel.h>


////////////////////////////////////////////////////////////////////////////////
// MARK: -
// MARK: ConsoleChannel
////////////////////////////////////////////////////////////////////////////////

errno_t ConsoleChannel_ioctl(IOChannelRef _Nonnull self, int cmd, va_list ap)
{
    switch (cmd) {
        case kIOChannelCommand_GetType:
            *((int*) va_arg(ap, int*)) = kIOChannelType_Terminal;
            return EOK;

        default:
            return Object_SuperN(ioctl, IOChannel, self, cmd, ap);
    }
}

CLASS_METHODS(ConsoleChannel, IOChannel,
OVERRIDE_METHOD_IMPL(ioctl, ConsoleChannel, IOChannel)
);


////////////////////////////////////////////////////////////////////////////////
// MARK: -
// MARK: Console
////////////////////////////////////////////////////////////////////////////////

static const RGBColor gANSIColors[8] = {
    {0x00, 0x00, 0x00},     // Black
    {0xff, 0x00, 0x00},     // Red
    {0x00, 0xff, 0x00},     // Green
    {0xff, 0xff, 0x00},     // Yellow
    {0x00, 0x00, 0xff},     // Blue
    {0xff, 0x00, 0xff},     // Magenta
    {0x00, 0xff, 0xff},     // Cyan
    {0xff, 0xff, 0xff},     // White
};

static const ColorTable gANSIColorTable = {
    8,
    gANSIColors
};


// Creates a new console object. This console will display its output on the
// provided graphics device.
// \param pEventDriver the event driver to provide keyboard input
// \param pGDevice the graphics device
// \return the console; NULL on failure
errno_t Console_Create(EventDriverRef _Nonnull pEventDriver, GraphicsDriverRef _Nonnull pGDevice, ConsoleRef _Nullable * _Nonnull pOutConsole)
{
    decl_try_err();
    Console* pConsole;

    try(Object_Create(Console, &pConsole));
    
    Lock_Init(&pConsole->lock);

    pConsole->eventDriver = Object_RetainAs(pEventDriver, EventDriver);
    try(IOResource_Open(pConsole->eventDriver, NULL/*XXX*/, kOpen_Read, kUser_Root, &pConsole->eventDriverChannel));
    try(RingBuffer_Init(&pConsole->reportsQueue, 4 * (MAX_MESSAGE_LENGTH + 1)));

    pConsole->gdevice = Object_RetainAs(pGDevice, GraphicsDriver);
    pConsole->keyMap = (const KeyMap*) gKeyMap_usa;

    pConsole->lineHeight = GLYPH_HEIGHT;
    pConsole->characterWidth = GLYPH_WIDTH;
    pConsole->compatibilityMode = kCompatibilityMode_ANSI;


    // Initialize the ANSI escape sequence parser
    vtparser_init(&pConsole->vtparser, (vt52parse_callback_t)Console_VT52_ParseByte_Locked, (vt500parse_callback_t)Console_VT100_ParseByte_Locked, pConsole);


    // Install an ANSI color table
    GraphicsDriver_SetCLUT(pGDevice, &gANSIColorTable);


    // Allocate the text cursor (sprite)
    const bool isInterlaced = ScreenConfiguration_IsInterlaced(GraphicsDriver_GetCurrentScreenConfiguration(pGDevice));
    const uint16_t* textCursorPlanes[2];
    textCursorPlanes[0] = (isInterlaced) ? &gBlock4x4_Plane0[0] : &gBlock4x8_Plane0[0];
    textCursorPlanes[1] = (isInterlaced) ? &gBlock4x4_Plane0[1] : &gBlock4x8_Plane0[1];
    const int textCursorWidth = (isInterlaced) ? gBlock4x4_Width : gBlock4x8_Width;
    const int textCursorHeight = (isInterlaced) ? gBlock4x4_Height : gBlock4x8_Height;
    try(GraphicsDriver_AcquireSprite(pGDevice, textCursorPlanes, 0, 0, textCursorWidth, textCursorHeight, 0, &pConsole->textCursor));
    pConsole->flags.isTextCursorVisible = false;


    // Allocate the text cursor blinking timer
    pConsole->flags.isTextCursorBlinkerEnabled = false;
    pConsole->flags.isTextCursorOn = false;
    pConsole->flags.isTextCursorSingleCycleOn = false;
    try(Timer_Create(kTimeInterval_Zero, TimeInterval_MakeMilliseconds(500), DispatchQueueClosure_Make((Closure1Arg_Func)Console_OnTextCursorBlink, pConsole), &pConsole->textCursorBlinker));


    // Reset the console to the default configuration
    try(Console_ResetState_Locked(pConsole));


    // Clear the console screen
    Console_ClearScreen_Locked(pConsole, kClearScreenMode_WhileAndScrollback);
    
    *pOutConsole = pConsole;
    return err;
    
catch:
    Object_Release(pConsole);
    *pOutConsole = NULL;
    return err;
}

// Deallocates the console.
// \param pConsole the console
void Console_deinit(ConsoleRef _Nonnull pConsole)
{
    Console_SetCursorBlinkingEnabled_Locked(pConsole, false);
    GraphicsDriver_RelinquishSprite(pConsole->gdevice, pConsole->textCursor);
    RingBuffer_Deinit(&pConsole->reportsQueue);

    Timer_Destroy(pConsole->textCursorBlinker);
    pConsole->textCursorBlinker = NULL;
    pConsole->keyMap = NULL;

    TabStops_Deinit(&pConsole->hTabStops);
        
    Lock_Deinit(&pConsole->lock);

    Object_Release(pConsole->gdevice);
    pConsole->gdevice = NULL;

    if (pConsole->eventDriverChannel) {
        IOResource_Close(pConsole->eventDriver, pConsole->eventDriverChannel);
        Object_Release(pConsole->eventDriverChannel);
        pConsole->eventDriverChannel = NULL;
    }
    Object_Release(pConsole->eventDriver);
    pConsole->eventDriver = NULL;
}

errno_t Console_ResetState_Locked(ConsoleRef _Nonnull pConsole)
{
    decl_try_err();
    const Surface* pFramebuffer;
    
    try_null(pFramebuffer, GraphicsDriver_GetFramebuffer(pConsole->gdevice), ENODEV);
    pConsole->bounds = Rect_Make(0, 0, pFramebuffer->width / pConsole->characterWidth, pFramebuffer->height / pConsole->lineHeight);
    pConsole->savedCursorState.x = 0;
    pConsole->savedCursorState.y = 0;

    Console_ResetCharacterAttributes_Locked(pConsole);

    TabStops_Deinit(&pConsole->hTabStops);
    try(TabStops_Init(&pConsole->hTabStops, __max(Rect_GetWidth(pConsole->bounds) / 8, 0), 8));

    Console_MoveCursorTo_Locked(pConsole, 0, 0);
    Console_SetCursorVisible_Locked(pConsole, true);
    Console_SetCursorBlinkingEnabled_Locked(pConsole, true);
    pConsole->flags.isAutoWrapEnabled = true;
    pConsole->flags.isInsertionMode = false;

    return EOK;

catch:
    return err;
}

void Console_ResetCharacterAttributes_Locked(ConsoleRef _Nonnull pConsole)
{
    Console_SetDefaultForegroundColor_Locked(pConsole);
    Console_SetDefaultBackgroundColor_Locked(pConsole);
    pConsole->characterRendition.isBold = 0;
    pConsole->characterRendition.isDimmed = 0;
    pConsole->characterRendition.isItalic = 0;
    pConsole->characterRendition.isUnderlined = 0;
    pConsole->characterRendition.isBlink = 0;
    pConsole->characterRendition.isReverse = 0;
    pConsole->characterRendition.isHidden = 0;
    pConsole->characterRendition.isStrikethrough = 0;
}

// Sets the console's foreground color to the given color
void Console_SetForegroundColor_Locked(ConsoleRef _Nonnull pConsole, Color color)
{
    assert(color.tag == kColorType_Index);
    pConsole->foregroundColor = color;

    // Sync up the sprite color registers with the selected foreground color
    GraphicsDriver_SetCLUTEntry(pConsole->gdevice, 17, &gANSIColors[color.u.index]);
    GraphicsDriver_SetCLUTEntry(pConsole->gdevice, 18, &gANSIColors[color.u.index]);
    GraphicsDriver_SetCLUTEntry(pConsole->gdevice, 19, &gANSIColors[color.u.index]);
}

// Sets the console's background color to the given color
void Console_SetBackgroundColor_Locked(ConsoleRef _Nonnull pConsole, Color color)
{
    assert(color.tag == kColorType_Index);
    pConsole->backgroundColor = color;
}

// Switches the console to the given compatibility mode
void Console_SetCompatibilityMode_Locked(ConsoleRef _Nonnull pConsole, CompatibilityMode mode)
{
    vtparser_mode_t vtmode;

    switch (mode) {
        case kCompatibilityMode_VT52:
            vtmode = VTPARSER_MODE_VT52;
            pConsole->flags.isInsertionMode = false;
            pConsole->flags.isAutoWrapEnabled = false;
            break;

        case kCompatibilityMode_VT52_AtariExtensions:
            vtmode = VTPARSER_MODE_VT52_ATARI;
            pConsole->flags.isInsertionMode = false;
            pConsole->flags.isAutoWrapEnabled = false;
            break;

        case kCompatibilityMode_VT100:
            vtmode = VTPARSER_MODE_VT100;
            break;

        default:
            abort();
            break;
    }

    vtparser_set_mode(&pConsole->vtparser, vtmode);
    pConsole->compatibilityMode = mode;
}

static void Console_DrawGlyph_Locked(ConsoleRef _Nonnull pConsole, char glyph, int x, int y)
{
    if (pConsole->characterRendition.isHidden) {
        glyph = ' ';
    }

    GraphicsDriver_BlitGlyph_8x8bw(
        pConsole->gdevice,
        &font8x8_latin1[glyph][0],
        x, y,
        (pConsole->characterRendition.isReverse) ? pConsole->backgroundColor : pConsole->foregroundColor,
        (pConsole->characterRendition.isReverse) ? pConsole->foregroundColor : pConsole->backgroundColor);
}

// Copies the content of 'srcRect' to 'dstLoc'. Does not change the cursor
// position.
static void Console_CopyRect_Locked(ConsoleRef _Nonnull pConsole, Rect srcRect, Point dstLoc)
{
    GraphicsDriver_CopyRect(pConsole->gdevice,
                            Rect_Make(srcRect.left * pConsole->characterWidth, srcRect.top * pConsole->lineHeight, srcRect.right * pConsole->characterWidth, srcRect.bottom * pConsole->lineHeight),
                            Point_Make(dstLoc.x * pConsole->characterWidth, dstLoc.y * pConsole->lineHeight));
}

// Fills the content of 'rect' with the character 'ch'. Does not change the
// cursor position.
static void Console_FillRect_Locked(ConsoleRef _Nonnull pConsole, Rect rect, char ch)
{
    const Rect r = Rect_Intersection(rect, pConsole->bounds);

    if (ch == ' ') {
        GraphicsDriver_FillRect(pConsole->gdevice,
                                Rect_Make(r.left * pConsole->characterWidth, r.top * pConsole->lineHeight, r.right * pConsole->characterWidth, r.bottom * pConsole->lineHeight),
                                (pConsole->characterRendition.isReverse) ? pConsole->foregroundColor : pConsole->backgroundColor);
    }
    else if (ch < 32 || ch == 127) {
        // Control characters -> do nothing
    }
    else {
        for (int y = r.top; y < r.bottom; y++) {
            for (int x = r.left; x < r.right; x++) {
                Console_DrawGlyph_Locked(pConsole, ch, x, y);
            }
        }
    }
}

// Scrolls the content of the console screen. 'clipRect' defines a viewport
// through which a virtual document is visible. This viewport is scrolled by
// 'dX' / 'dY' character cells. Positive values move the viewport down/right
// (and scroll the virtual document up/left) and negative values move the
// viewport up/left (and scroll the virtual document down/right).
static void Console_ScrollBy_Locked(ConsoleRef _Nonnull pConsole, int dX, int dY)
{
    const Rect clipRect = pConsole->bounds;
    const int absDx = __abs(dX), absDy = __abs(dY);

    if (absDx < Rect_GetWidth(clipRect) && absDy < Rect_GetHeight(clipRect)) {
        if (absDx > 0 || absDy > 0) {
            Rect copyRect;
            Point dstLoc;

            copyRect.left = (dX < 0) ? clipRect.left : clipRect.left + absDx;
            copyRect.top = (dY < 0) ? clipRect.top : clipRect.top + absDy;
            copyRect.right = (dX < 0) ? clipRect.right - absDx : clipRect.right;
            copyRect.bottom = (dY < 0) ? clipRect.bottom - absDy : clipRect.bottom;

            dstLoc.x = (dX < 0) ? clipRect.left + absDx : clipRect.left;
            dstLoc.y = (dY < 0) ? clipRect.top + absDy : clipRect.top;

            Console_CopyRect_Locked(pConsole, copyRect, dstLoc);
        }


        if (absDy > 0) {
            Rect vClearRect;

            vClearRect.left = clipRect.left;
            vClearRect.top = (dY < 0) ? clipRect.top : clipRect.bottom - absDy;
            vClearRect.right = clipRect.right;
            vClearRect.bottom = (dY < 0) ? clipRect.top + absDy : clipRect.bottom;
            Console_FillRect_Locked(pConsole, vClearRect, ' ');
        }


        if (absDx > 0) {
            Rect hClearRect;

            hClearRect.left = (dX < 0) ? clipRect.left : clipRect.right - absDx;
            hClearRect.top = (dY < 0) ? clipRect.top + absDy : clipRect.top;
            hClearRect.right = (dX < 0) ? clipRect.left + absDx : clipRect.right;
            hClearRect.bottom = (dY < 0) ? clipRect.bottom : clipRect.bottom - absDy;

            Console_FillRect_Locked(pConsole, hClearRect, ' ');
        }
    }
    else {
        Console_ClearScreen_Locked(pConsole, kClearScreenMode_Whole);
    }
}

// Clears the console screen.
// \param pConsole the console
// \param mode the clear screen mode
void Console_ClearScreen_Locked(ConsoleRef _Nonnull pConsole, ClearScreenMode mode)
{
    switch (mode) {
        case kClearScreenMode_ToEnd:
            Console_FillRect_Locked(pConsole, Rect_Make(pConsole->x, pConsole->y, pConsole->bounds.right, pConsole->y + 1), ' ');
            Console_FillRect_Locked(pConsole, Rect_Make(0, pConsole->y + 1, pConsole->bounds.right, pConsole->bounds.bottom), ' ');
            break;

        case kClearScreenMode_ToBeginning:
            Console_FillRect_Locked(pConsole, Rect_Make(0, pConsole->y, pConsole->x, pConsole->y + 1), ' ');
            Console_FillRect_Locked(pConsole, Rect_Make(0, 0, pConsole->bounds.right, pConsole->y - 1), ' ');
            break;

        case kClearScreenMode_Whole:
        case kClearScreenMode_WhileAndScrollback:
            GraphicsDriver_Clear(pConsole->gdevice);
            break;

        default:
            // Ignore
            break;
    }
}

// Clears the specified line. Does not change the cursor position.
void Console_ClearLine_Locked(ConsoleRef _Nonnull pConsole, int y, ClearLineMode mode)
{
    if (Rect_Contains(pConsole->bounds, 0, y)) {
        int left, right;

        switch (mode) {
            case kClearLineMode_ToEnd:
                left = pConsole->x;
                right = pConsole->bounds.right;
                break;

            case kClearLineMode_ToBeginning:
                left = 0;
                right = pConsole->x;
                break;

            case kClearLineMode_Whole:
                left = 0;
                right = pConsole->bounds.right;
                break;

            default:
                // Ignore
                return;
        }

        Console_FillRect_Locked(pConsole, Rect_Make(left, y, right, y + 1), ' ');
    }
}

void Console_SaveCursorState_Locked(ConsoleRef _Nonnull pConsole)
{
    pConsole->savedCursorState.x = pConsole->x;
    pConsole->savedCursorState.y = pConsole->y;
    pConsole->savedCursorState.foregroundColor = pConsole->foregroundColor;
    pConsole->savedCursorState.backgroundColor = pConsole->backgroundColor;
    pConsole->savedCursorState.characterRendition = pConsole->characterRendition;
}

void Console_RestoreCursorState_Locked(ConsoleRef _Nonnull pConsole)
{
    pConsole->characterRendition = pConsole->savedCursorState.characterRendition;
    Console_SetForegroundColor_Locked(pConsole, pConsole->savedCursorState.foregroundColor);
    Console_SetBackgroundColor_Locked(pConsole, pConsole->savedCursorState.backgroundColor);
    Console_MoveCursorTo_Locked(pConsole, pConsole->savedCursorState.x, pConsole->savedCursorState.y);
}

static void Console_OnTextCursorBlink(ConsoleRef _Nonnull pConsole)
{
    Lock_Lock(&pConsole->lock);
    
    pConsole->flags.isTextCursorOn = !pConsole->flags.isTextCursorOn;
    if (pConsole->flags.isTextCursorVisible) {
        GraphicsDriver_SetSpriteVisible(pConsole->gdevice, pConsole->textCursor, pConsole->flags.isTextCursorOn || pConsole->flags.isTextCursorSingleCycleOn);
    }
    pConsole->flags.isTextCursorSingleCycleOn = false;

    Lock_Unlock(&pConsole->lock);
}

static void Console_UpdateCursorVisibilityAndRestartBlinking_Locked(Console* _Nonnull pConsole)
{
    if (pConsole->flags.isTextCursorVisible) {
        // Changing the visibility to on should restart the blinking timer if
        // blinking is on too so that we always start out with a cursor-on phase
        DispatchQueue_RemoveTimer(gMainDispatchQueue, pConsole->textCursorBlinker);
        GraphicsDriver_SetSpriteVisible(pConsole->gdevice, pConsole->textCursor, true);
        pConsole->flags.isTextCursorOn = false;
        pConsole->flags.isTextCursorSingleCycleOn = false;

        if (pConsole->flags.isTextCursorBlinkerEnabled) {
            try_bang(DispatchQueue_DispatchTimer(gMainDispatchQueue, pConsole->textCursorBlinker));
        }
    } else {
        // Make sure that the text cursor and blinker are off
        DispatchQueue_RemoveTimer(gMainDispatchQueue, pConsole->textCursorBlinker);
        GraphicsDriver_SetSpriteVisible(pConsole->gdevice, pConsole->textCursor, false);
        pConsole->flags.isTextCursorOn = false;
        pConsole->flags.isTextCursorSingleCycleOn = false;
    }
}

void Console_SetCursorBlinkingEnabled_Locked(Console* _Nonnull pConsole, bool isEnabled)
{
    if (pConsole->flags.isTextCursorBlinkerEnabled != isEnabled) {
        pConsole->flags.isTextCursorBlinkerEnabled = isEnabled;
        Console_UpdateCursorVisibilityAndRestartBlinking_Locked(pConsole);
    }
}

void Console_SetCursorVisible_Locked(Console* _Nonnull pConsole, bool isVisible)
{
    if (pConsole->flags.isTextCursorVisible != isVisible) {
        pConsole->flags.isTextCursorVisible = isVisible;
        Console_UpdateCursorVisibilityAndRestartBlinking_Locked(pConsole);
    }
}

static void Console_CursorDidMove_Locked(Console* _Nonnull pConsole)
{
    GraphicsDriver_SetSpritePosition(pConsole->gdevice, pConsole->textCursor, pConsole->x * pConsole->characterWidth, pConsole->y * pConsole->lineHeight);
    // Temporarily force the cursor to be visible, but without changing the text
    // cursor visibility state officially. We just want to make sure that the
    // cursor is on when the user types a character. This however should not
    // change anything about the blinking phase and frequency.
    if (!pConsole->flags.isTextCursorSingleCycleOn && !pConsole->flags.isTextCursorOn && pConsole->flags.isTextCursorBlinkerEnabled && pConsole->flags.isTextCursorVisible) {
        pConsole->flags.isTextCursorSingleCycleOn = true;
        GraphicsDriver_SetSpriteVisible(pConsole->gdevice, pConsole->textCursor, true);
    }
}

// Moves the console position by the given delta values.
// \param pConsole the console
// \param mode how the cursor movement should be handled if it tries to go past the margins
// \param dx the X delta
// \param dy the Y delta
void Console_MoveCursor_Locked(ConsoleRef _Nonnull pConsole, CursorMovement mode, int dx, int dy)
{
    if (dx == 0 && dy == 0) {
        return;
    }
    
    const int aX = 0;
    const int aY = 0;
    const int eX = pConsole->bounds.right - 1;
    const int eY = pConsole->bounds.bottom - 1;
    int x = pConsole->x + dx;
    int y = pConsole->y + dy;

    switch (mode) {
        case kCursorMovement_Clamp:
            x = __max(__min(x, eX), aX);
            y = __max(__min(y, eY), aY);
            break;

        case kCursorMovement_AutoWrap:
            if (x < aX) {
                x = aX;
            }
            else if (x > eX) {
                x = aX;
                y++;
            }

            if (y < aY) {
                Console_ScrollBy_Locked(pConsole, 0, y);
                y = aY;
            }
            else if (y > eY) {
                Console_ScrollBy_Locked(pConsole, 0, y - eY);
                y = eY;
            }
            break;

        case kCursorMovement_AutoScroll:
            x = __max(__min(x, eX), aX);

            if (y < aY) {
                Console_ScrollBy_Locked(pConsole, 0, y);
                y = aY;
            }
            else if (y > eY) {
                Console_ScrollBy_Locked(pConsole, 0, y - eY);
                y = eY;
            }

            break;

        default:
            abort();
            break;
    }

    pConsole->x = x;
    pConsole->y = y;
    Console_CursorDidMove_Locked(pConsole);
}

// Sets the console position. The next print() will start printing at this
// location.
// \param pConsole the console
// \param x the X position
// \param y the Y position
void Console_MoveCursorTo_Locked(Console* _Nonnull pConsole, int x, int y)
{
    Console_MoveCursor_Locked(pConsole, kCursorMovement_Clamp, x - pConsole->x, y - pConsole->y);
}


////////////////////////////////////////////////////////////////////////////////
// Terminal reports queue
////////////////////////////////////////////////////////////////////////////////

// Posts a terminal report to the reports queue. The message length may not
// exceed MAX_MESSAGE_LENGTH.
void Console_PostReport_Locked(ConsoleRef _Nonnull pConsole, const char* msg)
{
    const ssize_t nBytesToWrite = String_Length(msg) + 1;
    assert(nBytesToWrite < (MAX_MESSAGE_LENGTH + 1));

    // Make space for the new message by removing the oldest (full) message(s)
    while (RingBuffer_WritableCount(&pConsole->reportsQueue) < nBytesToWrite) {
        char b;

        // Remove a full message
        do {
            RingBuffer_GetByte(&pConsole->reportsQueue, &b);
        } while (b != 0);
    }

    // Queue the new terminal report (including the trailing \0)
    RingBuffer_PutBytes(&pConsole->reportsQueue, msg, nBytesToWrite);
}


////////////////////////////////////////////////////////////////////////////////
// Processing input bytes
////////////////////////////////////////////////////////////////////////////////

// Interprets the given byte as a character, maps it to a glyph and prints it.
// \param pConsole the console
// \param ch the character
void Console_PrintByte_Locked(ConsoleRef _Nonnull pConsole, unsigned char ch)
{
    // The cursor position is always valid and inside the framebuffer
    if (pConsole->flags.isInsertionMode) {
        Console_CopyRect_Locked(pConsole, Rect_Make(pConsole->x, pConsole->y, pConsole->bounds.right - 1, pConsole->y + 1), Point_Make(pConsole->x + 1, pConsole->y));
    }

    Console_DrawGlyph_Locked(pConsole, ch, pConsole->x, pConsole->y);
    Console_MoveCursor_Locked(pConsole, (pConsole->flags.isAutoWrapEnabled) ? kCursorMovement_AutoWrap : kCursorMovement_Clamp, 1, 0);
}

void Console_Execute_BEL_Locked(ConsoleRef _Nonnull pConsole)
{
    // XXX implement me
    // XXX flash screen?
}

void Console_Execute_HT_Locked(ConsoleRef _Nonnull pConsole)
{
    Console_MoveCursorTo_Locked(pConsole, TabStops_GetNextStop(&pConsole->hTabStops, pConsole->x, Rect_GetWidth(pConsole->bounds)), pConsole->y);
}

// Line feed may be IND or NEL depending on a setting (that doesn't exist yet)
void Console_Execute_LF_Locked(ConsoleRef _Nonnull pConsole)
{
    Console_MoveCursor_Locked(pConsole, kCursorMovement_AutoScroll, -pConsole->x, 1);
}

void Console_Execute_BS_Locked(ConsoleRef _Nonnull pConsole)
{
    if (pConsole->x > 0) {
        // BS moves 1 cell to the left. It moves all characters in the line and
        // to the right of the cursor one column to the left if insertion mode
        // is enabled.
        if (pConsole->flags.isInsertionMode) {
            Console_CopyRect_Locked(pConsole, Rect_Make(pConsole->x, pConsole->y, pConsole->bounds.right, pConsole->y + 1), Point_Make(pConsole->x - 1, pConsole->y));
            Console_FillRect_Locked(pConsole, Rect_Make(pConsole->bounds.right - 1, pConsole->y, pConsole->bounds.right, pConsole->y + 1), ' ');
        }
        Console_MoveCursor_Locked(pConsole, kCursorMovement_Clamp, -1, 0);
    }
}

void Console_Execute_DEL_Locked(ConsoleRef _Nonnull pConsole)
{
    if (pConsole->x < pConsole->bounds.right - 1) {
        // DEL does not change the position.
        Console_CopyRect_Locked(pConsole, Rect_Make(pConsole->x + 1, pConsole->y, pConsole->bounds.right, pConsole->y + 1), Point_Make(pConsole->x, pConsole->y));
        Console_FillRect_Locked(pConsole, Rect_Make(pConsole->bounds.right - 1, pConsole->y, pConsole->bounds.right, pConsole->y + 1), ' ');
    }
}

void Console_Execute_DCH_Locked(ConsoleRef _Nonnull pConsole, int nChars)
{
    Console_CopyRect_Locked(pConsole, Rect_Make(pConsole->x + nChars, pConsole->y, pConsole->bounds.right - nChars, pConsole->y + 1), Point_Make(pConsole->x, pConsole->y));
    Console_FillRect_Locked(pConsole, Rect_Make(pConsole->bounds.right - nChars, pConsole->y, pConsole->bounds.right, pConsole->y + 1), ' ');
}

void Console_Execute_IL_Locked(ConsoleRef _Nonnull pConsole, int nLines)
{
    if (pConsole->y < pConsole->bounds.bottom) {
        Console_CopyRect_Locked(pConsole, Rect_Make(0, pConsole->y + 1, pConsole->bounds.right, pConsole->bounds.bottom - nLines), Point_Make(pConsole->x, pConsole->y + 2));
        Console_FillRect_Locked(pConsole, Rect_Make(0, pConsole->y + 1, pConsole->bounds.right, pConsole->y + 1 + nLines), ' ');
    }
}

void Console_Execute_DL_Locked(ConsoleRef _Nonnull pConsole, int nLines)
{
    Console_CopyRect_Locked(pConsole, Rect_Make(0, pConsole->y + 1, pConsole->bounds.right, pConsole->bounds.bottom - nLines), Point_Make(pConsole->x, pConsole->y));
    Console_FillRect_Locked(pConsole, Rect_Make(0, pConsole->bounds.bottom - nLines, pConsole->bounds.right, pConsole->bounds.bottom), ' ');
}


////////////////////////////////////////////////////////////////////////////////
// Read/Write
////////////////////////////////////////////////////////////////////////////////

errno_t Console_open(ConsoleRef _Nonnull pConsole, InodeRef _Nonnull _Locked pNode, unsigned int mode, User user, ConsoleChannelRef _Nullable * _Nonnull pOutChannel)
{
    decl_try_err();
    ConsoleChannelRef pChannel;

    try(IOChannel_AbstractCreate(&kConsoleChannelClass, (IOResourceRef) pConsole, mode, (IOChannelRef*)&pChannel));
    memset(pChannel->rdBuffer, 0, MAX_MESSAGE_LENGTH);
    pChannel->rdCount = 0;
    pChannel->rdIndex = 0;

catch:
    *pOutChannel = pChannel;
    return err;
}

errno_t Console_dup(ConsoleRef _Nonnull pConsole, ConsoleChannelRef _Nonnull pInChannel, ConsoleChannelRef _Nullable * _Nonnull pOutChannel)
{
    decl_try_err();
    ConsoleChannelRef pNewChannel;

    try(IOChannel_AbstractCreateCopy((IOChannelRef)pInChannel, (IOChannelRef*)&pNewChannel));
    memset(pNewChannel->rdBuffer, 0, MAX_MESSAGE_LENGTH);
    pNewChannel->rdCount = 0;
    pNewChannel->rdIndex = 0;

catch:
    *pOutChannel = pNewChannel;
    return err;

}

static void Console_ReadReports_NonBlocking_Locked(ConsoleRef _Nonnull pConsole, ConsoleChannelRef _Nonnull pChannel, char* _Nonnull pBuffer, ssize_t nBytesToRead, ssize_t* _Nonnull nOutBytesRead)
{
    ssize_t nBytesRead = 0;

    while (nBytesRead < nBytesToRead) {
        bool done = false;

        while (true) {
            char b;

            if (RingBuffer_GetByte(&pConsole->reportsQueue, &b) == 0) {
                done = true;
                break;
            }
            if (b == 0) {
                break;
            }

            pChannel->rdBuffer[pChannel->rdCount++] = b;
        }
        if (done) {
            break;
        }

        int i = 0;
        while (nBytesRead < nBytesToRead && pChannel->rdCount > 0) {
            pBuffer[nBytesRead++] = pChannel->rdBuffer[i++];
            pChannel->rdCount--;
        }

        if (pChannel->rdCount > 0) {
            // We ran out of space in the buffer that the user gave us. Remember
            // which bytes we need to copy next time read() is called.
            pChannel->rdIndex = i;
            break;
        }
    }
    
    *nOutBytesRead = nBytesRead;
}

static errno_t Console_ReadEvents_Locked(ConsoleRef _Nonnull pConsole, ConsoleChannelRef _Nonnull pChannel, char* _Nonnull pBuffer, ssize_t nBytesToRead, ssize_t* _Nonnull nOutBytesRead)
{
    decl_try_err();
    HIDEvent evt;
    ssize_t nBytesRead = 0;
    ssize_t nEvtBytesRead;

    while (nBytesRead < nBytesToRead) {
        // Drop the console lock while getting an event since the get events call
        // may block and holding the lock while being blocked for a potentially
        // long time would prevent any other process from working with the
        // console
        Lock_Unlock(&pConsole->lock);
        // XXX Need an API that allows me to read as many events as possible without blocking and that only blocks if there are no events available
        // XXX Or, probably, that's how the event driver read() should work in general
        const errno_t e1 = IOChannel_Read(pConsole->eventDriverChannel, &evt, sizeof(evt), &nEvtBytesRead);
        Lock_Lock(&pConsole->lock);
        // XXX we are currently assuming here that no relevant console state has
        // XXX changed while we didn't hold the lock. Confirm that this is okay
        if (e1 != EOK) {
            err = (nBytesRead == 0) ? e1 : EOK;
            break;
        }

        if (evt.type != kHIDEventType_KeyDown) {
            continue;
        }


        pChannel->rdCount = KeyMap_Map(pConsole->keyMap, &evt.data.key, pChannel->rdBuffer, MAX_MESSAGE_LENGTH);

        int i = 0;
        while (nBytesRead < nBytesToRead && pChannel->rdCount > 0) {
            pBuffer[nBytesRead++] = pChannel->rdBuffer[i++];
            pChannel->rdCount--;
        }

        if (pChannel->rdCount > 0) {
            // We ran out of space in the buffer that the user gave us. Remember
            // which bytes we need to copy next time read() is called.
            pChannel->rdIndex = i;
            break;
        }
    }
    
    *nOutBytesRead = nBytesRead;
    return err;
}

// Note that this read implementation will only block if there is no buffered
// data, no terminal reports and no events are available. It tries to do a
// non-blocking read as hard as possible even if it can't fully fill the user
// provided buffer. 
errno_t Console_read(ConsoleRef _Nonnull pConsole, ConsoleChannelRef _Nonnull pChannel, void* _Nonnull pBuffer, ssize_t nBytesToRead, ssize_t* _Nonnull nOutBytesRead)
{
    decl_try_err();
    char* pChars = pBuffer;
    HIDEvent evt;
    int evtCount;
    ssize_t nBytesRead = 0;
    ssize_t nTmpBytesRead;

    Lock_Lock(&pConsole->lock);

    // First check whether we got a partial key byte sequence sitting in our key
    // mapping buffer and copy that one out.
    while (nBytesRead < nBytesToRead && pChannel->rdCount > 0) {
        pChars[nBytesRead++] = pChannel->rdBuffer[pChannel->rdIndex++];
        pChannel->rdCount--;
    }


    if (!RingBuffer_IsEmpty(&pConsole->reportsQueue)) {
        // Now check whether there are terminal reports pending. Those take
        // priority over input device events.
        Console_ReadReports_NonBlocking_Locked(pConsole, pChannel, &pChars[nBytesRead], nBytesToRead - nBytesRead, &nTmpBytesRead);
        nBytesRead += nTmpBytesRead;
    }


    if (nBytesRead == 0 && err == EOK) {
        // We haven't read any data so far. Read input events and block if none
        // are available either.
        const errno_t e1 = Console_ReadEvents_Locked(pConsole, pChannel, &pChars[nBytesRead], nBytesToRead - nBytesRead, &nTmpBytesRead);
        if (e1 == EOK) {
            nBytesRead += nTmpBytesRead;
        } else {
            err = (nBytesRead == 0) ? e1 : EOK;
        }
    }

    Lock_Unlock(&pConsole->lock);

    *nOutBytesRead = nBytesRead;
    return err;
}

// Writes the given byte sequence of characters to the console.
// \param pConsole the console
// \param pBytes the byte sequence
// \param nBytes the number of bytes to write
// \return the number of bytes written; a negative error code if an error was encountered
errno_t Console_write(ConsoleRef _Nonnull pConsole, ConsoleChannelRef _Nonnull pChannel, const void* _Nonnull pBytes, ssize_t nBytesToWrite, ssize_t* _Nonnull nOutBytesWritten)
{
    const unsigned char* pChars = pBytes;
    const unsigned char* pCharsEnd = pChars + nBytesToWrite;

    Lock_Lock(&pConsole->lock);
    while (pChars < pCharsEnd) {
        const unsigned char by = *pChars++;

        vtparser_byte(&pConsole->vtparser, by);
    }
    Lock_Unlock(&pConsole->lock);

    *nOutBytesWritten = nBytesToWrite;
    return EOK;
}


CLASS_METHODS(Console, IOResource,
OVERRIDE_METHOD_IMPL(open, Console, IOResource)
OVERRIDE_METHOD_IMPL(dup, Console, IOResource)
OVERRIDE_METHOD_IMPL(read, Console, IOResource)
OVERRIDE_METHOD_IMPL(write, Console, IOResource)
OVERRIDE_METHOD_IMPL(deinit, Console, Object)
);
