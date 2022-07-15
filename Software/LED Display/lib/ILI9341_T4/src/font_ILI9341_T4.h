/******************************************************************************
*  ILI9341_T4 library for driving an ILI9341 screen via SPI with a Teensy 4/4.1
*  Implements vsync and differential updates from a memory framebuffer.
*
*  Copyright (c) 2020 Arvind Singh.  All right reserved.
*
* This library is free software; you can redistribute it and/or
*  modify it under the terms of the GNU Lesser General Public
*  License as published by the Free Software Foundation; either
*  version 2.1 of the License, or (at your option) any later version.
*
* This library is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
*  Lesser General Public License for more details.
*
*  You should have received a copy of the GNU Lesser General Public
*  License along with this library; if not, write to the Free Software
*  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*******************************************************************************/

#ifndef _font_ILI9341_T4_H_
#define _font_ILI9341_T4_H_


typedef struct 
    {
    const unsigned char* index;
    const unsigned char* unicode;
    const unsigned char* data;
    unsigned char version;
    unsigned char reserved;
    unsigned char index1_first;
    unsigned char index1_last;
    unsigned char index2_first;
    unsigned char index2_last;
    unsigned char bits_index;
    unsigned char bits_width;
    unsigned char bits_height;
    unsigned char bits_xoffset;
    unsigned char bits_yoffset;
    unsigned char bits_delta;
    unsigned char line_space;
    unsigned char cap_height;
    } ILI9341_t3_font_t;



extern const ILI9341_t3_font_t font_ILI9341_T4_OpenSans_Bold_10;
extern const ILI9341_t3_font_t font_ILI9341_T4_OpenSans_Bold_12;
extern const ILI9341_t3_font_t font_ILI9341_T4_OpenSans_Bold_14;
extern const ILI9341_t3_font_t font_ILI9341_T4_OpenSans_Bold_16;


#endif

/** end of file */
