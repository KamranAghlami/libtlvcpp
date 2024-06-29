#pragma once

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <functional>
#include <iomanip>
#include <iostream>
#include <limits>
#include <list>
#include <queue>
#include <utility>
#include <vector>

namespace tlvcpp
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

        tree_node(const tree_node &other) : m_parent(nullptr),
                                            m_data(other.m_data)
        {
            for (const auto &child : other.m_children)
                graft(child);
        };

        tree_node(tree_node &other) : tree_node(const_cast<const tree_node &>(other)){};

        tree_node &operator=(const tree_node &other)
        {
            if (this == &other)
                return *this;

            assert(!other.is_parent_of(*this));

            m_data = other.m_data;

            for (const auto &child : other.m_children)
                graft(child);

            return *this;
        };

        tree_node(tree_node &&other) noexcept : m_parent(nullptr),
                                                m_data(std::move(other.m_data)),
                                                m_children(std::move(other.m_children))
        {
            if (other.m_parent)
                other.parent()->prune(other);

            for (auto &child : m_children)
                child.m_parent = this;
        };

        tree_node &operator=(tree_node &&other) noexcept
        {
            if (this == &other)
                return *this;

            assert(!other.is_parent_of(*this));

            m_data = std::move(other.m_data);

            if (is_parent_of(other))
                m_children = std::move(std::list<tree_node>(std::move(other.m_children)));
            else
            {
                m_children = std::move(other.m_children);

                if (other.m_parent)
                    other.parent()->prune(other);
            }

            for (auto &child : m_children)
                child.m_parent = this;

            return *this;
        };

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

        void graft(const tree_node &node)
        {
            assert(this != &node && !node.is_parent_of(*this));

            auto &grafted_child = add_child(node.m_data);

            for (const auto &child : node.m_children)
                grafted_child.graft(child);
        }

        void prune(tree_node &node)
        {
            auto child_node = std::find_if(m_children.begin(),
                                           m_children.end(),
                                           [&node](const tree_node &_node)
                                           { return &_node == &node; });

            if (child_node != m_children.end())
                m_children.erase(child_node);
        }

        template <typename U>
        const tree_node *find(U value, size_t index = 0) const
        {
            return find_impl(value, index);
        }

        template <typename U>
        tree_node *find(U value, size_t index = 0)
        {
            return find_impl(value, index);
        }

        template <typename U>
        const tree_node *find_immediate(U value, size_t index = 0) const
        {
            return find_immediate_impl(value, index);
        }

        template <typename U>
        tree_node *find_immediate(U value, size_t index = 0)
        {
            return find_immediate_impl(value, index);
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

        bool serialize(std::vector<uint8_t> &buffer, size_t *bytes_written = nullptr) const;
        bool deserialize(const uint8_t *buffer, const size_t size);
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
        template <typename U>
        tree_node *find_impl(U value, size_t index = 0) const
        {
            std::queue<const tree_node *> queue;

            queue.push(this);

            while (queue.size())
            {
                const auto node = queue.front();

                queue.pop();

                if (node->m_data == value && !(index--))
                    return const_cast<tree_node *>(node);

                for (const auto &child : node->m_children)
                    queue.push(&child);
            }

            return nullptr;
        }

        template <typename U>
        tree_node *find_immediate_impl(U value, size_t index = 0) const
        {
            for (const auto &child : m_children)
                if (child.m_data == value && !(index--))
                    return const_cast<tree_node *>(&child);

            return nullptr;
        }

        tree_node *m_parent;
        T m_data;
        std::list<tree_node> m_children;
    };
}
