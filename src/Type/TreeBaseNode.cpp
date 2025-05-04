#include<hgl/type/TreeBaseNode.h>

namespace hgl
{
    void TreeBaseNode::DetachAll()
    {
        if(GetChildCount())
            DestoryAllChild();

        if(parent_node)
            parent_node->DetachChild(this);             //从父节点中移除
    }

    const bool TreeBaseNode::Contains(const TreeBaseNode *node)const
    {
        if(!node)return(false);
        if(node->GetManager()!=GetManager())return(false);
        if(node->GetParent()!=this)return(false);
        return Contains(node->GetUniqueID());
    }

    const bool TreeBaseNode::AttachChild(TreeBaseNode *node)
    {
        if(!node)return(false);
        if(Contains(node))return(false);

        child_map.emplace(node->GetUniqueID(),node);
        node->OnAttachParentNode(this);
        return(true);
    }

    void TreeBaseNode::DetachChild(TreeBaseNode *node)
    {
        if(!node)return;

        if(node->Node::GetManager()!=Node::GetManager())
            return;

        const size_t child_id=node->GetUniqueID();

        if(!child_map.contains(child_id))
            return;

        child_map.erase(child_id);
        node->OnDetachParentNode();
    }

    void TreeBaseNode::DetachAllChild()
    {
        for(auto &it:child_map)
        {
            TreeBaseNode *node=it.second;

            if(!node)
                continue;

            node->OnDetachParentNode();             //从父节点中移除
        }

        child_map.clear();
    }

    void TreeBaseNode::DestoryAllChild()
    {
        for(auto &it:child_map)
        {
            TreeBaseNode *node=it.second;

            if(!node)
                continue;

            child_map.erase(node->GetUniqueID());   //从子节点集合中移除
            node->OnDetachParentNode();             //从父节点中移除

            node->Destory();                        //销毁子节点
        }

        child_map.clear();
    }

    void TreeBaseNode::DetachParent()
    {
        if(!parent_node)
            return;

        parent_node->DetachChild(this);       //从父节点中移除
        parent_node=nullptr;
    }
}//namespace hgl
