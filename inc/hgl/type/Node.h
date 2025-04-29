#pragma once

#include<hgl/TypeFunc.h>

namespace hgl
{
    class NodeManager;

    class Node
    {
    public:

        enum class LifePhase
        {
            None=0,                 ///<未知
            Work,                   ///<工作中(一般的使用状态)
            Recycled,               ///<已回收(走过了回收流程，但并未释放资源)
            WaitDestory,            ///<等待销毁(已经进入真正的销毁流程)
            Destory,                ///<销毁(已经被销毁，不存这种对像，只在根据ID获取状态时可以得到)

            ENUM_CLASS_RANGE(None,Destory)
        };

    private:

        NodeManager *   manager;                                                                    ///<管理器指针
        size_t          unique_id;                                                                  ///<唯一ID

        LifePhase       life_phase;                                                                 ///<生命周期状态

    public:

              NodeManager * GetManager      ()     {return manager;}                                ///<获取管理器指针
        const NodeManager * GetManager      ()const{return manager;}                                ///<获取管理器指针

        const size_t        GetManagerID    ()const;                                                ///<获取管理器ID
        const size_t        GetUniqueID     ()const{return unique_id;}                              ///<获取对象唯一ID

        const LifePhase     GetLifePhase    ()const{return life_phase;}                             ///<获取生命周期状态

        const bool          IsWork          ()const{return life_phase==LifePhase::Work;}            ///<可以工作
        const bool          IsRecycled      ()const{return life_phase==LifePhase::Recycled;}        ///<是否已经回收
        const bool          IsWaitDestory   ()const{return life_phase==LifePhase::WaitDestory;}     ///<是否已经进入等待销毁状态

    protected:

        friend class NodeManager;

        virtual void MarkWaitDestory(){life_phase=LifePhase::WaitDestory;}                          ///<标记为等待销毁状态

    public: //事件

        virtual void OnDestory(){life_phase=LifePhase::Destory;}                                    ///<销毁节点(标记为销毁状态，下步就delete了，所以要做什么这是最后的机会)

    public:

        Node(NodeManager *nm,const size_t uid);
        virtual ~Node();

        virtual void Destory();                                                                     ///<销毁节点(标记为等待销毁状态)

    };//class Node

    template<typename T> class DataNode:public Node
    {
        T node_data;

    public:

        using Node::Node;
        virtual ~DataNode()override=default;

    public:

                T & get_data()      {return node_data;}
        const   T & get_data()const {return node_data;}

                T * get_ptr ()      {return &node_data;}
        const   T * get_ptr ()const {return &node_data;}

        operator T &        ()      {return node_data;}
        operator const T &  ()const {return node_data;}

        virtual void set_data(const T &data)
        {
            node_data=data;
        }
    };//class DataNode
}//namespcae hgl
