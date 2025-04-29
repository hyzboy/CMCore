#include<hgl/type/TreeNode.h>

namespace hgl
{
    BaseNode::BaseNode(TreeBaseNodeManager *nm,const size_t uid)
    {
        manager=nm;
        unique_id=uid;

        life_phase=TreeNodeLifePhase::None;
    }

    BaseNode::~BaseNode()
    {
        if(life_phase<TreeNodeLifePhase::WaitDestory)       //还没有进入销毁器
            manager->OnNodeDirectDestory(this);         //直接销毁
    }

    const size_t BaseNode::GetManagerID()const
    {
        return manager->GetMangaerID();
    }

    void BaseNode::Destory()
    {
        manager->DestoryNode(this);
    }

    void BaseNode::OnDestory()
    {
        life_phase=TreeNodeLifePhase::Destory;        //设置为销毁状态
        delete this;
    }
}//namespace hgl

namespace hgl
{
    TreeBaseNode::TreeBaseNode(TreeBaseNodeManager *nm,const size_t uid):BaseNode(nm,uid)
    {
        parent_node=nullptr;
    }

    TreeBaseNode::~TreeBaseNode()
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

        return Contains(node->GetUniqueID());
    }

    const bool TreeBaseNode::AttachChild(TreeBaseNode *node)
    {
        if(!node)return(false);
        if(Contains(node))return(false);

        child_map.emplace(node->GetUniqueID(),node);
        node->OnAttachParent(this);
        return(true);
    }

    void TreeBaseNode::DetachChild(TreeBaseNode *node)
    {
        if(!node)return;

        if(node->GetManager()!=GetManager())
            return;

        const size_t child_id=node->GetUniqueID();

        if(child_map.contains(child_id))
            child_map.erase(child_id);

        node->OnDetachParent(this);
    }

    void TreeBaseNode::DestoryAllChild()
    {
        for(auto &it:child_map)
        {
            TreeBaseNode *node=it.second;

            if(!node)
                continue;

            child_map.erase(node->GetUniqueID());   //从子节点集合中移除
            node->OnDetachParent(this);             //从父节点中移除

            node->Destory();                        //销毁子节点
        }

        child_map.clear();
    }
}//namespace hgl
