#pragma once

#include<hgl/type/node/Node.h>
#include<tsl/robin_map.h>

namespace hgl
{
    class TreeBaseNode:public Node
    {
        TreeBaseNode *parent_node;                                              ///<父节点指针
        tsl::robin_map<size_t,TreeBaseNode *> child_map;                        ///<子节点集合

    protected:

        void DetachAll();

    public:

        TreeBaseNode(NodeManager *nm,const size_t uid):Node(nm,uid)
        {
            parent_node=nullptr;
        }

        virtual ~TreeBaseNode()
        {
            DetachAll();
        }

    public:

        virtual void OnAttachParentNode(TreeBaseNode *pn){parent_node=pn;}                              ///<被附加到父节点时调用(参见AttachChild)
        virtual void OnDetachParentNode(){parent_node=nullptr;}                                         ///<被从父节点中移除时调用(参见DetachChild)

    public:

        const   TreeBaseNode *  GetParent()const{return parent_node;}                                           ///<获取父节点指针

        const   size_t          GetChildCount()const{return child_map.size();}                                  ///<获取子节点数量

        const   bool            Contains(const size_t id)const{return child_map.contains(id);}                  ///<根据ID判断是否包含子节点
        const   bool            Contains(const TreeBaseNode *node)const;

        const   bool            AttachChild(TreeBaseNode *node);
                void            DetachChild(TreeBaseNode *node);
                void            DetachAllChild();

                void            DestoryAllChild();

                void            DetachParent();
    };//class TreeBaseNode
}//namespace hgl
