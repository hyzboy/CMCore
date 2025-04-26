#pragma once

#include<hgl/TypeFunc.h>
#include<typeinfo>
#include<tsl/robin_map.h>
#include<tsl/robin_set.h>

namespace hgl
{
    class TreeBaseNodeManager;

    template<typename T> class TreeNodeManager;

    enum class TreeNodeLifePhase
    {
        None=0,                 ///<未知
        Work,                   ///<工作中(一般的使用状态)
        Recycled,               ///<已回收(走过了回收流程，但并未释放资源)
        WaitDestory,            ///<等待销毁(已经进入真正的销毁流程)
        Destory,                ///<销毁(已经被销毁，不存这种对像，只在根据ID获取状态时可以得到)

        ENUM_CLASS_RANGE(None,Destory)
    };

    class TreeBaseNode
    {
        TreeBaseNodeManager *manager;                                           ///<管理器指针
        size_t unique_id;                                                       ///<唯一ID

        TreeBaseNode *parent_node;                                              ///<父节点指针
        tsl::robin_map<size_t,TreeBaseNode *> child_map;                        ///<子节点集合

        TreeNodeLifePhase life_phase;                                           ///<生命周期状态

    public:

              TreeBaseNodeManager *GetManager()     {return manager;}                               ///<获取管理器指针
        const TreeBaseNodeManager *GetManager()const{return manager;}                               ///<获取管理器指针

        const size_t GetManagerID   ()const;                                                        ///<获取管理器ID
        const size_t GetUniqueID    ()const{return unique_id;}                                      ///<获取对象唯一ID

        const TreeNodeLifePhase GetLifePhase()const{return life_phase;}                             ///<获取生命周期状态

        const bool IsWork       ()const{return life_phase==TreeNodeLifePhase::Work;}                ///<可以工作
        const bool IsRecycled   ()const{return life_phase==TreeNodeLifePhase::Recycled;}            ///<是否已经回收
        const bool IsWaitDestory()const{return life_phase==TreeNodeLifePhase::WaitDestory;}         ///<是否已经进入等待销毁状态

    private:

        friend class TreeBaseNodeManager;
        template<typename T> friend class TreeNode;
        template<typename T> friend class TreeNodeManager;

        TreeBaseNode(TreeBaseNodeManager *nm,const size_t uid);
        virtual ~TreeBaseNode();

    protected:

        virtual void MarkWaitDestory(){life_phase=TreeNodeLifePhase::WaitDestory;}                  ///<标记为等待销毁状态

        virtual void OnDestory();                                                                   ///<最后的真实销毁调用操作(默认为delete this)

    public:

        virtual void Destory();                                                                     ///<销毁节点(标记为等待销毁状态)

    public: //子节点相关

        virtual         void            OnAttachParent(TreeBaseNode *pn){parent_node=pn;}                               ///<被附加到父节点时调用(参见AttachChild)
        virtual         void            OnDetachParent(TreeBaseNode *pn){parent_node=nullptr;}                          ///<被从父节点中移除时调用(参见DetachChild)
                const   TreeBaseNode *  GetParent()const{return parent_node;}                                           ///<获取父节点指针

                const   size_t          GetChildCount()const{return child_map.size();}                                  ///<获取子节点数量

                const   bool            Contains(const size_t id)const{return child_map.contains(id);}                  ///<根据ID判断是否包含子节点
                const   bool            Contains(const TreeBaseNode *node)const;

                const   bool            AttachChild(TreeBaseNode *node);
                        void            DetachChild(TreeBaseNode *node);
                        void            DestoryAllChild();
    };//class TreeBaseNode

    class TreeBaseNodeManager
    {
        static const size_t AcquireManagerID(){static size_t id_count=0;return ++id_count;}

    private:

        size_t manager_serial;
        size_t node_serial;

    private:

        tsl::robin_map<size_t,TreeBaseNode *> node_map;           ///<节点集合

        tsl::robin_set<TreeBaseNode *> wait_destory_node_set;     ///<等待销毁的节点集合

        tsl::robin_set<TreeBaseNode *> destored_node_set;         ///<已经销毁的节点集合

    protected:

        friend class TreeBaseNode;

                const size_t    AcquireNodeID(){return ++node_serial;}

        virtual TreeBaseNode *  OnCreateNode(const size_t node_id)=0;                               ///<创建节点时调用
        virtual void            OnDestoryNode(TreeBaseNode *node)=0;                                ///<销毁节点时调用

                void            OnNodeDirectDestory(TreeBaseNode *node);                            ///<直接销毁，这种情况只在对象被直接delete的情况下，一般不需要

    public:

        const size_t GetMangaerID()const{return manager_serial;}

    public:

        TreeBaseNodeManager()
        {
            manager_serial=AcquireManagerID();
            node_serial=0;
        }

        virtual ~TreeBaseNodeManager(){}

        virtual void            ForceClear();

                TreeBaseNode *  CreateNode();

        const   bool            ContainsNode(TreeBaseNode *tn)const;

                bool            DestoryNode(TreeBaseNode *node);

                TreeBaseNode *  GetNode(const size_t node_id);

        virtual void            Update(){}
    };//class TreeBaseNodeManager

    template<typename T> class TreeNode:public TreeBaseNode
    {
        T node_data;

    private:

        friend class TreeNodeManager<T>;        ///<树节点管理器

        using TreeBaseNode::TreeBaseNode;

    public:

        virtual ~TreeNode()override{}

    public:

        operator        T &()       {return node_data;}
        operator const  T &()const  {return node_data;}

                T &operator *()     {return node_data;}
        const   T &operator *()const{return node_data;}

                T &get()       {return node_data;}
        const   T &get()const  {return node_data;}

                T *operator ->()        {return &node_data;}
        const   T *operator ->()const   {return &node_data;}

        T &operator = (const T &data)
        {
            node_data=data;
            return node_data;
        }
    };//class TreeNode

    template<typename T> class TreeNodeManager:public TreeBaseNodeManager
    {
    public:

        using NodeType=TreeNode<T>;                         ///<节点类型

    protected:

        TreeBaseNode *OnCreateNode(const size_t node_id) override
        {
            return(new NodeType(this,node_id));
        }

        void OnDestoryNode(TreeBaseNode *node)override
        {
            if(!node)return;

            node->OnDestory();
        }

    public:

        using TreeBaseNodeManager::TreeBaseNodeManager;
        virtual ~TreeNodeManager()
        {
            ForceClear();
        }

        NodeType *Create(){return (NodeType *)CreateNode();}
    };//class TreeNodeManager
}//namespace hgl
