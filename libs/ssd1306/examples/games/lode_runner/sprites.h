/*
    MIT License

    Copyright (c) 2017-2018, Alexey Dynda

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.
*/

#pragma once

#include "ssd1306.h"

enum
{
    MAN_ANIM_FLYING   = 0,
    MAN_ANIM_UP       = 1,
    MAN_ANIM_DOWN     = 1,
    MAN_ANIM_LEFT     = 2,
    MAN_ANIM_RIGHT    = 3,
    MAN_ANIM_RIGHT_PIPE = 4,
    MAN_ANIM_LEFT_PIPE  = 5,

    MAN_ANIM_MAX      = 6,
};

extern PROGMEM const uint8_t bgSprites[5][8];

extern const PROGMEM uint8_t playerFlyingImage[MAN_ANIM_MAX][2][8];

extern const PROGMEM uint8_t coinImage[8];
