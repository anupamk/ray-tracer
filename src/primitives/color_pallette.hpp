#pragma once

/*
 * just a pallette of colors
 **/
#include "color.hpp"

#include <string>
#include <unordered_map>

namespace raytracer
{
        static std::unordered_map<std::string, color> const color_pallette = {
                {"black", color::RGB(0x0, 0x0, 0x0)},
                {"dim gray", color::RGB(0x69, 0x69, 0x69)},
                {"dark gray", color::RGB(0xa9, 0xa9, 0xa9)},
                {"gray", color::RGB(0xbe, 0xbe, 0xbe)},
                {"light gray", color::RGB(0xd3, 0xd3, 0xd3)},
                {"gainsboro", color::RGB(0xdc, 0xdc, 0xdc)},
                {"white smoke", color::RGB(0xf5, 0xf5, 0xf5)},
                {"white", color::RGB(0xff, 0xff, 0xff)},
                {"red", color::RGB(0xff, 0x00, 0x00)},
                {"orange red", color::RGB(0xff, 0x45, 0x00)},
                {"dark orange", color::RGB(0xff, 0x8c, 0x00)},
                {"orange", color::RGB(0xff, 0xa5, 0x00)},
                {"gold", color::RGB(0xff, 0xd7, 0x00)},
                {"yellow", color::RGB(0xff, 0xff, 0x00)},
                {"chartreuse", color::RGB(0x7f, 0xff, 0x00)},
                {"lawn green", color::RGB(0x7c, 0xfc, 0x00)},
                {"green", color::RGB(0x00, 0xff, 0x00)},
                {"spring green", color::RGB(0x00, 0xff, 0x7f)},
                {"medium spring green", color::RGB(0x00, 0xfa, 0x9a)},
                {"cyan", color::RGB(0x00, 0xff, 0xff)},
                {"deep sky blue", color::RGB(0x00, 0xbf, 0xff)},
                {"blue", color::RGB(0x00, 0x00, 0xff)},
                {"medium blue", color::RGB(0x00, 0x00, 0xcd)},
                {"dark violet", color::RGB(0x94, 0x00, 0xd3)},
                {"dark magenta", color::RGB(0x8b, 0x00, 0x8b)},
                {"magenta", color::RGB(0xff, 0x00, 0xff)},
                {"dark red", color::RGB(0x8b, 0x00, 0x00)},
                {"brown", color::RGB(0xa5, 0x2a, 0x2a)},
                {"firebrick", color::RGB(0xb2, 0x22, 0x22)},
                {"indian red", color::RGB(0xcd, 0x5c, 0x5c)},
                {"light coral", color::RGB(0xf0, 0x80, 0x80)},
                {"salmon", color::RGB(0xfa, 0x80, 0x72)},
                {"light salmon", color::RGB(0xff, 0xa0, 0x7a)},
                {"tomato", color::RGB(0xff, 0x63, 0x47)},
                {"coral", color::RGB(0xff, 0x7f, 0x50)},
                {"dark salmon", color::RGB(0xe9, 0x96, 0x7a)},
                {"rosy brown", color::RGB(0xbc, 0x8f, 0x8f)},
                {"sienna", color::RGB(0xa0, 0x52, 0x2d)},
                {"saddle brown", color::RGB(0x8b, 0x45, 0x13)},
                {"chocolate", color::RGB(0xd2, 0x69, 0x1e)},
                {"peru", color::RGB(0xcd, 0x85, 0x3f)},
                {"sandy brown", color::RGB(0xf4, 0xa4, 0x60)},
                {"burlywood", color::RGB(0xde, 0xb8, 0x87)},
                {"tan", color::RGB(0xd2, 0xb4, 0x8c)},
                {"navajo white", color::RGB(0xff, 0xde, 0xad)},
                {"wheat", color::RGB(0xf5, 0xde, 0xb3)},
                {"dark goldenrod", color::RGB(0xb8, 0x86, 0x0b)},
                {"goldenrod", color::RGB(0xda, 0xa5, 0x20)},
                {"light goldenrod", color::RGB(0xee, 0xdd, 0x82)},
                {"pale goldenrod", color::RGB(0xee, 0xe8, 0xaa)},
                {"cornsilk", color::RGB(0xff, 0xf8, 0xdc)},
                {"dark khaki", color::RGB(0xbd, 0xb7, 0x6b)},
                {"khaki", color::RGB(0xf0, 0xe6, 0x8c)},
                {"lemon chiffon", color::RGB(0xff, 0xfa, 0xcd)},
                {"dark olive green", color::RGB(0x55, 0x6b, 0x2f)},
                {"olive drab", color::RGB(0x6b, 0x8e, 0x23)},
                {"yellow green", color::RGB(0x9a, 0xcd, 0x32)},
                {"green yellow", color::RGB(0xad, 0xff, 0x2f)},
                {"light green", color::RGB(0x90, 0xee, 0x90)},
                {"forest green", color::RGB(0x22, 0x8b, 0x22)},
                {"dark green", color::RGB(0x00, 0x64, 0x00)},
                {"lime green", color::RGB(0x32, 0xcd, 0x32)},
                {"pale green", color::RGB(0x98, 0xfb, 0x98)},
                {"dark sea green", color::RGB(0x8f, 0xbc, 0x8f)},
                {"sea green", color::RGB(0x2e, 0x8b, 0x57)},
                {"medium sea green", color::RGB(0x3c, 0xb3, 0x71)},
                {"light sea green", color::RGB(0x20, 0xb2, 0xaa)},
                {"medium aquamarine", color::RGB(0x66, 0xcd, 0xaa)},
                {"aquamarine", color::RGB(0x7f, 0xff, 0xd4)},
                {"dark cyan", color::RGB(0x00, 0x8b, 0x8b)},
                {"dark turquoise", color::RGB(0x00, 0xce, 0xd1)},
                {"medium turquoise", color::RGB(0x48, 0xd1, 0xcc)},
                {"turquoise", color::RGB(0x40, 0xe0, 0xd0)},
                {"pale turquoise", color::RGB(0xaf, 0xee, 0xee)},
                {"powder blue", color::RGB(0xb0, 0xe0, 0xe6)},
                {"light blue", color::RGB(0xad, 0xd8, 0xe6)},
                {"sky blue", color::RGB(0x87, 0xce, 0xeb)},
                {"light sky blue", color::RGB(0x87, 0xce, 0xfa)},
                {"cadet blue", color::RGB(0x5f, 0x9e, 0xa0)},
                {"steel blue", color::RGB(0x46, 0x82, 0xb4)},
                {"dark slate gray", color::RGB(0x2f, 0x4f, 0x4f)},
                {"slate gray", color::RGB(0x70, 0x80, 0x90)},
                {"light slate gray", color::RGB(0x77, 0x88, 0x99)},
                {"royal blue", color::RGB(0x41, 0x69, 0xe1)},
                {"dodger blue", color::RGB(0x1e, 0x90, 0xff)},
                {"cornflower blue", color::RGB(0x64, 0x95, 0xed)},
                {"light steel blue", color::RGB(0xb0, 0xc4, 0xde)},
                {"dark blue", color::RGB(0x00, 0x00, 0x8b)},
                {"navy", color::RGB(0x00, 0x00, 0x80)},
                {"midnight blue", color::RGB(0x19, 0x19, 0x70)},
                {"dark slate blue", color::RGB(0x48, 0x3d, 0x8b)},
                {"slate blue", color::RGB(0x6a, 0x5a, 0xcd)},
                {"medium slate blue", color::RGB(0x7b, 0x68, 0xee)},
                {"light slate blue", color::RGB(0x84, 0x70, 0xff)},
                {"medium purple", color::RGB(0x93, 0x70, 0xdb)},
                {"blue violet", color::RGB(0x8a, 0x2b, 0xe2)},
                {"purple", color::RGB(0xa0, 0x20, 0xf0)},
                {"dark orchid", color::RGB(0x99, 0x32, 0xcc)},
                {"medium orchid", color::RGB(0xba, 0x55, 0xd3)},
                {"orchid", color::RGB(0xda, 0x70, 0xd6)},
                {"thistle", color::RGB(0xd8, 0xbf, 0xd8)},
                {"plum", color::RGB(0xdd, 0xa0, 0xdd)},
                {"violet", color::RGB(0xee, 0x82, 0xee)},
                {"medium violet red", color::RGB(0xc7, 0x15, 0x85)},
                {"violet red", color::RGB(0xd0, 0x20, 0x90)},
                {"pale violet red", color::RGB(0xdb, 0x70, 0x93)},
                {"maroon", color::RGB(0xb0, 0x30, 0x60)},
                {"deep pink", color::RGB(0xff, 0x14, 0x93)},
                {"hot pink", color::RGB(0xff, 0x69, 0xb4)},
                {"pink", color::RGB(0xff, 0xc0, 0xcb)},
                {"light pink", color::RGB(0xff, 0xb6, 0xc1)},
                {"snow", color::RGB(0xff, 0xfa, 0xfa)},
                {"misty rose", color::RGB(0xff, 0xe4, 0xe1)},
                {"seashell", color::RGB(0xff, 0xf5, 0xee)},
                {"peach puff", color::RGB(0xff, 0xda, 0xb9)},
                {"linen", color::RGB(0xfa, 0xf0, 0xe6)},
                {"antique white", color::RGB(0xfa, 0xeb, 0xd7)},
                {"bisque", color::RGB(0xff, 0xe4, 0xc4)},
                {"papaya whip", color::RGB(0xff, 0xef, 0xd5)},
                {"moccasin", color::RGB(0xff, 0xe4, 0xb5)},
                {"blanched almond", color::RGB(0xff, 0xeb, 0xcd)},
                {"old lace", color::RGB(0xfd, 0xf5, 0xe6)},
                {"floral white", color::RGB(0xff, 0xfa, 0xf0)},
                {"beige", color::RGB(0xf5, 0xf5, 0xdc)},
                {"light yellow", color::RGB(0xff, 0xff, 0xe0)},
                {"light goldenrod yellow", color::RGB(0xfa, 0xfa, 0xd2)},
                {"ivory", color::RGB(0xff, 0xff, 0xf0)},
                {"honeydew", color::RGB(0xf0, 0xff, 0xf0)},
                {"mint cream", color::RGB(0xf5, 0xff, 0xfa)},
                {"light cyan", color::RGB(0xe0, 0xff, 0xff)},
                {"azure", color::RGB(0xf0, 0xff, 0xff)},
                {"alice blue", color::RGB(0xf0, 0xf8, 0xff)},
                {"lavender", color::RGB(0xe6, 0xe6, 0xfa)},
                {"ghost white", color::RGB(0xf8, 0xf8, 0xff)},
                {"lavender blush", color::RGB(0xff, 0xf0, 0xf5)},
                {"red4", color::RGB(0x8b, 0x00, 0x00)},
                {"red3", color::RGB(0xcd, 0x00, 0x00)},
                {"red2", color::RGB(0xee, 0x00, 0x00)},
                {"red1", color::RGB(0xff, 0x00, 0x00)},
                {"OrangeRed4", color::RGB(0x8b, 0x25, 0x00)},
                {"OrangeRed3", color::RGB(0xcd, 0x37, 0x00)},
                {"OrangeRed2", color::RGB(0xee, 0x40, 0x00)},
                {"OrangeRed1", color::RGB(0xff, 0x45, 0x00)},
                {"DarkOrange4", color::RGB(0x8b, 0x45, 0x00)},
                {"DarkOrange3", color::RGB(0xcd, 0x66, 0x00)},
                {"DarkOrange2", color::RGB(0xee, 0x76, 0x00)},
                {"DarkOrange1", color::RGB(0xff, 0x7f, 0x00)},
                {"orange4", color::RGB(0x8b, 0x5a, 0x00)},
                {"orange3", color::RGB(0xcd, 0x85, 0x00)},
                {"orange2", color::RGB(0xee, 0x9a, 0x00)},
                {"orange1", color::RGB(0xff, 0xa5, 0x00)},
                {"gold4", color::RGB(0x8b, 0x75, 0x00)},
                {"gold3", color::RGB(0xcd, 0xad, 0x00)},
                {"gold2", color::RGB(0xee, 0xc9, 0x00)},
                {"gold1", color::RGB(0xff, 0xd7, 0x00)},
                {"yellow4", color::RGB(0x8b, 0x8b, 0x00)},
                {"yellow3", color::RGB(0xcd, 0xcd, 0x00)},
                {"yellow2", color::RGB(0xee, 0xee, 0x00)},
                {"yellow1", color::RGB(0xff, 0xff, 0x00)},
                {"chartreuse4", color::RGB(0x45, 0x8b, 0x00)},
                {"chartreuse3", color::RGB(0x66, 0xcd, 0x00)},
                {"chartreuse2", color::RGB(0x76, 0xee, 0x00)},
                {"chartreuse1", color::RGB(0x7f, 0xff, 0x00)},
                {"green4", color::RGB(0x00, 0x8b, 0x00)},
                {"green3", color::RGB(0x00, 0xcd, 0x00)},
                {"green2", color::RGB(0x00, 0xee, 0x00)},
                {"green1", color::RGB(0x00, 0xff, 0x00)},
                {"SpringGreen4", color::RGB(0x00, 0x8b, 0x45)},
                {"SpringGreen3", color::RGB(0x00, 0xcd, 0x66)},
                {"SpringGreen2", color::RGB(0x00, 0xee, 0x76)},
                {"SpringGreen1", color::RGB(0x00, 0xff, 0x7f)},
                {"cyan4", color::RGB(0x00, 0x8b, 0x8b)},
                {"cyan3", color::RGB(0x00, 0xcd, 0xcd)},
                {"cyan2", color::RGB(0x00, 0xee, 0xee)},
                {"cyan1", color::RGB(0x00, 0xff, 0xff)},
                {"turquoise4", color::RGB(0x00, 0x86, 0x8b)},
                {"turquoise3", color::RGB(0x00, 0xc5, 0xcd)},
                {"turquoise2", color::RGB(0x00, 0xe5, 0xee)},
                {"turquoise1", color::RGB(0x00, 0xf5, 0xff)},
                {"DeepSkyBlue4", color::RGB(0x00, 0x68, 0x8b)},
                {"DeepSkyBlue3", color::RGB(0x00, 0x9a, 0xcd)},
                {"DeepSkyBlue2", color::RGB(0x00, 0xb2, 0xee)},
                {"DeepSkyBlue1", color::RGB(0x00, 0xbf, 0xff)},
                {"blue4", color::RGB(0x00, 0x00, 0x8b)},
                {"blue3", color::RGB(0x00, 0x00, 0xcd)},
                {"blue2", color::RGB(0x00, 0x00, 0xee)},
                {"blue1", color::RGB(0x00, 0x00, 0xff)},
                {"magenta4", color::RGB(0x8b, 0x00, 0x8b)},
                {"magenta3", color::RGB(0xcd, 0x00, 0xcd)},
                {"magenta2", color::RGB(0xee, 0x00, 0xee)},
                {"magenta1", color::RGB(0xff, 0x00, 0xff)},
                {"brown4", color::RGB(0x8b, 0x23, 0x23)},
                {"brown3", color::RGB(0xcd, 0x33, 0x33)},
                {"brown2", color::RGB(0xee, 0x3b, 0x3b)},
                {"brown1", color::RGB(0xff, 0x40, 0x40)},
                {"firebrick4", color::RGB(0x8b, 0x1a, 0x1a)},
                {"firebrick3", color::RGB(0xcd, 0x26, 0x26)},
                {"firebrick2", color::RGB(0xee, 0x2c, 0x2c)},
                {"firebrick1", color::RGB(0xff, 0x30, 0x30)},
                {"IndianRed4", color::RGB(0x8b, 0x3a, 0x3a)},
                {"IndianRed3", color::RGB(0xcd, 0x55, 0x55)},
                {"IndianRed2", color::RGB(0xee, 0x63, 0x63)},
                {"IndianRed1", color::RGB(0xff, 0x6a, 0x6a)},
                {"RosyBrown4", color::RGB(0x8b, 0x69, 0x69)},
                {"RosyBrown3", color::RGB(0xcd, 0x9b, 0x9b)},
                {"RosyBrown2", color::RGB(0xee, 0xb4, 0xb4)},
                {"RosyBrown1", color::RGB(0xff, 0xc1, 0xc1)},
                {"snow4", color::RGB(0x8b, 0x89, 0x89)},
                {"snow3", color::RGB(0xcd, 0xc9, 0xc9)},
                {"snow2", color::RGB(0xee, 0xe9, 0xe9)},
                {"snow1", color::RGB(0xff, 0xfa, 0xfa)},
                {"MistyRose4", color::RGB(0x8b, 0x7d, 0x7b)},
                {"MistyRose3", color::RGB(0xcd, 0xb7, 0xb5)},
                {"MistyRose2", color::RGB(0xee, 0xd5, 0xd2)},
                {"MistyRose1", color::RGB(0xff, 0xe4, 0xe1)},
                {"tomato4", color::RGB(0x8b, 0x36, 0x26)},
                {"tomato3", color::RGB(0xcd, 0x4f, 0x39)},
                {"tomato2", color::RGB(0xee, 0x5c, 0x42)},
                {"tomato1", color::RGB(0xff, 0x63, 0x47)},
                {"coral4", color::RGB(0x8b, 0x3e, 0x2f)},
                {"coral3", color::RGB(0xcd, 0x5b, 0x45)},
                {"coral2", color::RGB(0xee, 0x6a, 0x50)},
                {"coral1", color::RGB(0xff, 0x72, 0x56)},
                {"salmon4", color::RGB(0x8b, 0x4c, 0x39)},
                {"salmon3", color::RGB(0xcd, 0x70, 0x54)},
                {"salmon2", color::RGB(0xee, 0x82, 0x62)},
                {"salmon1", color::RGB(0xff, 0x8c, 0x69)},
                {"LightSalmon4", color::RGB(0x8b, 0x57, 0x42)},
                {"LightSalmon3", color::RGB(0xcd, 0x81, 0x62)},
                {"LightSalmon2", color::RGB(0xee, 0x95, 0x72)},
                {"LightSalmon1", color::RGB(0xff, 0xa0, 0x7a)},
                {"sienna4", color::RGB(0x8b, 0x47, 0x26)},
                {"sienna3", color::RGB(0xcd, 0x68, 0x39)},
                {"sienna2", color::RGB(0xee, 0x79, 0x42)},
                {"sienna1", color::RGB(0xff, 0x82, 0x47)},
                {"chocolate4", color::RGB(0x8b, 0x45, 0x13)},
                {"chocolate3", color::RGB(0xcd, 0x66, 0x1d)},
                {"chocolate2", color::RGB(0xee, 0x76, 0x21)},
                {"chocolate1", color::RGB(0xff, 0x7f, 0x24)},
                {"seashell4", color::RGB(0x8b, 0x86, 0x82)},
                {"seashell3", color::RGB(0xcd, 0xc5, 0xbf)},
                {"seashell2", color::RGB(0xee, 0xe5, 0xde)},
                {"seashell1", color::RGB(0xff, 0xf5, 0xee)},
                {"PeachPuff4", color::RGB(0x8b, 0x77, 0x65)},
                {"PeachPuff3", color::RGB(0xcd, 0xaf, 0x95)},
                {"PeachPuff2", color::RGB(0xee, 0xcb, 0xad)},
                {"PeachPuff1", color::RGB(0xff, 0xda, 0xb9)},
                {"tan4", color::RGB(0x8b, 0x5a, 0x2b)},
                {"tan3", color::RGB(0xcd, 0x85, 0x3f)},
                {"tan2", color::RGB(0xee, 0x9a, 0x49)},
                {"tan1", color::RGB(0xff, 0xa5, 0x4f)},
                {"bisque4", color::RGB(0x8b, 0x7d, 0x6b)},
                {"bisque3", color::RGB(0xcd, 0xb7, 0x9e)},
                {"bisque2", color::RGB(0xee, 0xd5, 0xb7)},
                {"bisque1", color::RGB(0xff, 0xe4, 0xc4)},
                {"AntiqueWhite4", color::RGB(0x8b, 0x83, 0x78)},
                {"AntiqueWhite3", color::RGB(0xcd, 0xc0, 0xb0)},
                {"AntiqueWhite2", color::RGB(0xee, 0xdf, 0xcc)},
                {"AntiqueWhite1", color::RGB(0xff, 0xef, 0xdb)},
                {"burlywood4", color::RGB(0x8b, 0x73, 0x55)},
                {"burlywood3", color::RGB(0xcd, 0xaa, 0x7d)},
                {"burlywood2", color::RGB(0xee, 0xc5, 0x91)},
                {"burlywood1", color::RGB(0xff, 0xd3, 0x9b)},
                {"NavajoWhite4", color::RGB(0x8b, 0x79, 0x5e)},
                {"NavajoWhite3", color::RGB(0xcd, 0xb3, 0x8b)},
                {"NavajoWhite2", color::RGB(0xee, 0xcf, 0xa1)},
                {"NavajoWhite1", color::RGB(0xff, 0xde, 0xad)},
                {"wheat4", color::RGB(0x8b, 0x7e, 0x66)},
                {"wheat3", color::RGB(0xcd, 0xba, 0x96)},
                {"wheat2", color::RGB(0xee, 0xd8, 0xae)},
                {"wheat1", color::RGB(0xff, 0xe7, 0xba)},
                {"DarkGoldenrod4", color::RGB(0x8b, 0x65, 0x08)},
                {"DarkGoldenrod3", color::RGB(0xcd, 0x95, 0x0c)},
                {"DarkGoldenrod2", color::RGB(0xee, 0xad, 0x0e)},
                {"DarkGoldenrod1", color::RGB(0xff, 0xb9, 0x0f)},
                {"goldenrod4", color::RGB(0x8b, 0x69, 0x14)},
                {"goldenrod3", color::RGB(0xcd, 0x9b, 0x1d)},
                {"goldenrod2", color::RGB(0xee, 0xb4, 0x22)},
                {"goldenrod1", color::RGB(0xff, 0xc1, 0x25)},
                {"cornsilk4", color::RGB(0x8b, 0x88, 0x78)},
                {"cornsilk3", color::RGB(0xcd, 0xc8, 0xb1)},
                {"cornsilk2", color::RGB(0xee, 0xe8, 0xcd)},
                {"cornsilk1", color::RGB(0xff, 0xf8, 0xdc)},
                {"LightGoldenrod4", color::RGB(0x8b, 0x81, 0x4c)},
                {"LightGoldenrod3", color::RGB(0xcd, 0xbe, 0x70)},
                {"LightGoldenrod2", color::RGB(0xee, 0xdc, 0x82)},
                {"LightGoldenrod1", color::RGB(0xff, 0xec, 0x8b)},
                {"LemonChiffon4", color::RGB(0x8b, 0x89, 0x70)},
                {"LemonChiffon3", color::RGB(0xcd, 0xc9, 0xa5)},
                {"LemonChiffon2", color::RGB(0xee, 0xe9, 0xbf)},
                {"LemonChiffon1", color::RGB(0xff, 0xfa, 0xcd)},
                {"khaki4", color::RGB(0x8b, 0x86, 0x4e)},
                {"khaki3", color::RGB(0xcd, 0xc6, 0x73)},
                {"khaki2", color::RGB(0xee, 0xe6, 0x85)},
                {"khaki1", color::RGB(0xff, 0xf6, 0x8f)},
                {"LightYellow4", color::RGB(0x8b, 0x8b, 0x7a)},
                {"LightYellow3", color::RGB(0xcd, 0xcd, 0xb4)},
                {"LightYellow2", color::RGB(0xee, 0xee, 0xd1)},
                {"LightYellow1", color::RGB(0xff, 0xff, 0xe0)},
                {"ivory4", color::RGB(0x8b, 0x8b, 0x83)},
                {"ivory3", color::RGB(0xcd, 0xcd, 0xc1)},
                {"ivory2", color::RGB(0xee, 0xee, 0xe0)},
                {"ivory1", color::RGB(0xff, 0xff, 0xf0)},
                {"OliveDrab4", color::RGB(0x69, 0x8b, 0x22)},
                {"OliveDrab3", color::RGB(0x9a, 0xcd, 0x32)},
                {"OliveDrab2", color::RGB(0xb3, 0xee, 0x3a)},
                {"OliveDrab1", color::RGB(0xc0, 0xff, 0x3e)},
                {"DarkOliveGreen4", color::RGB(0x6e, 0x8b, 0x3d)},
                {"DarkOliveGreen3", color::RGB(0xa2, 0xcd, 0x5a)},
                {"DarkOliveGreen2", color::RGB(0xbc, 0xee, 0x68)},
                {"DarkOliveGreen1", color::RGB(0xca, 0xff, 0x70)},
                {"PaleGreen4", color::RGB(0x54, 0x8b, 0x54)},
                {"PaleGreen3", color::RGB(0x7c, 0xcd, 0x7c)},
                {"PaleGreen2", color::RGB(0x90, 0xee, 0x90)},
                {"PaleGreen1", color::RGB(0x9a, 0xff, 0x9a)},
                {"DarkSeaGreen4", color::RGB(0x69, 0x8b, 0x69)},
                {"DarkSeaGreen3", color::RGB(0x9b, 0xcd, 0x9b)},
                {"DarkSeaGreen2", color::RGB(0xb4, 0xee, 0xb4)},
                {"DarkSeaGreen1", color::RGB(0xc1, 0xff, 0xc1)},
                {"honeydew4", color::RGB(0x83, 0x8b, 0x83)},
                {"honeydew3", color::RGB(0xc1, 0xcd, 0xc1)},
                {"honeydew2", color::RGB(0xe0, 0xee, 0xe0)},
                {"honeydew1", color::RGB(0xf0, 0xff, 0xf0)},
                {"SeaGreen4", color::RGB(0x2e, 0x8b, 0x57)},
                {"SeaGreen3", color::RGB(0x43, 0xcd, 0x80)},
                {"SeaGreen2", color::RGB(0x4e, 0xee, 0x94)},
                {"SeaGreen1", color::RGB(0x54, 0xff, 0x9f)},
                {"aquamarine4", color::RGB(0x45, 0x8b, 0x74)},
                {"aquamarine3", color::RGB(0x66, 0xcd, 0xaa)},
                {"aquamarine2", color::RGB(0x76, 0xee, 0xc6)},
                {"aquamarine1", color::RGB(0x7f, 0xff, 0xd4)},
                {"DarkSlateGray4", color::RGB(0x52, 0x8b, 0x8b)},
                {"DarkSlateGray3", color::RGB(0x79, 0xcd, 0xcd)},
                {"DarkSlateGray2", color::RGB(0x8d, 0xee, 0xee)},
                {"DarkSlateGray1", color::RGB(0x97, 0xff, 0xff)},
                {"PaleTurquoise4", color::RGB(0x66, 0x8b, 0x8b)},
                {"PaleTurquoise3", color::RGB(0x96, 0xcd, 0xcd)},
                {"PaleTurquoise2", color::RGB(0xae, 0xee, 0xee)},
                {"PaleTurquoise1", color::RGB(0xbb, 0xff, 0xff)},
                {"LightCyan4", color::RGB(0x7a, 0x8b, 0x8b)},
                {"LightCyan3", color::RGB(0xb4, 0xcd, 0xcd)},
                {"LightCyan2", color::RGB(0xd1, 0xee, 0xee)},
                {"LightCyan1", color::RGB(0xe0, 0xff, 0xff)},
                {"azure4", color::RGB(0x83, 0x8b, 0x8b)},
                {"azure3", color::RGB(0xc1, 0xcd, 0xcd)},
                {"azure2", color::RGB(0xe0, 0xee, 0xee)},
                {"azure1", color::RGB(0xf0, 0xff, 0xff)},
                {"CadetBlue4", color::RGB(0x53, 0x86, 0x8b)},
                {"CadetBlue3", color::RGB(0x7a, 0xc5, 0xcd)},
                {"CadetBlue2", color::RGB(0x8e, 0xe5, 0xee)},
                {"CadetBlue1", color::RGB(0x98, 0xf5, 0xff)},
                {"LightBlue4", color::RGB(0x68, 0x83, 0x8b)},
                {"LightBlue3", color::RGB(0x9a, 0xc0, 0xcd)},
                {"LightBlue2", color::RGB(0xb2, 0xdf, 0xee)},
                {"LightBlue1", color::RGB(0xbf, 0xef, 0xff)},
                {"LightSkyBlue4", color::RGB(0x60, 0x7b, 0x8b)},
                {"LightSkyBlue3", color::RGB(0x8d, 0xb6, 0xcd)},
                {"LightSkyBlue2", color::RGB(0xa4, 0xd3, 0xee)},
                {"LightSkyBlue1", color::RGB(0xb0, 0xe2, 0xff)},
                {"SkyBlue4", color::RGB(0x4a, 0x70, 0x8b)},
                {"SkyBlue3", color::RGB(0x6c, 0xa6, 0xcd)},
                {"SkyBlue2", color::RGB(0x7e, 0xc0, 0xee)},
                {"SkyBlue1", color::RGB(0x87, 0xce, 0xff)},
                {"SteelBlue4", color::RGB(0x36, 0x64, 0x8b)},
                {"SteelBlue3", color::RGB(0x4f, 0x94, 0xcd)},
                {"SteelBlue2", color::RGB(0x5c, 0xac, 0xee)},
                {"SteelBlue1", color::RGB(0x63, 0xb8, 0xff)},
                {"DodgerBlue4", color::RGB(0x10, 0x4e, 0x8b)},
                {"DodgerBlue3", color::RGB(0x18, 0x74, 0xcd)},
                {"DodgerBlue2", color::RGB(0x1c, 0x86, 0xee)},
                {"DodgerBlue1", color::RGB(0x1e, 0x90, 0xff)},
                {"SlateGray4", color::RGB(0x6c, 0x7b, 0x8b)},
                {"SlateGray3", color::RGB(0x9f, 0xb6, 0xcd)},
                {"SlateGray2", color::RGB(0xb9, 0xd3, 0xee)},
                {"SlateGray1", color::RGB(0xc6, 0xe2, 0xff)},
                {"LightSteelBlue4", color::RGB(0x6e, 0x7b, 0x8b)},
                {"LightSteelBlue3", color::RGB(0xa2, 0xb5, 0xcd)},
                {"LightSteelBlue2", color::RGB(0xbc, 0xd2, 0xee)},
                {"LightSteelBlue1", color::RGB(0xca, 0xe1, 0xff)},
                {"RoyalBlue4", color::RGB(0x27, 0x40, 0x8b)},
                {"RoyalBlue3", color::RGB(0x3a, 0x5f, 0xcd)},
                {"RoyalBlue2", color::RGB(0x43, 0x6e, 0xee)},
                {"RoyalBlue1", color::RGB(0x48, 0x76, 0xff)},
                {"SlateBlue4", color::RGB(0x47, 0x3c, 0x8b)},
                {"SlateBlue3", color::RGB(0x69, 0x59, 0xcd)},
                {"SlateBlue2", color::RGB(0x7a, 0x67, 0xee)},
                {"SlateBlue1", color::RGB(0x83, 0x6f, 0xff)},
                {"MediumPurple4", color::RGB(0x5d, 0x47, 0x8b)},
                {"MediumPurple3", color::RGB(0x89, 0x68, 0xcd)},
                {"MediumPurple2", color::RGB(0x9f, 0x79, 0xee)},
                {"MediumPurple1", color::RGB(0xab, 0x82, 0xff)},
                {"purple4", color::RGB(0x55, 0x1a, 0x8b)},
                {"purple3", color::RGB(0x7d, 0x26, 0xcd)},
                {"purple2", color::RGB(0x91, 0x2c, 0xee)},
                {"purple1", color::RGB(0x9b, 0x30, 0xff)},
                {"DarkOrchid4", color::RGB(0x68, 0x22, 0x8b)},
                {"DarkOrchid3", color::RGB(0x9a, 0x32, 0xcd)},
                {"DarkOrchid2", color::RGB(0xb2, 0x3a, 0xee)},
                {"DarkOrchid1", color::RGB(0xbf, 0x3e, 0xff)},
                {"MediumOrchid4", color::RGB(0x7a, 0x37, 0x8b)},
                {"MediumOrchid3", color::RGB(0xb4, 0x52, 0xcd)},
                {"MediumOrchid2", color::RGB(0xd1, 0x5f, 0xee)},
                {"MediumOrchid1", color::RGB(0xe0, 0x66, 0xff)},
                {"thistle4", color::RGB(0x8b, 0x7b, 0x8b)},
                {"thistle3", color::RGB(0xcd, 0xb5, 0xcd)},
                {"thistle2", color::RGB(0xee, 0xd2, 0xee)},
                {"thistle1", color::RGB(0xff, 0xe1, 0xff)},
                {"plum4", color::RGB(0x8b, 0x66, 0x8b)},
                {"plum3", color::RGB(0xcd, 0x96, 0xcd)},
                {"plum2", color::RGB(0xee, 0xae, 0xee)},
                {"plum1", color::RGB(0xff, 0xbb, 0xff)},
                {"orchid4", color::RGB(0x8b, 0x47, 0x89)},
                {"orchid3", color::RGB(0xcd, 0x69, 0xc9)},
                {"orchid2", color::RGB(0xee, 0x7a, 0xe9)},
                {"orchid1", color::RGB(0xff, 0x83, 0xfa)},
                {"maroon4", color::RGB(0x8b, 0x1c, 0x62)},
                {"maroon3", color::RGB(0xcd, 0x29, 0x90)},
                {"maroon2", color::RGB(0xee, 0x30, 0xa7)},
                {"maroon1", color::RGB(0xff, 0x34, 0xb3)},
                {"DeepPink4", color::RGB(0x8b, 0x0a, 0x50)},
                {"DeepPink3", color::RGB(0xcd, 0x10, 0x76)},
                {"DeepPink2", color::RGB(0xee, 0x12, 0x89)},
                {"DeepPink1", color::RGB(0xff, 0x14, 0x93)},
                {"HotPink4", color::RGB(0x8b, 0x3a, 0x62)},
                {"HotPink3", color::RGB(0xcd, 0x60, 0x90)},
                {"HotPink2", color::RGB(0xee, 0x6a, 0xa7)},
                {"HotPink1", color::RGB(0xff, 0x6e, 0xb4)},
                {"VioletRed4", color::RGB(0x8b, 0x22, 0x52)},
                {"VioletRed3", color::RGB(0xcd, 0x32, 0x78)},
                {"VioletRed2", color::RGB(0xee, 0x3a, 0x8c)},
                {"VioletRed1", color::RGB(0xff, 0x3e, 0x96)},
                {"LavenderBlush4", color::RGB(0x8b, 0x83, 0x86)},
                {"LavenderBlush3", color::RGB(0xcd, 0xc1, 0xc5)},
                {"LavenderBlush2", color::RGB(0xee, 0xe0, 0xe5)},
                {"LavenderBlush1", color::RGB(0xff, 0xf0, 0xf5)},
                {"PaleVioletRed4", color::RGB(0x8b, 0x47, 0x5d)},
                {"PaleVioletRed3", color::RGB(0xcd, 0x68, 0x89)},
                {"PaleVioletRed2", color::RGB(0xee, 0x79, 0x9f)},
                {"PaleVioletRed1", color::RGB(0xff, 0x82, 0xab)},
                {"pink4", color::RGB(0x8b, 0x63, 0x6c)},
                {"pink3", color::RGB(0xcd, 0x91, 0x9e)},
                {"pink2", color::RGB(0xee, 0xa9, 0xb8)},
                {"pink1", color::RGB(0xff, 0xb5, 0xc5)},
                {"LightPink4", color::RGB(0x8b, 0x5f, 0x65)},
                {"LightPink3", color::RGB(0xcd, 0x8c, 0x95)},
                {"LightPink2", color::RGB(0xee, 0xa2, 0xad)},
                {"LightPink1", color::RGB(0xff, 0xae, 0xb9)},
                {"gray0", color::RGB(0x00, 0x00, 0x00)},
                {"gray1", color::RGB(0x03, 0x03, 0x03)},
                {"gray2", color::RGB(0x05, 0x05, 0x05)},
                {"gray3", color::RGB(0x08, 0x08, 0x08)},
                {"gray4", color::RGB(0x0a, 0x0a, 0x0a)},
                {"gray5", color::RGB(0x0d, 0x0d, 0x0d)},
                {"gray6", color::RGB(0x0f, 0x0f, 0x0f)},
                {"gray7", color::RGB(0x12, 0x12, 0x12)},
                {"gray8", color::RGB(0x14, 0x14, 0x14)},
                {"gray9", color::RGB(0x17, 0x17, 0x17)},
                {"gray10", color::RGB(0x1a, 0x1a, 0x1a)},
                {"gray11", color::RGB(0x1c, 0x1c, 0x1c)},
                {"gray12", color::RGB(0x1f, 0x1f, 0x1f)},
                {"gray13", color::RGB(0x21, 0x21, 0x21)},
                {"gray14", color::RGB(0x24, 0x24, 0x24)},
                {"gray15", color::RGB(0x26, 0x26, 0x26)},
                {"gray16", color::RGB(0x29, 0x29, 0x29)},
                {"gray17", color::RGB(0x2b, 0x2b, 0x2b)},
                {"gray18", color::RGB(0x2e, 0x2e, 0x2e)},
                {"gray19", color::RGB(0x30, 0x30, 0x30)},
                {"gray20", color::RGB(0x33, 0x33, 0x33)},
                {"gray21", color::RGB(0x36, 0x36, 0x36)},
                {"gray22", color::RGB(0x38, 0x38, 0x38)},
                {"gray23", color::RGB(0x3b, 0x3b, 0x3b)},
                {"gray24", color::RGB(0x3d, 0x3d, 0x3d)},
                {"gray25", color::RGB(0x40, 0x40, 0x40)},
                {"gray26", color::RGB(0x42, 0x42, 0x42)},
                {"gray27", color::RGB(0x45, 0x45, 0x45)},
                {"gray28", color::RGB(0x47, 0x47, 0x47)},
                {"gray29", color::RGB(0x4a, 0x4a, 0x4a)},
                {"gray30", color::RGB(0x4d, 0x4d, 0x4d)},
                {"gray31", color::RGB(0x4f, 0x4f, 0x4f)},
                {"gray32", color::RGB(0x52, 0x52, 0x52)},
                {"gray33", color::RGB(0x54, 0x54, 0x54)},
                {"gray34", color::RGB(0x57, 0x57, 0x57)},
                {"gray35", color::RGB(0x59, 0x59, 0x59)},
                {"gray36", color::RGB(0x5c, 0x5c, 0x5c)},
                {"gray37", color::RGB(0x5e, 0x5e, 0x5e)},
                {"gray38", color::RGB(0x61, 0x61, 0x61)},
                {"gray39", color::RGB(0x63, 0x63, 0x63)},
                {"gray40", color::RGB(0x66, 0x66, 0x66)},
                {"gray41", color::RGB(0x69, 0x69, 0x69)},
                {"gray42", color::RGB(0x6b, 0x6b, 0x6b)},
                {"gray43", color::RGB(0x6e, 0x6e, 0x6e)},
                {"gray44", color::RGB(0x70, 0x70, 0x70)},
                {"gray45", color::RGB(0x73, 0x73, 0x73)},
                {"gray46", color::RGB(0x75, 0x75, 0x75)},
                {"gray47", color::RGB(0x78, 0x78, 0x78)},
                {"gray48", color::RGB(0x7a, 0x7a, 0x7a)},
                {"gray49", color::RGB(0x7d, 0x7d, 0x7d)},
                {"gray50", color::RGB(0x7f, 0x7f, 0x7f)},
                {"gray51", color::RGB(0x82, 0x82, 0x82)},
                {"gray52", color::RGB(0x85, 0x85, 0x85)},
                {"gray53", color::RGB(0x87, 0x87, 0x87)},
                {"gray54", color::RGB(0x8a, 0x8a, 0x8a)},
                {"gray55", color::RGB(0x8c, 0x8c, 0x8c)},
                {"gray56", color::RGB(0x8f, 0x8f, 0x8f)},
                {"gray57", color::RGB(0x91, 0x91, 0x91)},
                {"gray58", color::RGB(0x94, 0x94, 0x94)},
                {"gray59", color::RGB(0x96, 0x96, 0x96)},
                {"gray60", color::RGB(0x99, 0x99, 0x99)},
                {"gray61", color::RGB(0x9c, 0x9c, 0x9c)},
                {"gray62", color::RGB(0x9e, 0x9e, 0x9e)},
                {"gray63", color::RGB(0xa1, 0xa1, 0xa1)},
                {"gray64", color::RGB(0xa3, 0xa3, 0xa3)},
                {"gray65", color::RGB(0xa6, 0xa6, 0xa6)},
                {"gray66", color::RGB(0xa8, 0xa8, 0xa8)},
                {"gray67", color::RGB(0xab, 0xab, 0xab)},
                {"gray68", color::RGB(0xad, 0xad, 0xad)},
                {"gray69", color::RGB(0xb0, 0xb0, 0xb0)},
                {"gray70", color::RGB(0xb3, 0xb3, 0xb3)},
                {"gray71", color::RGB(0xb5, 0xb5, 0xb5)},
                {"gray72", color::RGB(0xb8, 0xb8, 0xb8)},
                {"gray73", color::RGB(0xba, 0xba, 0xba)},
                {"gray74", color::RGB(0xbd, 0xbd, 0xbd)},
                {"gray75", color::RGB(0xbf, 0xbf, 0xbf)},
                {"gray76", color::RGB(0xc2, 0xc2, 0xc2)},
                {"gray77", color::RGB(0xc4, 0xc4, 0xc4)},
                {"gray78", color::RGB(0xc7, 0xc7, 0xc7)},
                {"gray79", color::RGB(0xc9, 0xc9, 0xc9)},
                {"gray80", color::RGB(0xcc, 0xcc, 0xcc)},
                {"gray81", color::RGB(0xcf, 0xcf, 0xcf)},
                {"gray82", color::RGB(0xd1, 0xd1, 0xd1)},
                {"gray83", color::RGB(0xd4, 0xd4, 0xd4)},
                {"gray84", color::RGB(0xd6, 0xd6, 0xd6)},
                {"gray85", color::RGB(0xd9, 0xd9, 0xd9)},
                {"gray86", color::RGB(0xdb, 0xdb, 0xdb)},
                {"gray87", color::RGB(0xde, 0xde, 0xde)},
                {"gray88", color::RGB(0xe0, 0xe0, 0xe0)},
                {"gray89", color::RGB(0xe3, 0xe3, 0xe3)},
                {"gray90", color::RGB(0xe5, 0xe5, 0xe5)},
                {"gray91", color::RGB(0xe8, 0xe8, 0xe8)},
                {"gray92", color::RGB(0xeb, 0xeb, 0xeb)},
                {"gray93", color::RGB(0xed, 0xed, 0xed)},
                {"gray94", color::RGB(0xf0, 0xf0, 0xf0)},
                {"gray95", color::RGB(0xf2, 0xf2, 0xf2)},
                {"gray96", color::RGB(0xf5, 0xf5, 0xf5)},
                {"gray97", color::RGB(0xf7, 0xf7, 0xf7)},
                {"gray98", color::RGB(0xfa, 0xfa, 0xfa)},
                {"gray99", color::RGB(0xfc, 0xfc, 0xfc)},
                {"gray100", color::RGB(0xff, 0xff, 0xff)},
        };
}
