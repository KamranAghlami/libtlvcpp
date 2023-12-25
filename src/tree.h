#pragma once

#include <utility>
#include <list>
#include <vector>
#include <functional>
#include <iostream>
#include <iomanip>

namespace ka
{
    template <typename T>
    class tree_node
    {
    public:
        template <typename... Args>
        tree_node(Args &&...args) : m_parent(nullptr),
                                    m_data(std::forward<Args>(args)...)
        {
        }

        template <typename... Args>
        tree_node(tree_node *parent, Args &&...args) : m_parent(parent),
                                                       m_data(std::forward<Args>(args)...)
        {
        }

        tree_node(const tree_node &other) = default;
        tree_node &operator=(const tree_node &other) = default;
        tree_node(tree_node &&other) noexcept = default;
        tree_node &operator=(tree_node &&other) noexcept = default;

        const tree_node &root() const
        {
            const auto root = this;

            while (root->parent())
                root = root->parent();

            return *root;
        }

        tree_node &root()
        {
            auto root = this;

            while (root->parent())
                root = root->parent();

            return *root;
        }

        const tree_node *parent() const
        {
            return m_parent;
        }

        tree_node *parent()
        {
            return m_parent;
        }

        bool is_child_of(const tree_node &other) const
        {
            const auto *parent = m_parent;

            while (parent)
            {
                if (parent == &other)
                    return true;

                parent = parent->parent();
            }

            return false;
        }

        bool is_parent_of(const tree_node &other) const
        {
            return other.is_child_of(*this);
        }

        size_t depth() const
        {
            size_t d = 0;

            const auto node = this;

            while (node->parent())
            {
                node = node->parent();

                d++;
            }

            return d;
        }

        const T &data() const
        {
            return m_data;
        }

        T &data()
        {
            return m_data;
        }

        const std::list<tree_node> &children() const
        {
            return m_children;
        }

        std::list<tree_node> &children()
        {
            return m_children;
        }

        template <typename... Args>
        tree_node<T> &add_child(Args &&...args)
        {
            return m_children.emplace_back(this, std::forward<Args>(args)...);
        }

        bool serialize(std::vector<uint8_t> &buffer) const;
        bool deserialize(const std::vector<uint8_t> &buffer);

        void dump(const size_t &indentation = 0, std::ostream &stream = std::cout) const
        {
            std::function<void(const tree_node &, size_t)> dump_recursive = [&](const tree_node &node, const size_t &indentation)
            {
                stream << std::setw(static_cast<int>(indentation) + 1) << node.m_data;

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