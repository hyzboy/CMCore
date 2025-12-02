#pragma once

#include<hgl/platform/Platform.h>
namespace hgl
{
    /**
     * 转换二进制字符串到整数变量
     */
    template<typename C>
    constexpr uint8 Binary2Integer(const C str[8])
    {
        return
            ((str[0] == '1')?128:0) +
            ((str[1] == '1')?64:0) +
            ((str[2] == '1')?32:0) +
            ((str[3] == '1')?16:0) +
            ((str[4] == '1')?8:0) +
            ((str[5] == '1')?4:0) +
            ((str[6] == '1')?2:0) +
            ((str[7] == '1')?1:0);
    }
    
    /**
     * 转换整数变量到二进制字符串
     */
    template<typename C>
    constexpr void Integer2Binary(C str[8],uint8 value)
    {
        str[0] = (value & 128)?'1':'0';
        str[1] = (value & 64)?'1':'0';
        str[2] = (value & 32)?'1':'0';
        str[3] = (value & 16)?'1':'0';
        str[4] = (value & 8)?'1':'0';
        str[5] = (value & 4)?'1':'0';
        str[6] = (value & 2)?'1':'0';
        str[7] = (value & 1)?'1':'0';
    }

    constexpr uint8 HGL_B0000=0;
    constexpr uint8 HGL_B0001=1;
    constexpr uint8 HGL_B0010=2;
    constexpr uint8 HGL_B0011=3;
    constexpr uint8 HGL_B0100=4;
    constexpr uint8 HGL_B0101=5;
    constexpr uint8 HGL_B0110=6;
    constexpr uint8 HGL_B0111=7;
    constexpr uint8 HGL_B1000=8;
    constexpr uint8 HGL_B1001=9;
    constexpr uint8 HGL_B1010=10;
    constexpr uint8 HGL_B1011=11;
    constexpr uint8 HGL_B1100=12;
    constexpr uint8 HGL_B1101=13;
    constexpr uint8 HGL_B1110=14;
    constexpr uint8 HGL_B1111=15;
    
