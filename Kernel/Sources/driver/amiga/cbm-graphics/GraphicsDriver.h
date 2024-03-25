//
//  GraphicsDriver.h
//  kernel
//
//  Created by Dietmar Planitzer on 2/7/21.
//  Copyright © 2021 Dietmar Planitzer. All rights reserved.
//

#ifndef GraphicsDriver_h
#define GraphicsDriver_h

#include <klib/klib.h>
#include "IOResource.h"
#include "Color.h"
#include "Surface.h"


typedef struct ColorTable {
    size_t                      entryCount;
    const RGBColor32* _Nonnull  entry;
} ColorTable;


struct _ScreenConfiguration;
typedef struct _ScreenConfiguration ScreenConfiguration;

// The supported screen configurations
extern const ScreenConfiguration kScreenConfig_NTSC_320_200_60;
extern const ScreenConfiguration kScreenConfig_NTSC_640_200_60;
extern const ScreenConfiguration kScreenConfig_NTSC_320_400_30;
extern const ScreenConfiguration kScreenConfig_NTSC_640_400_30;

extern const ScreenConfiguration kScreenConfig_PAL_320_256_50;
extern const ScreenConfiguration kScreenConfig_PAL_640_256_50;
extern const ScreenConfiguration kScreenConfig_PAL_320_512_25;
extern const ScreenConfiguration kScreenConfig_PAL_640_512_25;

extern int ScreenConfiguration_GetPixelWidth(const ScreenConfiguration* pConfig);
extern int ScreenConfiguration_GetPixelHeight(const ScreenConfiguration* pConfig);
extern int ScreenConfiguration_GetRefreshRate(const ScreenConfiguration* pConfig);
extern bool ScreenConfiguration_IsInterlaced(const ScreenConfiguration* pConfig);


OPAQUE_CLASS(GraphicsDriver, IOResource);
typedef struct _GraphicsDriverMethodTable {
    IOResourceMethodTable   super;
} GraphicsDriverMethodTable;


typedef int SpriteID;


extern errno_t GraphicsDriver_Create(const ScreenConfiguration* _Nonnull pConfig, PixelFormat pixelFormat, GraphicsDriverRef _Nullable * _Nonnull pOutDriver);
extern void GraphicsDriver_Destroy(GraphicsDriverRef _Nullable pDriver);

extern const ScreenConfiguration* _Nonnull GraphicsDriver_GetCurrentScreenConfiguration(GraphicsDriverRef _Nonnull pDriver);

extern Surface* _Nullable GraphicsDriver_GetFramebuffer(GraphicsDriverRef _Nonnull pDriver);
extern Size GraphicsDriver_GetFramebufferSize(GraphicsDriverRef _Nonnull pDriver);

extern errno_t GraphicsDriver_SetLightPenEnabled(GraphicsDriverRef _Nonnull pDriver, bool enabled);
extern bool GraphicsDriver_GetLightPenPosition(GraphicsDriverRef _Nonnull pDriver, int16_t* _Nonnull pPosX, int16_t* _Nonnull pPosY);


// Sprites
extern errno_t GraphicsDriver_AcquireSprite(GraphicsDriverRef _Nonnull pDriver, const uint16_t* _Nonnull pPlanes[2], int x, int y, int width, int height, int priority, SpriteID* _Nonnull pOutSpriteId);
extern errno_t GraphicsDriver_RelinquishSprite(GraphicsDriverRef _Nonnull pDriver, SpriteID spriteId);
extern errno_t GraphicsDriver_SetSpritePosition(GraphicsDriverRef _Nonnull pDriver, SpriteID spriteId, int x, int y);
extern errno_t GraphicsDriver_SetSpriteVisible(GraphicsDriverRef _Nonnull pDriver, SpriteID spriteId, bool isVisible);


// Mouse Cursor
extern void GraphicsDriver_SetMouseCursor(GraphicsDriverRef _Nonnull pDriver, const void* pBitmap, const void* pMask);
extern void GraphicsDriver_SetMouseCursorVisible(GraphicsDriverRef _Nonnull pDriver, bool isVisible);
extern void GraphicsDriver_SetMouseCursorHiddenUntilMouseMoves(GraphicsDriverRef _Nonnull pDriver, bool flag);
extern void GraphicsDriver_SetMouseCursorPosition(GraphicsDriverRef _Nonnull pDriver, Point loc);
extern void GraphicsDriver_SetMouseCursorPositionFromInterruptContext(GraphicsDriverRef _Nonnull pDriver, int16_t x, int16_t y);


// CLUT
extern errno_t GraphicsDriver_SetCLUTEntry(GraphicsDriverRef _Nonnull pDriver, int idx, RGBColor32 color);
extern void GraphicsDriver_SetCLUT(GraphicsDriverRef _Nonnull pDriver, const ColorTable* pCLUT);


// Drawing
extern void GraphicsDriver_Clear(GraphicsDriverRef _Nonnull pDriver);
extern void GraphicsDriver_FillRect(GraphicsDriverRef _Nonnull pDriver, Rect rect, Color color);
extern void GraphicsDriver_CopyRect(GraphicsDriverRef _Nonnull pDriver, Rect srcRect, Point dstLoc);
extern void GraphicsDriver_BlitGlyph_8x8bw(GraphicsDriverRef _Nonnull pDriver, const void* _Nonnull pGlyphBitmap, int x, int y, Color fgColor, Color bgColor);

#endif /* GraphicsDriver_h */
