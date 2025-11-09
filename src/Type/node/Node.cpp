#include<hgl/type/node/NodeManager.h>

namespace hgl
{
    Node::Node(NodeManager *nm,const size_t uid)
    {
        manager=nm;
        unique_id=uid;

        life_phase=LifePhase::None;
    }

    Node::~Node()
    {
        if(life_phase<LifePhase::WaitDestory)       //还没有进入销毁器
            manager->OnNodeDirectDestory(this);         //直接销毁
    }

    const size_t Node::GetManagerID()const
    {
        return manager->GetMangaerID();
    }

    void Node::Destory()
    {
        manager->DestoryNode(this);
    }
}//namespace hgl
