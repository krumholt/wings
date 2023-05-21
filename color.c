#ifndef _colors_c_
#define _colors_c_

#include "types.h"
#include "wings_math.c" 

const struct v4 colors[] =
{
    {0xFF / 255.0f, 0x00 / 255.0f, 0x00 / 255.0f, 1.0f},
    {0xFF / 255.0f, 0x7F / 255.0f, 0x00 / 255.0f, 1.0f},
    {0xFF / 255.0f, 0xFF / 255.0f, 0x00 / 255.0f, 1.0f},
    {0x7F / 255.0f, 0xFF / 255.0f, 0x00 / 255.0f, 1.0f},
    {0x00 / 255.0f, 0xFF / 255.0f, 0x00 / 255.0f, 1.0f},
    {0x00 / 255.0f, 0xFF / 255.0f, 0x7F / 255.0f, 1.0f},
    {0x00 / 255.0f, 0xFF / 255.0f, 0xFF / 255.0f, 1.0f},
    {0x00 / 255.0f, 0x7F / 255.0f, 0xFF / 255.0f, 1.0f},
    {0x00 / 255.0f, 0x00 / 255.0f, 0xFF / 255.0f, 1.0f},
    {0x7F / 255.0f, 0x00 / 255.0f, 0xFF / 255.0f, 1.0f},
    {0xFF / 255.0f, 0x00 / 255.0f, 0xFF / 255.0f, 1.0f},
    {0xFF / 255.0f, 0x00 / 255.0f, 0x7F / 255.0f, 1.0f},
    {0xB2 / 255.0f, 0x00 / 255.0f, 0xED / 255.0f, 1.0f},
    {0x7c / 255.0f, 0x47 / 255.0f, 0x00 / 255.0f, 1.0f},
    {0x82 / 255.0f, 0x82 / 255.0f, 0x82 / 255.0f, 1.0f},
    {0xf9 / 255.0f, 0xa6 / 255.0f, 0x02 / 255.0f, 1.0f},
    {0x0b / 255.0f, 0x66 / 255.0f, 0x23 / 255.0f, 1.0f},
    {0x00 / 255.0f, 0xa8 / 255.0f, 0x6b / 255.0f, 1.0f},
    {0x1c / 255.0f, 0x29 / 255.0f, 0x51 / 255.0f, 1.0f},
    {0xFC / 255.0f, 0x0F / 255.0f, 0xC0 / 255.0f, 1.0f},
    {0xfc / 255.0f, 0xd0 / 255.0f, 0xa1 / 255.0f, 1.0f},
    {0x6d / 255.0f, 0xd3 / 255.0f, 0xce / 255.0f, 1.0f},
    {0x39 / 255.0f, 0x39 / 255.0f, 0x3a / 255.0f, 1.0f},
    {0xda / 255.0f, 0xc4 / 255.0f, 0xf7 / 255.0f, 1.0f},
    {0xcc / 255.0f, 0x5a / 255.0f, 0x71 / 255.0f, 1.0f},
    {0x2f / 255.0f, 0x9d / 255.0f, 0x98 / 255.0f, 1.0f},
    {0xfa / 255.0f, 0xfa / 255.0f, 0xd2 / 255.0f, 1.0f},
    {0xFF / 255.0f, 0xFF / 255.0f, 0xFF / 255.0f, 1.0f},
    {0x00 / 255.0f, 0x00 / 255.0f, 0x00 / 255.0f, 1.0f},
    {0x7e / 255.0f, 0x19 / 255.0f, 0x1b / 255.0f, 1.0f},
    {0x5b / 255.0f, 0x23 / 255.0f, 0x33 / 255.0f, 1.0f},
    {0x12 / 255.0f, 0x35 / 255.0f, 0x24 / 255.0f, 1.0f},
    {0xDA / 255.0f, 0x18 / 255.0f, 0x84 / 255.0f, 1.0f},
    {0xB4 / 255.0f, 0x7E / 255.0f, 0xE5 / 255.0f, 1.0f},
    {0x7F / 255.0f, 0x47 / 255.0f, 0x37 / 255.0f, 1.0f},
    {0x8B / 255.0f, 0x45 / 255.0f, 0x13 / 255.0f, 1.0f},
    {0x90 / 255.0f, 0x6E / 255.0f, 0x3E / 255.0f, 1.0f},
    {0x2E / 255.0f, 0x15 / 255.0f, 0x03 / 255.0f, 1.0f},
    {0xED / 255.0f, 0xE8 / 255.0f, 0xBA / 255.0f, 1.0f},
    {0x48 / 255.0f, 0x49 / 255.0f, 0x4B / 255.0f, 1.0f},
    {0x6F / 255.0f, 0x90 / 255.0f, 0xF4 / 255.0f, 1.0f},
    {0xB9 / 255.0f, 0xBB / 255.0f, 0xB6 / 255.0f, 1.0f},
    {0xD3 / 255.0f, 0xD3 / 255.0f, 0xD3 / 255.0f, 1.0f},
};

#define color_red             colors[ 0]
#define color_orange          colors[ 1]
#define color_yellow          colors[ 2]
#define color_green_yellow    colors[ 3]
#define color_green           colors[ 4]
#define color_green_cyan      colors[ 5]
#define color_cyan            colors[ 6]
#define color_blue_cyan       colors[ 7]
#define color_blue            colors[ 8]
#define color_blue_magenta    colors[ 9]
#define color_magenta         colors[10]
#define color_red_magenta     colors[11]
#define color_violet          colors[12]
#define color_brown           colors[13]
#define color_grey            colors[14]
#define color_gold            colors[15]
#define color_forest          colors[16]
#define color_jade            colors[17]
#define color_space           colors[18]
#define color_pink            colors[19]
#define color_deep_champagne  colors[20]
#define color_medium_turquois colors[21]
#define color_onyx            colors[22]
#define color_lavender_blue   colors[23]
#define color_cinnamon_satin  colors[24]
#define color_viridian_green  colors[25]
#define color_light_goldenrod colors[26]
#define color_white           colors[27]
#define color_black           colors[28]
#define color_vermillion      colors[29]
#define color_old_mauve       colors[30]
#define color_dark_green      colors[31]
#define color_barbie_pink     colors[32]
#define color_floral          colors[33]
#define color_sienna          colors[34]
#define color_saddle          colors[35]
#define color_khaki           colors[36]
#define color_chocolate       colors[37]
#define color_sand_dollar     colors[38]
#define color_iron            colors[39]
#define color_cornflower_blue colors[40]
#define color_rhino           colors[41]
#define color_light_grey      colors[42]

u32 max_colors = array_length(colors);
#endif
