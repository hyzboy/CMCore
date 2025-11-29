#pragma once

#include <atomic>
#include <cstdint>

namespace hgl
{
    // 统一使用 C++ 标准原子类型，移除平台自定义封装。
    template<typename T>
    using atom = std::atomic<T>;

    using atom_bool    = atom<bool>;
    using atom_char    = atom<char>;
    using atom_uchar   = atom<unsigned char>;
    using atom_wchar   = atom<wchar_t>;
    using atom_char8   = atom<char8_t>;
    using atom_char16  = atom<char16_t>;
    using atom_char32  = atom<char32_t>;

    using atom_int     = atom<int>;
    using atom_uint    = atom<unsigned int>;
    using atom_int32   = atom<int32_t>;
    using atom_uint32  = atom<uint32_t>;
    using atom_int64   = atom<int64_t>;
    using atom_uint64  = atom<uint64_t>;

    using atom_float   = atom<float>;
    // 如需 double: using atom_double = atom<double>;
}