    constexpr uint8 HGL_B00000000=0;
    constexpr uint8 HGL_B00000001=1;
    constexpr uint8 HGL_B00000010=2;
    constexpr uint8 HGL_B00000011=3;
    constexpr uint8 HGL_B00000100=4;
    constexpr uint8 HGL_B00000101=5;
    constexpr uint8 HGL_B00000110=6;
    constexpr uint8 HGL_B00000111=7;
    constexpr uint8 HGL_B00001000=8;
    constexpr uint8 HGL_B00001001=9;
    constexpr uint8 HGL_B00001010=10;
    constexpr uint8 HGL_B00001011=11;
    constexpr uint8 HGL_B00001100=12;
    constexpr uint8 HGL_B00001101=13;
    constexpr uint8 HGL_B00001110=14;
    constexpr uint8 HGL_B00001111=15;
    constexpr uint8 HGL_B00010000=16;
    constexpr uint8 HGL_B00010001=17;
    constexpr uint8 HGL_B00010010=18;
    constexpr uint8 HGL_B00010011=19;
    constexpr uint8 HGL_B00010100=20;
    constexpr uint8 HGL_B00010101=21;
    constexpr uint8 HGL_B00010110=22;
    constexpr uint8 HGL_B00010111=23;
    constexpr uint8 HGL_B00011000=24;
    constexpr uint8 HGL_B00011001=25;
    constexpr uint8 HGL_B00011010=26;
    constexpr uint8 HGL_B00011011=27;
    constexpr uint8 HGL_B00011100=28;
    constexpr uint8 HGL_B00011101=29;
    constexpr uint8 HGL_B00011110=30;
    constexpr uint8 HGL_B00011111=31;
    constexpr uint8 HGL_B00100000=32;
    constexpr uint8 HGL_B00100001=33;
    constexpr uint8 HGL_B00100010=34;
    constexpr uint8 HGL_B00100011=35;
    constexpr uint8 HGL_B00100100=36;
    constexpr uint8 HGL_B00100101=37;
    constexpr uint8 HGL_B00100110=38;
    constexpr uint8 HGL_B00100111=39;
    constexpr uint8 HGL_B00101000=40;
    constexpr uint8 HGL_B00101001=41;
    constexpr uint8 HGL_B00101010=42;
    constexpr uint8 HGL_B00101011=43;
    constexpr uint8 HGL_B00101100=44;
    constexpr uint8 HGL_B00101101=45;
    constexpr uint8 HGL_B00101110=46;
    constexpr uint8 HGL_B00101111=47;
    constexpr uint8 HGL_B00110000=48;
    constexpr uint8 HGL_B00110001=49;
    constexpr uint8 HGL_B00110010=50;
    constexpr uint8 HGL_B00110011=51;
    constexpr uint8 HGL_B00110100=52;
    constexpr uint8 HGL_B00110101=53;
    constexpr uint8 HGL_B00110110=54;
    constexpr uint8 HGL_B00110111=55;
    constexpr uint8 HGL_B00111000=56;
    constexpr uint8 HGL_B00111001=57;
    constexpr uint8 HGL_B00111010=58;
    constexpr uint8 HGL_B00111011=59;
    constexpr uint8 HGL_B00111100=60;
    constexpr uint8 HGL_B00111101=61;
    constexpr uint8 HGL_B00111110=62;
    constexpr uint8 HGL_B00111111=63;
    constexpr uint8 HGL_B01000000=64;
    constexpr uint8 HGL_B01000001=65;
    constexpr uint8 HGL_B01000010=66;
    constexpr uint8 HGL_B01000011=67;
    constexpr uint8 HGL_B01000100=68;
    constexpr uint8 HGL_B01000101=69;
    constexpr uint8 HGL_B01000110=70;
    constexpr uint8 HGL_B01000111=71;
    constexpr uint8 HGL_B01001000=72;
    constexpr uint8 HGL_B01001001=73;
    constexpr uint8 HGL_B01001010=74;
    constexpr uint8 HGL_B01001011=75;
    constexpr uint8 HGL_B01001100=76;
    constexpr uint8 HGL_B01001101=77;
    constexpr uint8 HGL_B01001110=78;
    constexpr uint8 HGL_B01001111=79;
    constexpr uint8 HGL_B01010000=80;
    constexpr uint8 HGL_B01010001=81;
    constexpr uint8 HGL_B01010010=82;
    constexpr uint8 HGL_B01010011=83;
    constexpr uint8 HGL_B01010100=84;
    constexpr uint8 HGL_B01010101=85;
    constexpr uint8 HGL_B01010110=86;
    constexpr uint8 HGL_B01010111=87;
    constexpr uint8 HGL_B01011000=88;
    constexpr uint8 HGL_B01011001=89;
    constexpr uint8 HGL_B01011010=90;
    constexpr uint8 HGL_B01011011=91;
    constexpr uint8 HGL_B01011100=92;
    constexpr uint8 HGL_B01011101=93;
    constexpr uint8 HGL_B01011110=94;
    constexpr uint8 HGL_B01011111=95;
    constexpr uint8 HGL_B01100000=96;
    constexpr uint8 HGL_B01100001=97;
    constexpr uint8 HGL_B01100010=98;
    constexpr uint8 HGL_B01100011=99;
    constexpr uint8 HGL_B01100100=100;
    constexpr uint8 HGL_B01100101=101;
    constexpr uint8 HGL_B01100110=102;
    constexpr uint8 HGL_B01100111=103;
    constexpr uint8 HGL_B01101000=104;
    constexpr uint8 HGL_B01101001=105;
    constexpr uint8 HGL_B01101010=106;
    constexpr uint8 HGL_B01101011=107;
    constexpr uint8 HGL_B01101100=108;
    constexpr uint8 HGL_B01101101=109;
    constexpr uint8 HGL_B01101110=110;
    constexpr uint8 HGL_B01101111=111;
    constexpr uint8 HGL_B01110000=112;
    constexpr uint8 HGL_B01110001=113;
    constexpr uint8 HGL_B01110010=114;
    constexpr uint8 HGL_B01110011=115;
    constexpr uint8 HGL_B01110100=116;
    constexpr uint8 HGL_B01110101=117;
    constexpr uint8 HGL_B01110110=118;
    constexpr uint8 HGL_B01110111=119;
    constexpr uint8 HGL_B01111000=120;
    constexpr uint8 HGL_B01111001=121;
    constexpr uint8 HGL_B01111010=122;
    constexpr uint8 HGL_B01111011=123;
    constexpr uint8 HGL_B01111100=124;
    constexpr uint8 HGL_B01111101=125;
    constexpr uint8 HGL_B01111110=126;
    constexpr uint8 HGL_B01111111=127;
    constexpr uint8 HGL_B10000000=128;
    constexpr uint8 HGL_B10000001=129;
    constexpr uint8 HGL_B10000010=130;
    constexpr uint8 HGL_B10000011=131;
    constexpr uint8 HGL_B10000100=132;
    constexpr uint8 HGL_B10000101=133;
    constexpr uint8 HGL_B10000110=134;
    constexpr uint8 HGL_B10000111=135;
    constexpr uint8 HGL_B10001000=136;
    constexpr uint8 HGL_B10001001=137;
    constexpr uint8 HGL_B10001010=138;
    constexpr uint8 HGL_B10001011=139;
    constexpr uint8 HGL_B10001100=140;
    constexpr uint8 HGL_B10001101=141;
    constexpr uint8 HGL_B10001110=142;
    constexpr uint8 HGL_B10001111=143;
    constexpr uint8 HGL_B10010000=144;
    constexpr uint8 HGL_B10010001=145;
    constexpr uint8 HGL_B10010010=146;
    constexpr uint8 HGL_B10010011=147;
    constexpr uint8 HGL_B10010100=148;
    constexpr uint8 HGL_B10010101=149;
    constexpr uint8 HGL_B10010110=150;
    constexpr uint8 HGL_B10010111=151;
    constexpr uint8 HGL_B10011000=152;
    constexpr uint8 HGL_B10011001=153;
    constexpr uint8 HGL_B10011010=154;
    constexpr uint8 HGL_B10011011=155;
    constexpr uint8 HGL_B10011100=156;
    constexpr uint8 HGL_B10011101=157;
    constexpr uint8 HGL_B10011110=158;
    constexpr uint8 HGL_B10011111=159;
    constexpr uint8 HGL_B10100000=160;
    constexpr uint8 HGL_B10100001=161;
    constexpr uint8 HGL_B10100010=162;
    constexpr uint8 HGL_B10100011=163;
    constexpr uint8 HGL_B10100100=164;
    constexpr uint8 HGL_B10100101=165;
    constexpr uint8 HGL_B10100110=166;
    constexpr uint8 HGL_B10100111=167;
    constexpr uint8 HGL_B10101000=168;
    constexpr uint8 HGL_B10101001=169;
    constexpr uint8 HGL_B10101010=170;
    constexpr uint8 HGL_B10101011=171;
    constexpr uint8 HGL_B10101100=172;
    constexpr uint8 HGL_B10101101=173;
    constexpr uint8 HGL_B10101110=174;
    constexpr uint8 HGL_B10101111=175;
    constexpr uint8 HGL_B10110000=176;
    constexpr uint8 HGL_B10110001=177;
    constexpr uint8 HGL_B10110010=178;
    constexpr uint8 HGL_B10110011=179;
    constexpr uint8 HGL_B10110100=180;
    constexpr uint8 HGL_B10110101=181;
    constexpr uint8 HGL_B10110110=182;
    constexpr uint8 HGL_B10110111=183;
    constexpr uint8 HGL_B10111000=184;
    constexpr uint8 HGL_B10111001=185;
    constexpr uint8 HGL_B10111010=186;
    constexpr uint8 HGL_B10111011=187;
    constexpr uint8 HGL_B10111100=188;
    constexpr uint8 HGL_B10111101=189;
    constexpr uint8 HGL_B10111110=190;
    constexpr uint8 HGL_B10111111=191;
    constexpr uint8 HGL_B11000000=192;
    constexpr uint8 HGL_B11000001=193;
    constexpr uint8 HGL_B11000010=194;
    constexpr uint8 HGL_B11000011=195;
    constexpr uint8 HGL_B11000100=196;
    constexpr uint8 HGL_B11000101=197;
    constexpr uint8 HGL_B11000110=198;
    constexpr uint8 HGL_B11000111=199;
    constexpr uint8 HGL_B11001000=200;
    constexpr uint8 HGL_B11001001=201;
    constexpr uint8 HGL_B11001010=202;
    constexpr uint8 HGL_B11001011=203;
    constexpr uint8 HGL_B11001100=204;
    constexpr uint8 HGL_B11001101=205;
    constexpr uint8 HGL_B11001110=206;
    constexpr uint8 HGL_B11001111=207;
    constexpr uint8 HGL_B11010000=208;
    constexpr uint8 HGL_B11010001=209;
    constexpr uint8 HGL_B11010010=210;
    constexpr uint8 HGL_B11010011=211;
    constexpr uint8 HGL_B11010100=212;
    constexpr uint8 HGL_B11010101=213;
    constexpr uint8 HGL_B11010110=214;
    constexpr uint8 HGL_B11010111=215;
    constexpr uint8 HGL_B11011000=216;
    constexpr uint8 HGL_B11011001=217;
    constexpr uint8 HGL_B11011010=218;
    constexpr uint8 HGL_B11011011=219;
    constexpr uint8 HGL_B11011100=220;
    constexpr uint8 HGL_B11011101=221;
    constexpr uint8 HGL_B11011110=222;
    constexpr uint8 HGL_B11011111=223;
    constexpr uint8 HGL_B11100000=224;
    constexpr uint8 HGL_B11100001=225;
    constexpr uint8 HGL_B11100010=226;
    constexpr uint8 HGL_B11100011=227;
    constexpr uint8 HGL_B11100100=228;
    constexpr uint8 HGL_B11100101=229;
    constexpr uint8 HGL_B11100110=230;
    constexpr uint8 HGL_B11100111=231;
    constexpr uint8 HGL_B11101000=232;
    constexpr uint8 HGL_B11101001=233;
    constexpr uint8 HGL_B11101010=234;
    constexpr uint8 HGL_B11101011=235;
    constexpr uint8 HGL_B11101100=236;
    constexpr uint8 HGL_B11101101=237;
    constexpr uint8 HGL_B11101110=238;
    constexpr uint8 HGL_B11101111=239;
    constexpr uint8 HGL_B11110000=240;
    constexpr uint8 HGL_B11110001=241;
    constexpr uint8 HGL_B11110010=242;
    constexpr uint8 HGL_B11110011=243;
    constexpr uint8 HGL_B11110100=244;
    constexpr uint8 HGL_B11110101=245;
    constexpr uint8 HGL_B11110110=246;
    constexpr uint8 HGL_B11110111=247;
    constexpr uint8 HGL_B11111000=248;
    constexpr uint8 HGL_B11111001=249;
    constexpr uint8 HGL_B11111010=250;
    constexpr uint8 HGL_B11111011=251;
    constexpr uint8 HGL_B11111100=252;
    constexpr uint8 HGL_B11111101=253;
    constexpr uint8 HGL_B11111110=254;
    constexpr uint8 HGL_B11111111=255;
}//namespace hgl
