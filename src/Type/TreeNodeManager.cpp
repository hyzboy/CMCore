#include<hgl/type/TreeNode.h>

namespace hgl
{
    void TreeBaseNodeManager::OnNodeDirectDestory(BaseNode *node)                       ///<直接销毁，这种情况只在对象被直接delete的情况下，一般不需要
    {
        if(!node)return;

        if(wait_destory_node_set.contains(node))
            wait_destory_node_set.erase(node);

        if(!destored_node_set.contains(node))
            destored_node_set.emplace(node);

        if(node_map.contains(node->GetUniqueID()))
            node_map.erase(node->GetUniqueID());
    }

    void TreeBaseNodeManager::ForceClear()
    {
        if(!node_map.empty())
        {
            for(auto &it:node_map)
            {
                it.second->MarkWaitDestory();

                wait_destory_node_set.insert(it.second);
            }

            node_map.clear();
        }

        //正式的销毁可能是需要考虑node顺序的，需要根据依赖进行排序.这里暂时没做。

        if(!wait_destory_node_set.empty())
        {
            for(auto *node:wait_destory_node_set)
                OnDestoryNode(node);           //销毁节点

            wait_destory_node_set.clear();
        }
    }

    BaseNode *TreeBaseNodeManager::CreateNode()
    {
        const size_t node_id=AcquireNodeID();

        BaseNode *node=OnCreateNode(node_id);

        if(!node)
            return(nullptr);

        node_map.emplace(node_id,node);

        return(node);
    }

    const bool TreeBaseNodeManager::ContainsNode(BaseNode *tn)const
    {
        if(!tn)return(false);
        if(tn->GetManagerID()!=GetMangaerID())return(false);

        if(!node_map.contains(tn->GetUniqueID()))
            return(false);

        return(true);
    }

    bool TreeBaseNodeManager::DestoryNode(BaseNode *node)
    {
        if(!node)return(false);

        if(wait_destory_node_set.contains(node))
            return(true);

        if(!ContainsNode(node))
            return(false);

        node->MarkWaitDestory();
        node_map.erase(node->GetUniqueID());
        wait_destory_node_set.insert(node);

        return(true);
    }

    BaseNode *TreeBaseNodeManager::GetNode(const size_t node_id)
    {
        auto iter=node_map.find(node_id);

        if(iter==node_map.end())
            return(nullptr);

        return(iter->second);
    }
}//namespace hgl
