#pragma once

#include "tree_node.h"

namespace ka
{
    template <typename T>
    class tree
    {
    public:
        template <typename... Args>
        tree(const Args &&...args) : m_root(nullptr, std::forward<Args>(args)...)
        {
        }

        const tree_node<T> &root() const { return m_root; }

        tree_node<T> &root() { return m_root; }

        void erase()
        {
            m_root.erase();
        }

        void dump() const
        {
            m_root.dump();
        }

    protected:
        tree_node<T> m_root;
    };
}