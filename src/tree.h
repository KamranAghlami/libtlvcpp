#pragma once

#include "tree_node.h"

namespace ka
{
    template <typename T>
    class tree : private life_time
    {
    public:
        template <typename... Args>
        tree(Args &&...args) : life_time(__func__), m_root(nullptr, std::forward<Args>(args)...)
        {
        }

        tree_node<T> &root() { return m_root; }

    private:
        tree_node<T> m_root;
    };
}