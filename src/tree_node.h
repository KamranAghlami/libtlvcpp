#pragma once

#include <utility>
#include <list>
#include <functional>
#include <iostream>
#include <iomanip>

#include "utilities/life_time.h"

namespace ka
{
    template <typename T>
    class tree_node : private life_time
    {
    public:
        template <typename... Args>
        tree_node(tree_node *parent, Args &&...args) : life_time(__func__),
                                                       m_parent(parent),
                                                       m_data(std::forward<Args>(args)...)
        {
        }

        // ~tree_node(){};

        // tree_node(const tree_node &other) : life_time(__func__),
        //                                     m_parent(other.m_parent),
        //                                     m_data(other.m_data),
        //                                     m_children(other.m_children){};

        // tree_node &operator=(const tree_node &other)
        // {
        //     if (&other == this)
        //         return *this;

        //     m_parent = other.m_parent;
        //     m_data = other.m_data;
        //     m_children = other.m_children;

        //     return *this;
        // };

        // tree_node(tree_node &&other) noexcept : life_time(__func__),
        //                                         m_parent(std::move(other.m_parent)),
        //                                         m_data(std::move(other.m_data)),
        //                                         m_children(std::move(other.m_children)){};

        // tree_node &operator=(tree_node &&other) noexcept
        // {
        //     if (&other == this)
        //         return *this;

        //     m_parent = std::move(other.m_parent);
        //     m_data = std::move(other.m_data);
        //     m_children = std::move(other.m_children);

        //     return *this;
        // };

        const T &data() const
        {
            return m_data;
        }

        T &data()
        {
            return m_data;
        }

        template <typename... Args>
        tree_node<T> &add_node(Args &&...args)
        {
            return m_children.emplace_back(this, std::forward<Args>(args)...);
        }

        void dump(const size_t &indentation = 0, std::ostream &stream = std::cout) const
        {
            std::function<void(const tree_node &, size_t)> dump_recursive = [&](const tree_node &node, const size_t &indentation)
            {
                stream << std::setw(static_cast<int>(indentation) + 2) << node.m_data << '\n';

                for (const auto &child : node.m_children)
                    dump_recursive(child, indentation + 2);
            };

            dump_recursive(*this, indentation);
        }

    private:
        tree_node *m_parent;
        T m_data;
        std::list<tree_node> m_children;
    };
}