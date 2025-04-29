#pragma once

#include<hgl/type/NodeManager.h>

namespace hgl
{
    template<typename T> class TreeNode:public DataNode<T>
    {
        TreeNode<T> *parent_node;                                              ///<父节点指针
        tsl::robin_map<size_t,TreeNode<T> *> child_map;                        ///<子节点集合

    private:

        friend class NodeManager;

        template<typename T> friend class DataNodeManager;
        
        TreeNode(NodeManager *nm,const size_t uid):DataNode<T>(nm,uid)
        {
            parent_node=nullptr;
        }

    public:

        virtual ~TreeNode()
        {
            if(GetChildCount())
                DestoryAllChild();

            if(parent_node)
                parent_node->DetachChild(this);             //从父节点中移除
        }

              T *operator ->()      {return DataNode<T>::get_ptr();}
        const T *operator ->()const {return DataNode<T>::get_ptr();}

    public: //子节点相关

        virtual         void            OnAttachParent(TreeNode<T> *pn){parent_node=pn;}                               ///<被附加到父节点时调用(参见AttachChild)
        virtual         void            OnDetachParent(TreeNode<T> *pn){parent_node=nullptr;}                          ///<被从父节点中移除时调用(参见DetachChild)
                const   TreeNode<T> *   GetParent()const{return parent_node;}                                           ///<获取父节点指针

                const   size_t          GetChildCount()const{return child_map.size();}                                  ///<获取子节点数量

                const   bool            Contains(const size_t id)const{return child_map.contains(id);}                  ///<根据ID判断是否包含子节点
                const   bool            Contains(const TreeNode<T> *node)const
                                        {
                                            if(!node)return(false);
                                            if(node->Node::GetManager()!=Node::GetManager())return(false);

                                            return Contains(node->GetUniqueID());
                                        }

                const   bool            AttachChild(TreeNode<T> *node)
                                        {
                                            if(!node)return(false);
                                            if(Contains(node))return(false);

                                            child_map.emplace(node->GetUniqueID(),node);
                                            node->OnAttachParent(this);
                                            return(true);
                                        }

                        void            DetachChild(TreeNode<T> *node)
                                        {
                                            if(!node)return;

                                            if(node->Node::GetManager()!=Node::GetManager())
                                                return;

                                            const size_t child_id=node->GetUniqueID();

                                            if(child_map.contains(child_id))
                                                child_map.erase(child_id);

                                            node->OnDetachParent(this);
                                        }

                        void            DestoryAllChild()
                                        {
                                            for(auto &it:child_map)
                                            {
                                                TreeNode<T> *node=it.second;

                                                if(!node)
                                                    continue;

                                                child_map.erase(node->GetUniqueID());   //从子节点集合中移除
                                                node->OnDetachParent(this);             //从父节点中移除

                                                node->Destory();                        //销毁子节点
                                            }

                                            child_map.clear();
                                        }
    };//class TreeNode
}//namespace hgl
