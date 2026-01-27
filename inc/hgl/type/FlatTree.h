#pragma once

#include <vector>
#include <functional>
#include <optional>

namespace hgl
{
    // FlatTree: store tree nodes in a flat vector. Each node keeps indices to parent, first child and next sibling.
    // Advantages: compact memory layout, easy traversal of contiguous memory, stable indices (until removals).

    template<typename T>
    class FlatTree
    {
    public:
        using value_type = T;
        using size_type  = std::size_t;
        static constexpr size_type npos = (size_type)-1;

        struct Node
        {
            T       value;
            size_type parent = npos;
            size_type first_child = npos;
            size_type next_sibling = npos;

            Node() = default;
            Node(const T &v):value(v){}
            Node(T &&v):value(std::move(v)){}
        };

    protected:
        std::vector<Node> nodes;

    public:
        FlatTree() = default;

        size_type size()const{return nodes.size();}
        bool empty()const{return nodes.empty();}

        // Create a root node
        size_type AddRoot(const T &value)
        {
            nodes.emplace_back(value);
            return nodes.size() - 1;
        }

        size_type AddRoot(T &&value)
        {
            nodes.emplace_back(std::move(value));
            return nodes.size() - 1;
        }

        // Add child to parent (append as last child)
        size_type AddChild(size_type parent_index, const T &value)
        {
            if(parent_index >= nodes.size()) return npos;
            size_type idx = nodes.size();
            nodes.emplace_back(value);
            nodes[idx].parent = parent_index;

            // link into parent's child list
            if(nodes[parent_index].first_child == npos)
                nodes[parent_index].first_child = idx;
            else
            {
                // find last sibling
                size_type s = nodes[parent_index].first_child;
                while(nodes[s].next_sibling != npos) s = nodes[s].next_sibling;
                nodes[s].next_sibling = idx;
            }

            return idx;
        }

        size_type AddChild(size_type parent_index, T &&value)
        {
            if(parent_index >= nodes.size()) return npos;
            size_type idx = nodes.size();
            nodes.emplace_back(std::move(value));
            nodes[idx].parent = parent_index;

            if(nodes[parent_index].first_child == npos)
                nodes[parent_index].first_child = idx;
            else
            {
                size_type s = nodes[parent_index].first_child;
                while(nodes[s].next_sibling != npos) s = nodes[s].next_sibling;
                nodes[s].next_sibling = idx;
            }

            return idx;
        }

        // Access
        const Node &GetNode(size_type i)const{return nodes[i];}
        Node &GetNode(size_type i){return nodes[i];}

        std::optional<size_type> GetParent(size_type i)const
        {
            if(i>=nodes.size()) return std::nullopt;
            return nodes[i].parent == npos ? std::nullopt : std::optional<size_type>(nodes[i].parent);
        }

        // iterate children of a node by index
        template<typename Fn>
        void ForEachChild(size_type parent_index, Fn &&fn)const
        {
            if(parent_index>=nodes.size()) return;
            for(size_type c = nodes[parent_index].first_child; c!=npos; c = nodes[c].next_sibling)
                fn(c, nodes[c].value);
        }

        // traverse entire subtree (pre-order), call fn(index, value)
        template<typename Fn>
        void TraversePreOrder(size_type root_index, Fn &&fn)const
        {
            if(root_index>=nodes.size()) return;
            fn(root_index, nodes[root_index].value);
            for(size_type c = nodes[root_index].first_child; c!=npos; c = nodes[c].next_sibling)
                TraversePreOrder(c, fn);
        }

        // non-const overload allowing modification of nodes during traversal
        template<typename Fn>
        void TraversePreOrder(size_type root_index, Fn &&fn)
        {
            if(root_index>=nodes.size()) return;
            fn(root_index, nodes[root_index].value);
            for(size_type c = nodes[root_index].first_child; c!=npos; c = nodes[c].next_sibling)
                TraversePreOrder(c, fn);
        }

        // remove subtree rooted at index (physically removes nodes and updates indices)
        bool RemoveSubtree(size_type root_index)
        {
            if(root_index>=nodes.size()) return false;

            // collect indices to remove in pre-order
            std::vector<size_type> to_remove;
            to_remove.reserve(32);
            TraversePreOrder(root_index, [&](size_type i, const T&){ to_remove.push_back(i); });

            // mark deletion
            std::vector<char> del(nodes.size(),0);
            for(auto i:to_remove) del[i]=1;

            // build new nodes vector and mapping
            std::vector<Node> new_nodes;
            new_nodes.reserve(nodes.size()-to_remove.size());
            std::vector<size_type> new_index(nodes.size(), npos);

            for(size_type i=0;i<nodes.size();++i)
            {
                if(del[i]) continue;
                new_index[i] = new_nodes.size();
                new_nodes.push_back(std::move(nodes[i]));
            }

            // remap parent/child/sibling indices
            for(auto &n:new_nodes)
            {
                if(n.parent!=npos) n.parent = new_index[n.parent];
                if(n.first_child!=npos) n.first_child = new_index[n.first_child];
                if(n.next_sibling!=npos) n.next_sibling = new_index[n.next_sibling];
            }

            nodes.swap(new_nodes);
            return true;
        }

    };
}
