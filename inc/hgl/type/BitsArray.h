#pragma once
#include<hgl/type/MemoryUtil.h>
#include<hgl/type/AlignUtil.h>
namespace hgl
{
    namespace io
    {
        class DataInputStream;
        class DataOutputStream;
    }//namespace io

    /**
    * 位队列用于处理需要按位保存的数据，它可以按位读取和写入
    */
    class BitsArray                                                                                  ///位数据阵列
    {
        bool mapping;

        int byte_size;
        int count,pos;

        uint8 *data;

    public:

        int     GetCount()const noexcept{return count;}
        void    Resize(int);

        int     GetByteSize()const noexcept{return byte_size;}

        int     GetPos()const noexcept{return pos;}
        void    SetPos(int);

                uint8 * GetData()       {return data;}
        const   uint8 * GetData()const  {return data;}

    public:

        BitsArray(int=0);
        BitsArray(void *,int,bool=true);
        virtual ~BitsArray();

        void Create(int);                                                                           ///<创建数据区
        void MapData(void *,int);                                                                   ///<映射数据
        void Clear();                                                                               ///<清除所有数据

        void Fill(bool);                                                                            ///<填充数据

        bool WriteZero();                                                                           ///<写入一位0
        bool WriteOne();                                                                            ///<写入一位1

        bool WriteTrue(){return WriteOne();}                                                        ///<写入一位真(即1)
        bool WriteFalse(){return WriteZero();}                                                      ///<写入一位假(即0)

        bool WriteBit(bool bit)                                                                     ///<写入一位数据
            {return(bit?WriteOne():WriteZero());}
        bool WriteBytes(uint8 *,int);                                                                ///<按字节写入数据

        bool ReadBit(bool &);                                                                       ///<读取一位

        bool FlipBit();                                                                             ///<翻转一位
        bool FlipBitAt(int o)                                                                       ///<翻转指定位
            {SetPos(o);return(FlipBit());}

        bool ReadAt(   int o,bool &bit)const;                                                       ///<读取指定位
        bool WriteAt(  int o,bool bit);                                                             ///<写入一位数据

        bool SaveToStream(io::DataOutputStream *);                                                  ///<保存当前位阵列数据到流
        bool LoadFromStream(io::DataInputStream *);                                                 ///<从流中读取位阵列数据

    public: //操作符重载

        BitsArray & operator =(const BitsArray &);                                                    ///<=号重载

        BitsArray & operator +=(const BitsArray &);                                                   ///<+
        BitsArray & operator -=(const BitsArray &);                                                   ///<-
        BitsArray & operator *=(const BitsArray &);                                                   ///<*

        bool operator == (const BitsArray &) const;
        bool operator != (const BitsArray &rhs) const {return !operator==(rhs);}
    };//class BitsArray
}//namespace hgl

