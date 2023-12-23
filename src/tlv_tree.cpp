#include "tlv_tree.h"

#include <cstring>

namespace ka
{
    static bool tag_is_primitive(tag_t tag)
    {
        union
        {
            tag_t tag;
            uint8_t byte[sizeof(tag_t)];
        } tag_bytes;

        tag_bytes.tag = tag;

        for (ssize_t i = sizeof(tag_t) - 1; i > -1; i--)
            if (tag_bytes.byte[i])
                return !(tag_bytes.byte[i] & 0b00100000);

        return false;
    }

    static length_t length_of_tag(const tag_t tag)
    {
        union
        {
            tag_t tag;
            uint8_t byte[sizeof(tag_t)];
        } tag_bytes;

        tag_bytes.tag = tag;

        length_t length = 1;

        while (tag_bytes.byte[length])
        {
            length++;

            if (length > sizeof(tag_t))
            {
                length = sizeof(tag_t);

                break;
            }
        }

        return length;
    }

    static length_t length_of_length(const length_t length)
    {
        length_t length_of_length = 1;

        if (length < 128)
            return length_of_length;

        union
        {
            length_t length;
            uint8_t byte[sizeof(length_t)];
        } length_bytes;

        length_bytes.length = length;

        while (length_bytes.byte[length_of_length])
        {
            length_of_length++;

            if (length_of_length > sizeof(length_t))
            {
                length_of_length = sizeof(length_t);

                break;
            }
        }

        return length_of_length + 1;
    }

    static length_t node_length_recursive(const _tree_node<tlv> &node)
    {
        const ka::tlv &tlv = node.data();
        length_t size = 0;

        size += length_of_tag(tlv.tag());
        size += length_of_length(tlv.length());

        if (tag_is_primitive(tlv.tag()))
        {
            size += tlv.length();

            return size;
        }

        for (const auto &child : node.children())
            size += node_length_recursive(child);

        return size;
    }

    static length_t node_length(const _tree_node<tlv> &node)
    {
        const ka::tlv &tlv = node.data();

        if (tag_is_primitive(tlv.tag()))
            return tlv.length();

        length_t size = 0;

        for (const auto &child : node.children())
            size += node_length_recursive(child);

        return size;
    }

    static bool serialize_tag(const tag_t tag, std::vector<uint8_t> &buffer)
    {
        if (!tag)
            return false;

        size_t length = length_of_tag(tag);

        union
        {
            tag_t tag;
            uint8_t byte[sizeof(tag_t)];
        } tag_bytes;

        tag_bytes.tag = tag;

        for (ssize_t i = length - 1; i > -1; i--)
            buffer.push_back(tag_bytes.byte[i]);

        return true;
    }

    static bool serialize_length(const length_t length, std::vector<uint8_t> &buffer)
    {
        if (length < 0b01111111)
        {
            buffer.push_back(static_cast<uint8_t>(length));

            return true;
        }

        union
        {
            length_t length;
            uint8_t byte[sizeof(length_t)];
        } length_bytes;

        length_bytes.length = length;

        uint8_t length_of_length = sizeof(length_t);

        for (ssize_t i = sizeof(length_t) - 1; i > -1; i--)
            if (!length_bytes.byte[i])
                length_of_length--;
            else
                break;

        if (length_of_length > sizeof(length_t))
            return false;

        buffer.push_back(length_of_length | 0b10000000);

        ssize_t byte_index = length_of_length - 1;

        while (byte_index > -1)
            buffer.push_back(length_bytes.byte[byte_index--]);

        return true;
    }

    static bool serialize_value(const uint8_t *value, const size_t size, std::vector<uint8_t> &buffer)
    {
        if (size)
            buffer.insert(buffer.end(), value, value + size);

        return true;
    }

