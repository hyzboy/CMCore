#pragma once

#include<hgl/type/node/NodeManager.h>
#include<hgl/type/node/TreeBaseNode.h>

namespace hgl
{
    template<typename T> class TreeNode:public DataNode<T,TreeBaseNode>
    {
    private:

        using SuperNode=DataNode<T,TreeBaseNode>;

        friend class NodeManager;

        template<typename T> friend class DataNodeManager;
        
        TreeNode(NodeManager *nm,const size_t uid):SuperNode(nm,uid){}

    public:

        virtual ~TreeNode()
        {
            //这里必须处理DetachChild问题，不然各种OnDetach虚拟函数回调就失效了
            TreeBaseNode::DetachAll();
        }

              T *operator ->()      {return SuperNode::get_ptr();}
        const T *operator ->()const {return SuperNode::get_ptr();}

        TreeNode *GetParent()
        {
            return (TreeNode *)SuperNode::GetParent();
        }

    public:

        virtual void OnAttachParent(TreeNode *){}                                                   ///<被附加到父节点时调用
        virtual void OnDetachParent(){}                                                             ///<被从父节点中移除时调用

        virtual void OnAttachParentNode(TreeBaseNode *pn)override                                   ///<被附加到父节点时调用(参见AttachChild)
        {
            SuperNode::OnAttachParentNode(pn);
            OnAttachParent((TreeNode *)pn);
        }

        virtual void OnDetachParentNode()override                                                   ///<被从父节点中移除时调用(参见DetachChild)
        {
            OnDetachParent();
            SuperNode::OnDetachParentNode();
        }

        virtual void OnDestory()override
        {
            TreeBaseNode::DetachParent();
            TreeBaseNode::DetachAllChild();
            SuperNode::OnDestory();
        }
    };//class TreeNode
}//namespace hgl
