#pragma once

#include<hgl/object/RefObject.h>

namespace hgl
{
    /**
    * 负载(Payload)状态
    *
    * MetaOnly 与 Resident 之间可以反复切换：卸载负载后 meta 信息仍在，
    * 因此可以在不重新解析资源头的情况下再次加载。
    */
    enum class PayloadState:uint8
    {
        MetaOnly=0,     ///<只有meta信息，未加载实际资源
        Loading,        ///<正在加载负载
        Resident,       ///<负载已在内存中，可正常使用
        Unloading,      ///<正在卸载负载
        LoadFailed,     ///<加载失败(可重试)
        Disposed,       ///<meta与负载均已释放，对象不可再用
    };//enum class PayloadState

    const os_char *ToString(PayloadState);

    /**
    * 资源meta信息
    *
    * 只保存"可重建负载所需的信息"，不持有任何运行时资源句柄。
    * 典型用途：纹理记住文件名与内容哈希，卸载显存后仍能按需重建。
    */
    struct AssetMetaHeader
    {
        OSString    source_path;                            ///<源文件路径/资源定位串
        OSString    asset_type;                             ///<资源类型名
        uint64      source_size     =0;                     ///<源数据字节数
        uint64      source_hash     =0;                     ///<源数据FNV1a哈希
        uint32      meta_version    =0;                     ///<meta格式版本(由派生类定义)
        uint32      user_flags      =0;                     ///<派生类自定义标记

        bool IsValid()const{return(!source_path.IsEmpty());}
    };//struct AssetMetaHeader

    /**
    * 带meta信息的引用计数对象
    *
    * 在 RefObject 之上划分为两层契约：
    *
    * | 层 | 钩子 | 何时调用 |
    * |----|------|---------|
    * | meta | OnInitMeta()/OnReleaseMeta() | Init()/Release() 时 |
    * | 负载 | OnLoadPayload()/OnUnloadPayload() | LoadPayload()/UnloadPayload() 时 |
    *
    * 默认归零策略为 KeepMetaUnloadPayload：引用归零只卸载负载并释放生命周期资源，
    * 对象壳与meta信息保留，可由持有者再次 LoadPayload() 复用。
    *
    * @code
    * class TextureAsset:public hgl::MetaObject
    * {
    * public:
    *
    *     HGL_OBJECT(TextureAsset,hgl::MetaObject)
    *
    * protected:
    *
    *     bool OnLoadPayload() override
    *     {
    *         // 按 GetMeta().source_path 加载，失败返回false
    *         return(true);
    *     }
    *
    *     void OnUnloadPayload() override
    *     {
    *         // 释放显存等运行时资源
    *     }
    * };
    * @endcode
    */
    class MetaObject:public RefObject
    {
    public:

        NO_COPY(MetaObject)
        NO_MOVE(MetaObject)

    protected:

        AssetMetaHeader     meta;                           ///<meta信息(不随负载卸载而失效)
        PayloadState        payload_state;                  ///<负载状态

    protected:  // 双层契约

        /**
        * meta层初始化：只准备meta信息，不加载资源(默认成功)
        */
        virtual bool OnInitMeta(){return(true);}

        /**
        * meta层释放：在负载已卸载之后调用
        */
        virtual void OnReleaseMeta(){}

        /**
        * 按meta信息加载负载
        * @return true 加载成功(进入Resident)；false 加载失败(进入LoadFailed，可重试)
        */
        virtual bool OnLoadPayload()=0;

        /**
        * 卸载负载，必须释放OnLoadPayload及其后申请的全部运行时资源
        */
        virtual void OnUnloadPayload(){}

    private:    // 封闭Object层的模板方法，强制走双层契约

        bool OnInit() final;

        void OnRelease() final;

    public:

        MetaObject();
        virtual ~MetaObject()=default;

    public:     // meta信息

        const AssetMetaHeader &GetMeta()const{return meta;}

        AssetMetaHeader &GetMeta(){return meta;}

    public:     // 负载管理

        const PayloadState GetPayloadState()const{return payload_state;}

        const bool IsPayloadResident()const{return payload_state==PayloadState::Resident;}

        const bool IsPayloadLoaded()const
        {
            return payload_state==PayloadState::Resident
                 ||payload_state==PayloadState::Unloading;
        }

        /**
        * 加载负载(幂等：已加载时直接返回true)
        * @note 必须先Init()
        * @return true 负载可用
        */
        bool LoadPayload();

        /**
        * 卸载负载(幂等)，meta信息保留
        */
        void UnloadPayload();
    };//class MetaObject
}//namespace hgl