    static bool serialize_recursive(const _tree_node<tlv> &node, std::vector<uint8_t> &buffer)
    {
        const ka::tlv &tlv = node.data();

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

    bool tree_node<ka::tlv>::serialize(std::vector<uint8_t> &buffer) const
    {
        if (data().tag())
        {
            if (!serialize_recursive(*this, buffer))
                return false;
        }
        else
            for (const auto &child : children())
                if (!serialize_recursive(child, buffer))
                    return false;

        return true;
    }

    bool tree_node<ka::tlv>::deserialize(const std::vector<uint8_t> &buffer) const
    {
        (void)buffer;

        return false;
    }
};

// refrence implementation of C version

// static bool tp_tlv_tree_deserialize_tag(const uint8_t **buffer, size_t *remaining_size, tp_tlv_tag_t *tag)
// {
//     if (!*remaining_size)
//         return false;

//     size_t index = 0;
//     tp_tlv_tag_t _tag = (*buffer)[index++];

//     if (!tag)
//         return false;

//     if ((_tag & 0b00011111) != 0b00011111)
//     {
//         (*buffer) += index;
//         *remaining_size -= index;
//         *tag = _tag;

//         return true;
//     }

//     do
//     {
//         if ((index >= *remaining_size) || index >= sizeof(tp_tlv_tag_t))
//             return false;

//         _tag = (_tag << 8) | (*buffer)[index++];
//     } while (_tag & 0b10000000);

//     (*buffer) += index;
//     *remaining_size -= index;
//     *tag = _tag;

//     return true;
// }

// static bool tp_tlv_tree_deserialize_length(const uint8_t **buffer, size_t *remaining_size, tp_tlv_length_t *length)
// {
//     if (!*remaining_size)
//         return false;

//     size_t index = 0;
//     tp_tlv_length_t _length = (*buffer)[index++];

//     if (!(_length & 0b10000000))
//     {
//         (*buffer) += 1;
//         *remaining_size -= 1;
//         *length = _length & 0b01111111;

//         return true;
//     }

//     uint8_t length_of_length = _length & 0b01111111;

//     _assert(length_of_length, "[common::tlv_tree::deserialize_length] deserializing length in indefinite form is not supported.");

//     if ((length_of_length + 1 > *remaining_size) || (length_of_length > sizeof(tp_tlv_length_t)))
//         return false;

//     _length = 0;

//     for (size_t i = 0; i < length_of_length; i++)
//         _length = (_length << 8) | (*buffer)[index++];

//     (*buffer) += index;
//     *remaining_size -= index;
//     *length = _length;

//     return true;
// }

// static bool tp_tlv_tree_deserialize_recursive(const uint8_t **buffer, size_t *remaining_size, tp_tlv_tree_t *tree, tp_tlv_tree_node_t *node)
// {
//     while (*remaining_size)
//     {
//         tp_tlv_tag_t tag = 0;

//         if (!tp_tlv_tree_deserialize_tag(buffer, remaining_size, &tag))
//             return false;

//         tp_tlv_length_t length = 0;

//         if (!tp_tlv_tree_deserialize_length(buffer, remaining_size, &length))
//             return false;

//         if (length > *remaining_size)
//             return false;

//         if (tp_tlv_tree_tag_is_primitive(tag))
//         {
//             tp_tlv_tree_add_node(tree, node, tag, length, *buffer);

//             (*buffer) += length;
//             *remaining_size -= length;
//         }
//         else
//         {
//             tp_tlv_tree_node_t *child = tp_tlv_tree_add_node(tree, node, tag, 0, NULL);

//             size_t _remaining_size = length;

//             if (!tp_tlv_tree_deserialize_recursive(buffer, &_remaining_size, tree, child))
//                 return false;

//             *remaining_size -= length;
//         }
//     }

//     return true;
// }

// bool tp_tlv_tree_deserialize(const uint8_t *buffer, const size_t size, tp_tlv_tree_t *tree, tp_tlv_tree_node_t *node)
// {
//     if (!buffer || !tree)
//         return false;

//     bool is_root_injected = false;

//     if (!node)
//     {
//         node = tp_tlv_tree_add_node(tree, tp_tlv_tree_get_root(tree), 0, 0, NULL);

//         is_root_injected = true;
//     }

//     size_t remaining_size = size;

//     if (!tp_tlv_tree_deserialize_recursive(&buffer, &remaining_size, tree, node))
//         return false;

//     if (is_root_injected)
//     {
//         tp_tree_t *_tree = tree;
//         tp_tree_node_t *_node = node;

//         if (tp_list_size(_node->children) < 2)
//         {
//             tp_list_node_t *child = _node->children->first;

//             if (child)
//             {
//                 _tree->root = child->data;
//                 _tree->root->parent = NULL;
//             }
//             else
//                 _tree->root = NULL;

//             _tree->mem_deallocator(_node->data);
//             tp_list_destroy(_node->children);

//             _tree->mem_deallocator(_node);
//         }
//     }

//     return true;
// }