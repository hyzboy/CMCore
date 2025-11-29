#pragma once

using u32char           =char32_t;
using u16char           =char16_t;
using os_char           =char;

#define to_oschar       to_u8
#define OS_TEXT(str)    u8##str
#define U8_TEXT(str)    u8##str
#define U16_TEXT(str)   u##str
#define U32_TEXT(str)   U##str

#define os_main         main
#define os_out          std::cout
#define os_in           std::cin
#define os_err          std::cerr
#define os_log          std::clog
