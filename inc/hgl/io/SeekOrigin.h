#ifndef HGL_IO_SEEK_ORIGIN_INCLUDE
#define HGL_IO_SEEK_ORIGIN_INCLUDE

namespace hgl
{
    namespace io
    {
        enum class SeekOrigin     /// 资源偏移方向枚举
        {
            Begin=0,              ///<从资源最开始处开始，offset必须大于0。移到资源的offset位置
            Current,              ///<从资源当前位置开始，移到资源的Position+offset位置
            End                   ///<从资源的结束位置开始，offset必须小于0，表示结束前的字符数
        };//enum SeekOrigin
    }//namespace io
}//namespace hgl
#endif//HGL_IO_SEEK_ORIGIN_INCLUDE
