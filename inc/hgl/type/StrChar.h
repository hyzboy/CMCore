#pragma once

/**
 * CN:  在这里我们自行实现了C标准库的一些字符和字符串处理函数，
 *      这个行为并不是为了追求性能，或是在炫技。而是因为不同编译器所附带的标准库在一些边界、非法值上处理不一致。
 * 
 *      比如你在一些字符串处理函数中传入非法长度或是nullptr，不同实现的标准库都有正确的错误处理，但错误的处理方式确不一定完全一样。
 * 
 * EN:  Here we have implemented some character and string handling functions from the C standard library ourselves.
 *      This is not done for performance or to show off skills,
 *      but because the standard libraries provided by different compilers handle certain edge cases and illegal values inconsistently.
 * 
 *      For example,
 *          if you pass an illegal length or nullptr to some string handling functions,
 *          different implementations of the standard library may have correct error handling,
 *          but the way they handle errors is not necessarily the same.
 */

#include <hgl/type/CharType.h>
#include <type_traits>
#include <cmath>

namespace hgl
{



} // namespace hgl
