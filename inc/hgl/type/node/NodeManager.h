#pragma once

#include<hgl/type/node/Node.h>
#include<typeinfo>
#include<tsl/robin_map.h>
#include<tsl/robin_set.h>

namespace hgl
{
    class NodeManager
    {
        static const size_t AcquireManagerID(){static size_t id_count=0;return ++id_count;}

    private:

        size_t manager_serial;
        size_t node_serial;

    private:

        tsl::robin_map<size_t,Node *> node_map;           ///<节点集合

        tsl::robin_set<Node *> wait_destory_node_set;     ///<等待销毁的节点集合

        tsl::robin_set<Node *> destored_node_set;         ///<已经销毁的节点集合

    protected:

        friend class Node;

                const   size_t  AcquireNodeID       (){return ++node_serial;}

        virtual         Node *  OnCreateNode        (const size_t node_id)=0;                       ///<创建节点时调用
        virtual         void    OnDestoryNode       (Node *node)=0;                                 ///<销毁节点时调用

                        void    OnNodeDirectDestory (Node *node);                                   ///<直接销毁，这种情况只在对象被直接delete的情况下，一般不需要

    public:

                const   size_t  GetMangaerID        ()const{return manager_serial;}

    public:

        NodeManager()
        {
            manager_serial=AcquireManagerID();
            node_serial=0;
        }

        virtual ~NodeManager()=default;

        virtual void    ForceClear();

                Node *  CreateNode();

        const   bool    ContainsNode(Node *tn)const;

                bool    DestoryNode(Node *node);

                Node *  GetNode(const size_t node_id);

        virtual void    Update(){}
    };//class NodeManager

    template<typename T> class DataNodeManager:public NodeManager
    {
    protected: //事件

        Node *OnCreateNode(const size_t node_id) override
        {
            return(new T(this,node_id));
        }

        void OnDestoryNode(Node *node)override
        {
            if(!node)return;

            node->OnDestory();
            delete node;
        }

    public:

        using NodeManager::NodeManager;
        virtual ~DataNodeManager()
        {
            ForceClear();
        }

        T *Create()
        {
            return (T *)CreateNode();
            //走基类的CreateNode()是为了添加到管理器中
            //基类会通过虚拟的OnCreateNode()再度回到本类来创建节点
        }
    };//class DataNodeManager
}//namespace hgl
