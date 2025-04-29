#pragma once

#include<hgl/type/NodeManager.h>

namespace hgl
{
    class TreeBaseNode:public Node
    {
        TreeBaseNode *parent_node;                                              ///<父节点指针
        tsl::robin_map<size_t,TreeBaseNode *> child_map;                        ///<子节点集合

    private:

        friend class NodeManager;
        template<typename T> friend class TreeNode;
        template<typename T> friend class TreeNodeManager;

        TreeBaseNode(NodeManager *nm,const size_t uid);
        virtual ~TreeBaseNode();

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

    template<typename T> class TreeNodeManager:public NodeManager
    {
    public:

        using NodeType=TreeNode<T>;                         ///<节点类型

    protected:

        Node *OnCreateNode(const size_t node_id) override
        {
            return(new NodeType(this,node_id));
        }

        void OnDestoryNode(Node *node)override
        {
            if(!node)return;

            node->OnDestory();
            delete node;
        }

    public:

        using NodeManager::NodeManager;
        virtual ~TreeNodeManager()
        {
            ForceClear();
        }

        NodeType *Create(){return (NodeType *)CreateNode();}
    };//class TreeNodeManager
}//namespace hgl
