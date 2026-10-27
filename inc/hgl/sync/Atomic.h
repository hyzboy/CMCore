#pragma once

#include <atomic>
#include <cstdint>

namespace hgl
{
    /**
     * @brief CN: HGL 原子类型的统一入口。
     *        EN: Unified entry point for HGL atomic types.
     * @details CN: 此处仅为原子类型起 HGL 命名别名，操作语义仍遵循 C++ 原子规则；
     *              读写共享状态时应根据数据依赖选择 memory_order，不能把原子变量
     *              当作互斥锁，也不能依赖原子操作自动保护它所指向的复合对象。
     *              对外的同步 API 使用 hgl::atom<T> 及以下常用别名，避免业务代码
     *              直接绑定某个平台的原子实现。
     *          EN: These are HGL-named aliases for atomic types and retain C++ atomic
     *              semantics. Select memory_order based on data dependencies. An atomic
     *              variable is not a mutex and does not protect a compound object it
     *              points to. Public synchronization APIs use hgl::atom<T> and the
     *              aliases below rather than binding application code to a platform
     *              implementation.
     */
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
    /// @brief CN: 常用类型别名与 atom<T> 等价；其他类型可直接使用 atom<T>。
    ///        EN: Common aliases are equivalent to atom<T>; use atom<T> directly for other types.
    /// @note CN: 如需 double，可声明 using atom_double = atom<double>。
    ///       EN: To use double, declare using atom_double = atom<double>.
}
