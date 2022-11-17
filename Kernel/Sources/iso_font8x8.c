//
//  iso_font8x8.c
//  Apollo
//
//  Created by Dietmar Planitzer on 2/9/21.
//  Copyright © 2021 Dietmar Planitzer. All rights reserved.
//
/**
 * 8x8 monochrome bitmap fonts for rendering
 * Author: Daniel Hepper <daniel@hepper.net>
 *
 * License: Public Domain
 *
 * Based on:
 * // Summary: font8x8.h
 * // 8x8 monochrome bitmap fonts for rendering
 * //
 * // Author:
 * //     Marcel Sondaar
 * //     International Business Machines (public domain VGA fonts)
 * //
 * // License:
 * //     Public Domain
 *
 * Fetched from: http://dimensionalrift.homelinux.net/combuster/mos3/?p=viewsource&file=/modules/gfx/font8_8.asm
 **/

#include "Foundation.h"


// Constant: font8x8_basic
// Contains an 8x8 font map for unicode points U+0000 - U+007F (basic latin)
const Byte font8x8_latin1[256][8] = {
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+0000
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+0001
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+0002
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+0003
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+0004
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+0005
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+0006
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+0007
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+0008
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+0009
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+000A
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+000B
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+000C
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+000D
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+000E
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+000F
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+0010
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+0011
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+0012
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+0013
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+0014
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+0015
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+0016
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+0017
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+0018
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+0019
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+001A
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+001B
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+001C
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+001D
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+001E
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+001F
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+0020
    { 0x18, 0x3c, 0x3c, 0x18, 0x18, 0x00, 0x18, 0x00},   // U+0021
    { 0x6c, 0x6c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+0022
    { 0x6c, 0x6c, 0xfe, 0x6c, 0xfe, 0x6c, 0x6c, 0x00},   // U+0023
    { 0x30, 0x7c, 0xc0, 0x78, 0x0c, 0xf8, 0x30, 0x00},   // U+0024
    { 0x00, 0xc6, 0xcc, 0x18, 0x30, 0x66, 0xc6, 0x00},   // U+0025
    { 0x38, 0x6c, 0x38, 0x76, 0xdc, 0xcc, 0x76, 0x00},   // U+0026
    { 0x60, 0x60, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+0027
    { 0x18, 0x30, 0x60, 0x60, 0x60, 0x30, 0x18, 0x00},   // U+0028
    { 0x60, 0x30, 0x18, 0x18, 0x18, 0x30, 0x60, 0x00},   // U+0029
    { 0x00, 0x66, 0x3c, 0xff, 0x3c, 0x66, 0x00, 0x00},   // U+002A
    { 0x00, 0x30, 0x30, 0xfc, 0x30, 0x30, 0x00, 0x00},   // U+002B
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x30, 0x60},   // U+002C
    { 0x00, 0x00, 0x00, 0xfc, 0x00, 0x00, 0x00, 0x00},   // U+002D
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x30, 0x00},   // U+002E
    { 0x06, 0x0c, 0x18, 0x30, 0x60, 0xc0, 0x80, 0x00},   // U+002F
    { 0x7c, 0xc6, 0xce, 0xde, 0xf6, 0xe6, 0x7c, 0x00},   // U+0030
    { 0x30, 0x70, 0x30, 0x30, 0x30, 0x30, 0xfc, 0x00},   // U+0031
    { 0x78, 0xcc, 0x0c, 0x38, 0x60, 0xcc, 0xfc, 0x00},   // U+0032
    { 0x78, 0xcc, 0x0c, 0x38, 0x0c, 0xcc, 0x78, 0x00},   // U+0033
    { 0x1c, 0x3c, 0x6c, 0xcc, 0xfe, 0x0c, 0x1e, 0x00},   // U+0034
    { 0xfc, 0xc0, 0xf8, 0x0c, 0x0c, 0xcc, 0x78, 0x00},   // U+0035
    { 0x38, 0x60, 0xc0, 0xf8, 0xcc, 0xcc, 0x78, 0x00},   // U+0036
    { 0xfc, 0xcc, 0x0c, 0x18, 0x30, 0x30, 0x30, 0x00},   // U+0037
    { 0x78, 0xcc, 0xcc, 0x78, 0xcc, 0xcc, 0x78, 0x00},   // U+0038
    { 0x78, 0xcc, 0xcc, 0x7c, 0x0c, 0x18, 0x70, 0x00},   // U+0039
    { 0x00, 0x30, 0x30, 0x00, 0x00, 0x30, 0x30, 0x00},   // U+003A
    { 0x00, 0x30, 0x30, 0x00, 0x00, 0x30, 0x30, 0x60},   // U+003B
    { 0x18, 0x30, 0x60, 0xc0, 0x60, 0x30, 0x18, 0x00},   // U+003C
    { 0x00, 0x00, 0xfc, 0x00, 0x00, 0xfc, 0x00, 0x00},   // U+003D
    { 0x60, 0x30, 0x18, 0x0c, 0x18, 0x30, 0x60, 0x00},   // U+003E
    { 0x78, 0xcc, 0x0c, 0x18, 0x30, 0x00, 0x30, 0x00},   // U+003F
    { 0x7c, 0xc6, 0xde, 0xde, 0xde, 0xc0, 0x78, 0x00},   // U+0040
    { 0x30, 0x78, 0xcc, 0xcc, 0xfc, 0xcc, 0xcc, 0x00},   // U+0041
    { 0xfc, 0x66, 0x66, 0x7c, 0x66, 0x66, 0xfc, 0x00},   // U+0042
    { 0x3c, 0x66, 0xc0, 0xc0, 0xc0, 0x66, 0x3c, 0x00},   // U+0043
    { 0xf8, 0x6c, 0x66, 0x66, 0x66, 0x6c, 0xf8, 0x00},   // U+0044
    { 0xfe, 0x62, 0x68, 0x78, 0x68, 0x62, 0xfe, 0x00},   // U+0045
    { 0xfe, 0x62, 0x68, 0x78, 0x68, 0x60, 0xf0, 0x00},   // U+0046
    { 0x3c, 0x66, 0xc0, 0xc0, 0xce, 0x66, 0x3e, 0x00},   // U+0047
    { 0xcc, 0xcc, 0xcc, 0xfc, 0xcc, 0xcc, 0xcc, 0x00},   // U+0048
    { 0x78, 0x30, 0x30, 0x30, 0x30, 0x30, 0x78, 0x00},   // U+0049
    { 0x1e, 0x0c, 0x0c, 0x0c, 0xcc, 0xcc, 0x78, 0x00},   // U+004A
    { 0xe6, 0x66, 0x6c, 0x78, 0x6c, 0x66, 0xe6, 0x00},   // U+004B
    { 0xf0, 0x60, 0x60, 0x60, 0x62, 0x66, 0xfe, 0x00},   // U+004C
    { 0xc6, 0xee, 0xfe, 0xfe, 0xd6, 0xc6, 0xc6, 0x00},   // U+004D
    { 0xc6, 0xe6, 0xf6, 0xde, 0xce, 0xc6, 0xc6, 0x00},   // U+004E
    { 0x38, 0x6c, 0xc6, 0xc6, 0xc6, 0x6c, 0x38, 0x00},   // U+004F
    { 0xfc, 0x66, 0x66, 0x7c, 0x60, 0x60, 0xf0, 0x00},   // U+0050
    { 0x78, 0xcc, 0xcc, 0xcc, 0xdc, 0x78, 0x1c, 0x00},   // U+0051
    { 0xfc, 0x66, 0x66, 0x7c, 0x6c, 0x66, 0xe6, 0x00},   // U+0052
    { 0x78, 0xcc, 0xe0, 0x70, 0x1c, 0xcc, 0x78, 0x00},   // U+0053
    { 0xfc, 0xb4, 0x30, 0x30, 0x30, 0x30, 0x78, 0x00},   // U+0054
    { 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xfc, 0x00},   // U+0055
    { 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0x78, 0x30, 0x00},   // U+0056
    { 0xc6, 0xc6, 0xc6, 0xd6, 0xfe, 0xee, 0xc6, 0x00},   // U+0057
    { 0xc6, 0xc6, 0x6c, 0x38, 0x38, 0x6c, 0xc6, 0x00},   // U+0058
    { 0xcc, 0xcc, 0xcc, 0x78, 0x30, 0x30, 0x78, 0x00},   // U+0059
    { 0xfe, 0xc6, 0x8c, 0x18, 0x32, 0x66, 0xfe, 0x00},   // U+005A
    { 0x78, 0x60, 0x60, 0x60, 0x60, 0x60, 0x78, 0x00},   // U+005B
    { 0xc0, 0x60, 0x30, 0x18, 0x0c, 0x06, 0x02, 0x00},   // U+005C
    { 0x78, 0x18, 0x18, 0x18, 0x18, 0x18, 0x78, 0x00},   // U+005D
    { 0x10, 0x38, 0x6c, 0xc6, 0x00, 0x00, 0x00, 0x00},   // U+005E
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff},   // U+005F
    { 0x30, 0x30, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+0060
    { 0x00, 0x00, 0x78, 0x0c, 0x7c, 0xcc, 0x76, 0x00},   // U+0061
    { 0xe0, 0x60, 0x60, 0x7c, 0x66, 0x66, 0xdc, 0x00},   // U+0062
    { 0x00, 0x00, 0x78, 0xcc, 0xc0, 0xcc, 0x78, 0x00},   // U+0063
    { 0x1c, 0x0c, 0x0c, 0x7c, 0xcc, 0xcc, 0x76, 0x00},   // U+0064
    { 0x00, 0x00, 0x78, 0xcc, 0xfc, 0xc0, 0x78, 0x00},   // U+0065
    { 0x38, 0x6c, 0x60, 0xf0, 0x60, 0x60, 0xf0, 0x00},   // U+0066
    { 0x00, 0x00, 0x76, 0xcc, 0xcc, 0x7c, 0x0c, 0xf8},   // U+0067
    { 0xe0, 0x60, 0x6c, 0x76, 0x66, 0x66, 0xe6, 0x00},   // U+0068
    { 0x30, 0x00, 0x70, 0x30, 0x30, 0x30, 0x78, 0x00},   // U+0069
    { 0x0c, 0x00, 0x0c, 0x0c, 0x0c, 0xcc, 0xcc, 0x78},   // U+006A
    { 0xe0, 0x60, 0x66, 0x6c, 0x78, 0x6c, 0xe6, 0x00},   // U+006B
    { 0x70, 0x30, 0x30, 0x30, 0x30, 0x30, 0x78, 0x00},   // U+006C
    { 0x00, 0x00, 0xcc, 0xfe, 0xfe, 0xd6, 0xc6, 0x00},   // U+006D
    { 0x00, 0x00, 0xf8, 0xcc, 0xcc, 0xcc, 0xcc, 0x00},   // U+006E
    { 0x00, 0x00, 0x78, 0xcc, 0xcc, 0xcc, 0x78, 0x00},   // U+006F
    { 0x00, 0x00, 0xdc, 0x66, 0x66, 0x7c, 0x60, 0xf0},   // U+0070
    { 0x00, 0x00, 0x76, 0xcc, 0xcc, 0x7c, 0x0c, 0x1e},   // U+0071
    { 0x00, 0x00, 0xdc, 0x76, 0x66, 0x60, 0xf0, 0x00},   // U+0072
    { 0x00, 0x00, 0x7c, 0xc0, 0x78, 0x0c, 0xf8, 0x00},   // U+0073
    { 0x10, 0x30, 0x7c, 0x30, 0x30, 0x34, 0x18, 0x00},   // U+0074
    { 0x00, 0x00, 0xcc, 0xcc, 0xcc, 0xcc, 0x76, 0x00},   // U+0075
    { 0x00, 0x00, 0xcc, 0xcc, 0xcc, 0x78, 0x30, 0x00},   // U+0076
    { 0x00, 0x00, 0xc6, 0xd6, 0xfe, 0xfe, 0x6c, 0x00},   // U+0077
    { 0x00, 0x00, 0xc6, 0x6c, 0x38, 0x6c, 0xc6, 0x00},   // U+0078
    { 0x00, 0x00, 0xcc, 0xcc, 0xcc, 0x7c, 0x0c, 0xf8},   // U+0079
    { 0x00, 0x00, 0xfc, 0x98, 0x30, 0x64, 0xfc, 0x00},   // U+007A
    { 0x1c, 0x30, 0x30, 0xe0, 0x30, 0x30, 0x1c, 0x00},   // U+007B
    { 0x18, 0x18, 0x18, 0x00, 0x18, 0x18, 0x18, 0x00},   // U+007C
    { 0xe0, 0x30, 0x30, 0x1c, 0x30, 0x30, 0xe0, 0x00},   // U+007D
    { 0x76, 0xdc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+007E
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+007F
    
    // Constant: font8x8_0080
    // Contains an 8x8 font map for unicode points U+0080 - U+009F (C1/C2 control)
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+0080
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+0081
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+0082
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+0083
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+0084
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+0085
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+0086
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+0087
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+0088
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+0089
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+008A
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+008B
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+008C
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+008D
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+008E
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+008F
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+0090
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+0091
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+0092
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+0093
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+0094
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+0095
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+0096
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+0097
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+0098
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+0099
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+009A
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+009B
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+009C
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+009D
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+009E
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+009F
    
    // Constant: font8x8_00A0
    // Contains an 8x8 font map for unicode points U+00A0 - U+00FF (extended latin)
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+00A0
    { 0x18, 0x18, 0x00, 0x18, 0x18, 0x18, 0x18, 0x00},   // U+00A1
    { 0x18, 0x18, 0x7e, 0xc0, 0xc0, 0x7e, 0x18, 0x18},   // U+00A2
    { 0x38, 0x6c, 0x64, 0xf0, 0x60, 0xe6, 0xfc, 0x00},   // U+00A3
    { 0x00, 0x00, 0xc6, 0x7c, 0x6c, 0x7c, 0xc6, 0x00},   // U+00A4
    { 0xcc, 0xcc, 0x78, 0xfc, 0x30, 0xfc, 0x30, 0x30},   // U+00A5
    { 0x18, 0x18, 0x18, 0x00, 0x18, 0x18, 0x18, 0x00},   // U+00A6
    { 0x3e, 0x63, 0x38, 0x6c, 0x6c, 0x38, 0xcc, 0x78},   // U+00A7
    { 0xcc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+00A8
    { 0x3c, 0x42, 0x99, 0xa1, 0xa1, 0x99, 0x42, 0x3c},   // U+00A9
    { 0x3c, 0x6c, 0x6c, 0x3e, 0x00, 0x00, 0x00, 0x00},   // U+00AA
    { 0x00, 0x33, 0x66, 0xcc, 0x66, 0x33, 0x00, 0x00},   // U+00AB
    { 0x00, 0x00, 0x00, 0xfc, 0x0c, 0x0c, 0x00, 0x00},   // U+00AC
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+00AD
    { 0x3c, 0x42, 0xb9, 0xa5, 0xb9, 0xa5, 0x42, 0x3c},   // U+00AE
    { 0x7e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+00AF
    { 0x38, 0x6c, 0x6c, 0x38, 0x00, 0x00, 0x00, 0x00},   // U+00B0
    { 0x18, 0x18, 0x7e, 0x18, 0x18, 0x00, 0x7e, 0x00},   // U+00B1
    { 0x38, 0x0c, 0x18, 0x30, 0x3c, 0x00, 0x00, 0x00},   // U+00B2
    { 0x38, 0x0c, 0x18, 0x0c, 0x38, 0x00, 0x00, 0x00},   // U+00B3
    { 0x18, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+00B4
    { 0x00, 0x00, 0x66, 0x66, 0x66, 0x7c, 0x60, 0xc0},   // U+00B5
    { 0x7f, 0xdb, 0xdb, 0x7b, 0x1b, 0x1b, 0x1b, 0x00},   // U+00B6
    { 0x00, 0x00, 0x00, 0x18, 0x18, 0x00, 0x00, 0x00},   // U+00B7
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x0c, 0x78},   // U+00B8
    { 0x10, 0x30, 0x10, 0x38, 0x00, 0x00, 0x00, 0x00},   // U+00B9
    { 0x38, 0x6c, 0x6c, 0x38, 0x00, 0x00, 0x00, 0x00},   // U+00BA
    { 0x00, 0xcc, 0x66, 0x33, 0x66, 0xcc, 0x00, 0x00},   // U+00BB
    { 0xc3, 0xc6, 0xcc, 0xbd, 0x37, 0x6f, 0xcf, 0xc0},   // U+00BC
    { 0xc3, 0xc6, 0xcc, 0xde, 0x33, 0x66, 0xcc, 0x0f},   // U+00BD
    { 0xc0, 0x23, 0xc6, 0x2d, 0xdb, 0x35, 0x67, 0x01},   // U+00BE
    { 0x30, 0x00, 0x30, 0x60, 0xc0, 0xcc, 0x78, 0x00},   // U+00BF
    { 0xe0, 0x00, 0x38, 0x6c, 0xc6, 0xfe, 0xc6, 0x00},   // U+00C0
    { 0x0e, 0x00, 0x38, 0x6c, 0xc6, 0xfe, 0xc6, 0x00},   // U+00C1
    { 0x38, 0x6c, 0x00, 0x7c, 0xc6, 0xfe, 0xc6, 0x00},   // U+00C2
    { 0x76, 0xdc, 0x00, 0x7c, 0xc6, 0xfe, 0xc6, 0x00},   // U+00C3
    { 0xc6, 0x38, 0x6c, 0xc6, 0xfe, 0xc6, 0xc6, 0x00},   // U+00C4
    { 0x30, 0x30, 0x00, 0x78, 0xcc, 0xfc, 0xcc, 0x00},   // U+00C5
    { 0x3e, 0x6c, 0xcc, 0xfe, 0xcc, 0xcc, 0xce, 0x00},   // U+00C6
    { 0x78, 0xcc, 0xc0, 0xcc, 0x78, 0x18, 0x0c, 0x78},   // U+00C7
    { 0xe0, 0x00, 0xfc, 0x60, 0x78, 0x60, 0xfc, 0x00},   // U+00C8
    { 0x1c, 0x00, 0xfc, 0x60, 0x78, 0x60, 0xfc, 0x00},   // U+00C9
    { 0x30, 0x48, 0xfc, 0x60, 0x78, 0x60, 0xfc, 0x00},   // U+00CA
    { 0x6c, 0x00, 0xfc, 0x60, 0x78, 0x60, 0xfc, 0x00},   // U+00CB
    { 0xe0, 0x00, 0x78, 0x30, 0x30, 0x30, 0x78, 0x00},   // U+00CC
    { 0x1c, 0x00, 0x78, 0x30, 0x30, 0x30, 0x78, 0x00},   // U+00CD
    { 0x30, 0x48, 0x00, 0x78, 0x30, 0x30, 0x78, 0x00},   // U+00CE
    { 0xcc, 0x00, 0x78, 0x30, 0x30, 0x30, 0x78, 0x00},   // U+00CF
    { 0xfc, 0x66, 0xf6, 0xf6, 0x66, 0x66, 0xfc, 0x00},   // U+00D0
    { 0xfc, 0x00, 0xcc, 0xec, 0xfc, 0xdc, 0xcc, 0x00},   // U+00D1
    { 0x70, 0x00, 0x18, 0x3c, 0x66, 0x3c, 0x18, 0x00},   // U+00D2
    { 0x0e, 0x00, 0x18, 0x3c, 0x66, 0x3c, 0x18, 0x00},   // U+00D3
    { 0x3c, 0x66, 0x18, 0x3c, 0x66, 0x3c, 0x18, 0x00},   // U+00D4
    { 0x76, 0xdc, 0x00, 0x7c, 0xc6, 0xc6, 0x7c, 0x00},   // U+00D5
    { 0xc3, 0x18, 0x3c, 0x66, 0x66, 0x3c, 0x18, 0x00},   // U+00D6
    { 0x00, 0x6c, 0x38, 0x10, 0x38, 0x6c, 0x00, 0x00},   // U+00D7
    { 0x3a, 0x6c, 0xce, 0xde, 0xf6, 0x6c, 0xb8, 0x00},   // U+00D8
    { 0x70, 0x00, 0x66, 0x66, 0x66, 0x66, 0x3c, 0x00},   // U+00D9
    { 0x0e, 0x00, 0x66, 0x66, 0x66, 0x66, 0x3c, 0x00},   // U+00DA
    { 0x3c, 0x66, 0x00, 0x66, 0x66, 0x66, 0x3c, 0x00},   // U+00DB
    { 0xcc, 0x00, 0xcc, 0xcc, 0xcc, 0xcc, 0x78, 0x00},   // U+00DC
    { 0x0e, 0x00, 0x66, 0x66, 0x3c, 0x18, 0x18, 0x00},   // U+00DD
    { 0xf0, 0x60, 0x7c, 0x66, 0x66, 0x7c, 0x60, 0xf0},   // U+00DE
    { 0x00, 0x78, 0xcc, 0xf8, 0xcc, 0xf8, 0xc0, 0xc0},   // U+00DF
    { 0xe0, 0x00, 0x78, 0x0c, 0x7c, 0xcc, 0x7e, 0x00},   // U+00E0
    { 0x1c, 0x00, 0x78, 0x0c, 0x7c, 0xcc, 0x7e, 0x00},   // U+00E1
    { 0x7e, 0xc3, 0x3c, 0x06, 0x3e, 0x66, 0x3f, 0x00},   // U+00E2
    { 0x76, 0xdc, 0x78, 0x0c, 0x7c, 0xcc, 0x7e, 0x00},   // U+00E3
    { 0xcc, 0x00, 0x78, 0x0c, 0x7c, 0xcc, 0x7e, 0x00},   // U+00E4
    { 0x30, 0x30, 0x78, 0x0c, 0x7c, 0xcc, 0x7e, 0x00},   // U+00E5
    { 0x00, 0x00, 0x7f, 0x0c, 0x7f, 0xcc, 0x7f, 0x00},   // U+00E6
    { 0x00, 0x00, 0x78, 0xc0, 0xc0, 0x78, 0x0c, 0x38},   // U+00E7
    { 0xe0, 0x00, 0x78, 0xcc, 0xfc, 0xc0, 0x78, 0x00},   // U+00E8
    { 0x1c, 0x00, 0x78, 0xcc, 0xfc, 0xc0, 0x78, 0x00},   // U+00E9
    { 0x7e, 0xc3, 0x3c, 0x66, 0x7e, 0x60, 0x3c, 0x00},   // U+00EA
    { 0xcc, 0x00, 0x78, 0xcc, 0xfc, 0xc0, 0x78, 0x00},   // U+00EB
    { 0xe0, 0x00, 0x70, 0x30, 0x30, 0x30, 0x78, 0x00},   // U+00EC
    { 0x38, 0x00, 0x70, 0x30, 0x30, 0x30, 0x78, 0x00},   // U+00ED
    { 0x7c, 0xc6, 0x38, 0x18, 0x18, 0x18, 0x3c, 0x00},   // U+00EE
    { 0xcc, 0x00, 0x70, 0x30, 0x30, 0x30, 0x78, 0x00},   // U+00EF
    { 0xd8, 0x70, 0xd8, 0x0c, 0x7c, 0xcc, 0x78, 0x00},   // U+00F0
    { 0x00, 0xf8, 0x00, 0xf8, 0xcc, 0xcc, 0xcc, 0x00},   // U+00F1
    { 0x00, 0xe0, 0x00, 0x78, 0xcc, 0xcc, 0x78, 0x00},   // U+00F2
    { 0x00, 0x1c, 0x00, 0x78, 0xcc, 0xcc, 0x78, 0x00},   // U+00F3
    { 0x78, 0xcc, 0x00, 0x78, 0xcc, 0xcc, 0x78, 0x00},   // U+00F4
    { 0x76, 0xdc, 0x00, 0x78, 0xcc, 0xcc, 0x78, 0x00},   // U+00F5
    { 0x00, 0xcc, 0x00, 0x78, 0xcc, 0xcc, 0x78, 0x00},   // U+00F6
    { 0x18, 0x18, 0x00, 0x7e, 0x00, 0x18, 0x18, 0x00},   // U+00F7
    { 0x00, 0x06, 0x3c, 0x6e, 0x7e, 0x76, 0x3c, 0x60},   // U+00F8
    { 0x00, 0xe0, 0x00, 0xcc, 0xcc, 0xcc, 0x7e, 0x00},   // U+00F9
    { 0x00, 0x1c, 0x00, 0xcc, 0xcc, 0xcc, 0x7e, 0x00},   // U+00FA
    { 0x78, 0xcc, 0x00, 0xcc, 0xcc, 0xcc, 0x7e, 0x00},   // U+00FB
    { 0x00, 0xcc, 0x00, 0xcc, 0xcc, 0xcc, 0x7e, 0x00},   // U+00FC
    { 0x00, 0x1c, 0x00, 0xcc, 0xcc, 0x7c, 0x0c, 0xf8},   // U+00FD
    { 0x00, 0x00, 0x60, 0x7c, 0x66, 0x7c, 0x60, 0x00},   // U+00FE
    { 0x00, 0xcc, 0x00, 0xcc, 0xcc, 0x7c, 0x0c, 0xf8},   // U+00FF
};


// Constant: font8x8_2500
// Contains an 8x8 font map for unicode points U+2500 - U+257F (box drawing)
const Byte font8x8_dingbat[160][8] = {
    { 0x00, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00},   // U+2500
    { 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00},   // U+2501
    { 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10},   // U+2502
    { 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18},   // U+2503
    { 0x00, 0x00, 0x00, 0x00, 0xdd, 0x00, 0x00, 0x00},   // U+2504
    { 0x00, 0x00, 0x00, 0xdd, 0xdd, 0x00, 0x00, 0x00},   // U+2505
    { 0x10, 0x00, 0x10, 0x10, 0x10, 0x00, 0x10, 0x10},   // U+2506
    { 0x18, 0x00, 0x18, 0x18, 0x18, 0x00, 0x18, 0x18},   // U+2507
    { 0x00, 0x00, 0x00, 0x00, 0xaa, 0x00, 0x00, 0x00},   // U+2508
    { 0x00, 0x00, 0x00, 0xaa, 0xaa, 0x00, 0x00, 0x00},   // U+2509
    { 0x00, 0x10, 0x00, 0x10, 0x00, 0x10, 0x00, 0x10},   // U+250A
    { 0x00, 0x18, 0x00, 0x18, 0x00, 0x18, 0x00, 0x18},   // U+250B
    { 0x00, 0x00, 0x00, 0x00, 0x1f, 0x10, 0x10, 0x10},   // U+250C
    { 0x00, 0x00, 0x00, 0x1f, 0x1f, 0x10, 0x10, 0x10},   // U+250D
    { 0x00, 0x00, 0x00, 0x00, 0x1f, 0x18, 0x18, 0x18},   // U+250E
    { 0x00, 0x00, 0x00, 0x1f, 0x1f, 0x18, 0x18, 0x18},   // U+250F
    { 0x00, 0x00, 0x00, 0x00, 0xf0, 0x10, 0x10, 0x10},   // U+2510
    { 0x00, 0x00, 0x00, 0xf0, 0xf0, 0x10, 0x10, 0x10},   // U+2511
    { 0x00, 0x00, 0x00, 0x00, 0xf8, 0x18, 0x18, 0x18},   // U+2512
    { 0x00, 0x00, 0x00, 0xf8, 0xf8, 0x18, 0x18, 0x18},   // U+2513
    { 0x10, 0x10, 0x10, 0x10, 0x1f, 0x00, 0x00, 0x00},   // U+2514
    { 0x10, 0x10, 0x10, 0x1f, 0x1f, 0x00, 0x00, 0x00},   // U+2515
    { 0x18, 0x18, 0x18, 0x18, 0x1f, 0x00, 0x00, 0x00},   // U+2516
    { 0x18, 0x18, 0x18, 0x1f, 0x1f, 0x00, 0x00, 0x00},   // U+2517
    { 0x10, 0x10, 0x10, 0x10, 0xf0, 0x00, 0x00, 0x00},   // U+2518
    { 0x10, 0x10, 0x10, 0xf0, 0xf0, 0x00, 0x00, 0x00},   // U+2519
    { 0x18, 0x18, 0x18, 0x18, 0xf8, 0x00, 0x00, 0x00},   // U+251A
    { 0x18, 0x18, 0x18, 0xf8, 0xf8, 0x00, 0x00, 0x00},   // U+251B
    { 0x10, 0x10, 0x10, 0x10, 0x1f, 0x10, 0x10, 0x10},   // U+251C
    { 0x10, 0x10, 0x10, 0x1f, 0x1f, 0x10, 0x10, 0x10},   // U+251D
    { 0x18, 0x18, 0x18, 0x18, 0x1f, 0x10, 0x10, 0x10},   // U+251E
    { 0x10, 0x10, 0x10, 0x10, 0x1f, 0x18, 0x18, 0x18},   // U+251F
    { 0x18, 0x18, 0x18, 0x18, 0x1f, 0x18, 0x18, 0x18},   // U+2520
    { 0x18, 0x18, 0x18, 0x1f, 0x1f, 0x10, 0x10, 0x10},   // U+2521
    { 0x10, 0x10, 0x10, 0x1f, 0x1f, 0x18, 0x18, 0x18},   // U+2522
    { 0x18, 0x18, 0x18, 0x1f, 0x1f, 0x18, 0x18, 0x18},   // U+2523
    { 0x10, 0x10, 0x10, 0x10, 0xf0, 0x10, 0x10, 0x10},   // U+2524
    { 0x10, 0x10, 0x10, 0xf0, 0xf0, 0x10, 0x10, 0x10},   // U+2525
    { 0x18, 0x18, 0x18, 0x18, 0xf8, 0x10, 0x10, 0x10},   // U+2526
    { 0x10, 0x10, 0x10, 0x10, 0xf8, 0x18, 0x18, 0x18},   // U+2527
    { 0x18, 0x18, 0x18, 0x18, 0xf8, 0x18, 0x18, 0x18},   // U+2528
    { 0x18, 0x18, 0x18, 0xf8, 0xf8, 0x10, 0x10, 0x10},   // U+2529
    { 0x10, 0x10, 0x10, 0xf8, 0xf8, 0x18, 0x18, 0x18},   // U+252A
    { 0x18, 0x18, 0x18, 0xf8, 0xf8, 0x18, 0x18, 0x18},   // U+252B
    { 0x00, 0x00, 0x00, 0x00, 0xff, 0x10, 0x10, 0x10},   // U+252C
    { 0x00, 0x00, 0x00, 0xf0, 0xff, 0x10, 0x10, 0x10},   // U+252D
    { 0x00, 0x00, 0x00, 0x1f, 0xff, 0x10, 0x10, 0x10},   // U+252E
    { 0x00, 0x00, 0x00, 0xff, 0xff, 0x10, 0x10, 0x10},   // U+252F
    { 0x00, 0x00, 0x00, 0x00, 0xff, 0x18, 0x18, 0x18},   // U+2530
    { 0x00, 0x00, 0x00, 0xf8, 0xff, 0x18, 0x18, 0x18},   // U+2531
    { 0x00, 0x00, 0x00, 0x1f, 0xff, 0x18, 0x18, 0x18},   // U+2532
    { 0x00, 0x00, 0x00, 0xff, 0xff, 0x18, 0x18, 0x18},   // U+2533
    { 0x10, 0x10, 0x10, 0x10, 0xff, 0x00, 0x00, 0x00},   // U+2534
    { 0x10, 0x10, 0x10, 0xf0, 0xff, 0x00, 0x00, 0x00},   // U+2535
    { 0x10, 0x10, 0x10, 0x1f, 0xff, 0x00, 0x00, 0x00},   // U+2536
    { 0x10, 0x10, 0x10, 0xff, 0xff, 0x00, 0x00, 0x00},   // U+2537
    { 0x18, 0x18, 0x18, 0x18, 0xff, 0x00, 0x00, 0x00},   // U+2538
    { 0x18, 0x18, 0x18, 0xf8, 0xff, 0x00, 0x00, 0x00},   // U+2539
    { 0x18, 0x18, 0x18, 0x1f, 0xff, 0x00, 0x00, 0x00},   // U+253A
    { 0x18, 0x18, 0x18, 0xff, 0xff, 0x00, 0x00, 0x00},   // U+253B
    { 0x10, 0x10, 0x10, 0x10, 0xff, 0x10, 0x10, 0x10},   // U+253C
    { 0x10, 0x10, 0x10, 0xf0, 0xff, 0x10, 0x10, 0x10},   // U+253D
    { 0x10, 0x10, 0x10, 0x1f, 0xff, 0x10, 0x10, 0x10},   // U+253E
    { 0x10, 0x10, 0x10, 0xff, 0xff, 0x10, 0x10, 0x10},   // U+253F
    { 0x18, 0x18, 0x18, 0x18, 0xff, 0x10, 0x10, 0x10},   // U+2540
    { 0x10, 0x10, 0x10, 0x10, 0xff, 0x18, 0x18, 0x18},   // U+2541
    { 0x18, 0x18, 0x18, 0x18, 0xff, 0x18, 0x18, 0x18},   // U+2542
    { 0x18, 0x18, 0x18, 0xf8, 0xff, 0x10, 0x10, 0x10},   // U+2543
    { 0x18, 0x18, 0x18, 0x1f, 0xff, 0x10, 0x10, 0x10},   // U+2544
    { 0x10, 0x10, 0x10, 0xf8, 0xff, 0x18, 0x18, 0x18},   // U+2545
    { 0x10, 0x10, 0x10, 0x1f, 0xff, 0x18, 0x18, 0x18},   // U+2546
    { 0x10, 0x10, 0x10, 0xff, 0xff, 0x18, 0x18, 0x18},   // U+2547
    { 0x18, 0x18, 0x18, 0xff, 0xff, 0x10, 0x10, 0x10},   // U+2548
    { 0x18, 0x18, 0x18, 0x1f, 0xff, 0x18, 0x18, 0x18},   // U+2549
    { 0x18, 0x18, 0x18, 0xf8, 0xff, 0x18, 0x18, 0x18},   // U+254A
    { 0x18, 0x18, 0x18, 0xff, 0xff, 0x18, 0x18, 0x18},   // U+254B
    { 0x00, 0x00, 0x00, 0x00, 0xe7, 0x00, 0x00, 0x00},   // U+254C
    { 0x00, 0x00, 0x00, 0xe7, 0xe7, 0x00, 0x00, 0x00},   // U+254D
    { 0x10, 0x10, 0x10, 0x00, 0x00, 0x10, 0x10, 0x10},   // U+254E
    { 0x18, 0x18, 0x18, 0x00, 0x00, 0x18, 0x18, 0x18},   // U+254F
    { 0x00, 0x00, 0x00, 0xff, 0x00, 0xff, 0x00, 0x00},   // U+2550
    { 0x28, 0x28, 0x28, 0x28, 0x28, 0x28, 0x28, 0x28},   // U+2551
    { 0x00, 0x00, 0x00, 0x1f, 0x10, 0x1f, 0x10, 0x10},   // U+2552
    { 0x00, 0x00, 0x00, 0x00, 0x3f, 0x28, 0x28, 0x28},   // U+2553
    { 0x00, 0x00, 0x00, 0x3f, 0x20, 0x2f, 0x28, 0x28},   // U+2554
    { 0x00, 0x00, 0x00, 0xf0, 0x10, 0xf0, 0x10, 0x10},   // U+2555
    { 0x00, 0x00, 0x00, 0x00, 0xf8, 0x28, 0x28, 0x28},   // U+2556
    { 0x00, 0x00, 0x00, 0xf8, 0x08, 0xe8, 0x28, 0x28},   // U+2557
    { 0x10, 0x10, 0x10, 0x1f, 0x10, 0x1f, 0x00, 0x00},   // U+2558
    { 0x28, 0x28, 0x28, 0x28, 0x3f, 0x00, 0x00, 0x00},   // U+2559
    { 0x28, 0x28, 0x28, 0x2f, 0x20, 0x3f, 0x00, 0x00},   // U+255A
    { 0x10, 0x10, 0x10, 0xf0, 0x10, 0xf0, 0x00, 0x00},   // U+255B
    { 0x28, 0x28, 0x28, 0x28, 0xf8, 0x00, 0x00, 0x00},   // U+255C
    { 0x28, 0x28, 0x28, 0xe8, 0x08, 0xf8, 0x00, 0x00},   // U+255D
    { 0x10, 0x10, 0x10, 0x1f, 0x10, 0x1f, 0x10, 0x10},   // U+255E
    { 0x28, 0x28, 0x28, 0x28, 0x2f, 0x28, 0x28, 0x28},   // U+255F
    { 0x28, 0x28, 0x28, 0x2f, 0x20, 0x2f, 0x28, 0x28},   // U+2560
    { 0x10, 0x10, 0x10, 0xf0, 0x10, 0xf0, 0x10, 0x10},   // U+2561
    { 0x28, 0x28, 0x28, 0x28, 0xe8, 0x28, 0x28, 0x28},   // U+2562
    { 0x28, 0x28, 0x28, 0xe8, 0x08, 0xe8, 0x28, 0x28},   // U+2563
    { 0x00, 0x00, 0x00, 0xff, 0x00, 0xff, 0x10, 0x10},   // U+2564
    { 0x00, 0x00, 0x00, 0x00, 0xff, 0x28, 0x28, 0x28},   // U+2565
    { 0x00, 0x00, 0x00, 0xff, 0x00, 0xef, 0x28, 0x28},   // U+2566
    { 0x10, 0x10, 0x10, 0xff, 0x00, 0xff, 0x00, 0x00},   // U+2567
    { 0x28, 0x28, 0x28, 0x28, 0xff, 0x00, 0x00, 0x00},   // U+2568
    { 0x28, 0x28, 0x28, 0xef, 0x00, 0xff, 0x00, 0x00},   // U+2569
    { 0x10, 0x10, 0x10, 0xff, 0x10, 0xff, 0x10, 0x10},   // U+256A
    { 0x28, 0x28, 0x28, 0x28, 0xff, 0x28, 0x28, 0x28},   // U+256B
    { 0x28, 0x28, 0x28, 0xef, 0x00, 0xef, 0x28, 0x28},   // U+256C
    { 0x00, 0x00, 0x00, 0x00, 0x07, 0x08, 0x10, 0x10},   // U+256D
    { 0x00, 0x00, 0x00, 0x00, 0xc0, 0x20, 0x10, 0x10},   // U+256E
    { 0x10, 0x10, 0x10, 0x20, 0xc0, 0x00, 0x00, 0x00},   // U+256F
    { 0x10, 0x10, 0x10, 0x08, 0x07, 0x00, 0x00, 0x00},   // U+2570
    { 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80},   // U+2571
    { 0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01},   // U+2572
    { 0x81, 0x42, 0x24, 0x18, 0x18, 0x24, 0x42, 0x81},   // U+2573
    { 0x00, 0x00, 0x00, 0x00, 0xf0, 0x00, 0x00, 0x00},   // U+2574
    { 0x10, 0x10, 0x10, 0x10, 0x00, 0x00, 0x00, 0x00},   // U+2575
    { 0x00, 0x00, 0x00, 0x00, 0x1f, 0x00, 0x00, 0x00},   // U+2576
    { 0x00, 0x00, 0x00, 0x00, 0x10, 0x10, 0x10, 0x10},   // U+2577
    { 0x00, 0x00, 0x00, 0xf0, 0xf0, 0x00, 0x00, 0x00},   // U+2578
    { 0x18, 0x18, 0x18, 0x18, 0x00, 0x00, 0x00, 0x00},   // U+2579
    { 0x00, 0x00, 0x00, 0x1f, 0x1f, 0x00, 0x00, 0x00},   // U+257A
    { 0x00, 0x00, 0x00, 0x00, 0x18, 0x18, 0x18, 0x18},   // U+257B
    { 0x00, 0x00, 0x00, 0x1f, 0xff, 0x00, 0x00, 0x00},   // U+257C
    { 0x10, 0x10, 0x10, 0x10, 0x18, 0x18, 0x18, 0x18},   // U+257D
    { 0x00, 0x00, 0x00, 0xf0, 0xff, 0x00, 0x00, 0x00},   // U+257E
    { 0x18, 0x18, 0x18, 0x18, 0x10, 0x10, 0x10, 0x10},   // U+257F
    
    // Constant: font8x8_2580
    // Contains an 8x8 font map for unicode points U+2580 - U+259F (block elements)
    { 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00},   // U+2580
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff},   // U+2581
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff},   // U+2582
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff},   // U+2583
    { 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff},   // U+2584
    { 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff},   // U+2585
    { 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},   // U+2586
    { 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},   // U+2587
    { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},   // U+2588
    { 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe},   // U+2589
    { 0xfc, 0xfc, 0xfc, 0xfc, 0xfc, 0xfc, 0xfc, 0xfc},   // U+258A
    { 0xf8, 0xf8, 0xf8, 0xf8, 0xf8, 0xf8, 0xf8, 0xf8},   // U+258B
    { 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0},   // U+258C
    { 0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0xe0, 0xe0},   // U+258D
    { 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0},   // U+258E
    { 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80},   // U+258F
    { 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f},   // U+2590
    { 0xaa, 0x00, 0x55, 0x00, 0xaa, 0x00, 0x55, 0x00},   // U+2591
    { 0xaa, 0x55, 0xaa, 0x55, 0xaa, 0x55, 0xaa, 0x55},   // U+2592
    { 0xff, 0x55, 0xff, 0xaa, 0xff, 0x55, 0xff, 0xaa},   // U+2593
    { 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},   // U+2594
    { 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01},   // U+2595
    { 0x00, 0x00, 0x00, 0x00, 0xf0, 0xf0, 0xf0, 0xf0},   // U+2596
    { 0x00, 0x00, 0x00, 0x00, 0x0f, 0x0f, 0x0f, 0x0f},   // U+2597
    { 0xf0, 0xf0, 0xf0, 0xf0, 0x00, 0x00, 0x00, 0x00},   // U+2598
    { 0xf0, 0xf0, 0xf0, 0xf0, 0xff, 0xff, 0xff, 0xff},   // U+2599
    { 0xf0, 0xf0, 0xf0, 0xf0, 0x0f, 0x0f, 0x0f, 0x0f},   // U+259A
    { 0xff, 0xff, 0xff, 0xff, 0xf0, 0xf0, 0xf0, 0xf0},   // U+259B
    { 0xff, 0xff, 0xff, 0xff, 0x0f, 0x0f, 0x0f, 0x0f},   // U+259C
    { 0x0f, 0x0f, 0x0f, 0x0f, 0x00, 0x00, 0x00, 0x00},   // U+259D
    { 0x0f, 0x0f, 0x0f, 0x0f, 0xf0, 0xf0, 0xf0, 0xf0},   // U+259E
    { 0x0f, 0x0f, 0x0f, 0x0f, 0xff, 0xff, 0xff, 0xff},   // U+259F
};