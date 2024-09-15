#pragma once

#include "tlv.h"
#include "tree.h"

namespace tlvcpp
{
    template <typename allocator = std::allocator<value_t>>
    using tlv_tree_node = tree_node<tlv<allocator>>;

    length_t length_of_tag(const tag_t tag);
    length_t length_of_length(const length_t length);
    bool serialize_tag(const tag_t tag, std::vector<uint8_t> &buffer);
    bool serialize_length(const length_t length, std::vector<uint8_t> &buffer);
    bool serialize_value(const uint8_t *value, const size_t size, std::vector<uint8_t> &buffer);

    template <typename allocator>
    length_t node_length_recursive(const tlv_tree_node<allocator> &node)
    {
        const auto &tlv = node.data();
        length_t size = 0;

        size += length_of_tag(tlv.tag());

        if (tag_is_primitive(tlv.tag()))
            size += tlv.length();
        else
            for (const auto &child : node.children())
                size += node_length_recursive(child);

        size += length_of_length(size);

        return size;
    }

    template <typename allocator>
    length_t node_length(const tlv_tree_node<allocator> &node)
    {
        const auto &tlv = node.data();

        if (tag_is_primitive(tlv.tag()))
            return tlv.length();

        length_t size = 0;

        for (const auto &child : node.children())
            size += node_length_recursive(child);

        return size;
    }

    template <typename allocator>
    bool serialize_recursive(const tlv_tree_node<allocator> &node, std::vector<uint8_t> &buffer)
    {
        const auto &tlv = node.data();

        if (!serialize_tag(tlv.tag(), buffer))
            return false;

        if (!serialize_length(node_length(node), buffer))
            return false;

        if (tag_is_primitive(tlv.tag()))
        {
            if (!serialize_value(tlv.value(), tlv.length(), buffer))
                return false;
        }
        else
            for (const auto &child : node.children())
                if (!serialize_recursive(child, buffer))
                    return false;

        return true;
    }

    template <typename allocator>
    bool serialize(const tree_node<tlvcpp::tlv<allocator>> &node, std::vector<uint8_t> &buffer)
    {
        if (node.data().tag())
        {
            if (!serialize_recursive(node, buffer))
                return false;
        }
        else
            for (const auto &child : node.children())
                if (!serialize_recursive(child, buffer))
                    return false;

        return true;
    }

    bool deserialize_tag(const uint8_t *&buffer, size_t &remaining_size, tag_t &tag);
    bool deserialize_length(const uint8_t *&buffer, size_t &remaining_size, length_t &length);

    template <typename allocator>
    bool deserialize_recursive(const uint8_t *&buffer, size_t &remaining_size, tlv_tree_node<allocator> &node)
    {
        while (remaining_size)
        {
            tag_t tag = 0;

            if (!deserialize_tag(buffer, remaining_size, tag))
                return false;

            length_t length = 0;

            if (!deserialize_length(buffer, remaining_size, length))
                return false;

            if (length > remaining_size)
                return false;

            if (tag_is_primitive(tag))
            {
                node.add_child(tag, length, buffer);

                buffer += length;
                remaining_size -= length;
            }
            else
            {
                auto &child = node.add_child(tag, 0U, nullptr);

                size_t _remaining_size = length;

                if (!deserialize_recursive(buffer, _remaining_size, child))
                    return false;

                remaining_size -= length;
            }
        }

        return true;
    }

    template <typename allocator>
    bool deserialize(const uint8_t *buffer, const size_t size, tree_node<tlvcpp::tlv<allocator>> &node)
    {
        size_t remaining_size = size;

        if (!deserialize_recursive(buffer, remaining_size, node))
            return false;

        if (node.data().tag() == 0 && node.children().size() == 1)
            node = std::move(node.children().front());

        return true;
    }

    template <typename allocator>
    bool deserialize(const std::vector<uint8_t> &buffer, tree_node<tlvcpp::tlv<allocator>> &node)
    {
        return deserialize(buffer.data(), buffer.size(), node);
    }
}