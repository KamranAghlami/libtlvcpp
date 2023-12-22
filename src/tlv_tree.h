#pragma once

#include <vector>

#include "tree.h"
#include "tlv.h"

namespace ka
{
    template <>
    class tree_node<tlv> : public _tree_node<tlv>
    {
    public:
        bool serialize(std::vector<uint8_t> &buffer) const
        {
            (void)buffer;

            return false;
        }

        bool deserialize(const std::vector<uint8_t> &buffer) const
        {
            (void)buffer;

            return false;
        }
    };
}

// refrence implementation of C version

// static size_t tp_tlv_tree_serialize_length_of_tag(const tp_tlv_tag_t tag)
// {
//     union
//     {
//         tp_tlv_tag_t tag;
//         uint8_t byte[sizeof(tp_tlv_tag_t)];
//     } tag_bytes;

//     tag_bytes.tag = tag;

//     size_t length = 1;

//     while (tag_bytes.byte[length])
//     {
//         length++;

//         if (length > sizeof(tp_tlv_tag_t))
//         {
//             length = sizeof(tp_tlv_tag_t);

//             break;
//         }
//     }

//     return length;
// }

// static size_t tp_tlv_tree_serialize_length_of_length(const tp_tlv_length_t length)
// {
//     size_t length_of_length = 1;

//     if (length < 128)
//         return length_of_length;

//     union
//     {
//         tp_tlv_length_t length;
//         uint8_t byte[sizeof(tp_tlv_length_t)];
//     } length_bytes;

//     length_bytes.length = length;

//     while (length_bytes.byte[length_of_length])
//     {
//         length_of_length++;

//         if (length_of_length > sizeof(tp_tlv_tag_t))
//         {
//             length_of_length = sizeof(tp_tlv_tag_t);

//             break;
//         }
//     }

//     return length_of_length + 1;
// }

// static tp_tlv_length_t tp_tlv_tree_serialize_node_length_recursive(const tp_tlv_tree_node_t *node)
// {
//     const tp_tlv_t *tlv = tp_tlv_tree_node_get_data(node);
//     size_t size = 0;

//     size += tp_tlv_tree_serialize_length_of_tag(tlv->tag);
//     size += tp_tlv_tree_serialize_length_of_length(tlv->length);

//     if (tp_tlv_tree_tag_is_primitive(tlv->tag))
//     {
//         size += tlv->length;

//         return size;
//     }

//     const tp_tree_node_t *_node = node;

//     TP_LIST_RANGELOOP(child_node, _node->children)
//     size += tp_tlv_tree_serialize_node_length_recursive(child_node->data);

//     return size;
// }

// static tp_tlv_length_t tp_tlv_tree_serialize_node_length(const tp_tlv_tree_node_t *node)
// {
//     const tp_tlv_t *tlv = tp_tlv_tree_node_get_data(node);

//     if (tp_tlv_tree_tag_is_primitive(tlv->tag))
//         return tlv->length;

//     const tp_tree_node_t *_node = node;
//     size_t size = 0;

//     TP_LIST_RANGELOOP(child_node, _node->children)
//     size += tp_tlv_tree_serialize_node_length_recursive(child_node->data);

//     return size;
// }

// static bool tp_tlv_tree_serialize_tag(const tp_tlv_tag_t tag, uint8_t **buffer, size_t *remaining_size)
// {
//     if (!tag)
//         return false;

//     size_t length = tp_tlv_tree_serialize_length_of_tag(tag);

//     if (length > *remaining_size)
//         return false;

//     union
//     {
//         tp_tlv_tag_t tag;
//         uint8_t byte[sizeof(tp_tlv_tag_t)];
//     } tag_bytes;

//     tag_bytes.tag = tag;

//     for (ssize_t i = length - 1; i > -1; i--)
//     {
//         **buffer = tag_bytes.byte[i];

//         (*buffer)++;
//         (*remaining_size)--;
//     }

//     return true;
// }

// static bool tp_tlv_tree_serialize_length(const tp_tlv_length_t length, uint8_t **buffer, size_t *remaining_size)
// {
//     if (length < 0b01111111)
//     {
//         if (*remaining_size < 1)
//             return false;

//         **buffer = length;

//         (*buffer) += 1;
//         *remaining_size -= 1;

//         return true;
//     }

//     union
//     {
//         tp_tlv_length_t length;
//         uint8_t byte[sizeof(tp_tlv_length_t)];
//     } length_bytes;

//     length_bytes.length = length;

//     uint8_t length_of_length = sizeof(tp_tlv_length_t);

//     for (ssize_t i = sizeof(tp_tlv_length_t) - 1; i > -1; i--)
//         if (!length_bytes.byte[i])
//             length_of_length--;
//         else
//             break;

//     if ((length_of_length + 1 > *remaining_size) || (length_of_length > sizeof(tp_tlv_length_t)))
//         return false;

//     ssize_t index = 0;

//     (*buffer)[index++] = length_of_length | 0b10000000;

//     ssize_t byte_index = length_of_length - 1;

//     while (byte_index > -1)
//         (*buffer)[index++] = length_bytes.byte[byte_index--];

//     (*buffer) += (length_of_length + 1);
//     *remaining_size -= (length_of_length + 1);

//     return true;
// }

// static bool tp_tlv_tree_serialize_value(const uint8_t *value, const size_t size, uint8_t **buffer, size_t *remaining_size)
// {
//     if (size)
//     {
//         if (size > *remaining_size)
//             return false;

//         memcpy(*buffer, value, size);

//         (*buffer) += size;
//         *remaining_size -= size;
//     }

//     return true;
// }

// static bool tp_tlv_tree_serialize_recursive(const tp_tlv_tree_node_t *node, uint8_t **buffer, size_t *remaining_size)
// {
//     const tp_tlv_t *tlv = tp_tlv_tree_node_get_data(node);

//     if (!tp_tlv_tree_serialize_tag(tlv->tag, buffer, remaining_size))
//         return false;

//     if (!tp_tlv_tree_serialize_length(tp_tlv_tree_serialize_node_length(node), buffer, remaining_size))
//         return false;

//     if (tp_tlv_tree_tag_is_primitive(tlv->tag))
//     {
//         if (!tp_tlv_tree_serialize_value(tlv->value, tlv->length, buffer, remaining_size))
//             return false;
//     }
//     else
//     {
//         const tp_tree_node_t *_node = node;

//         TP_LIST_RANGELOOP(child_node, _node->children)
//         if (!tp_tlv_tree_serialize_recursive(child_node->data, buffer, remaining_size))
//             return false;
//     }

//     return true;
// }

// bool tp_tlv_tree_serialize(const tp_tlv_tree_node_t *node, uint8_t *buffer, const size_t size, size_t *serialized_size)
// {
//     if (!buffer || !size)
//         return false;

//     size_t remaining_size = size;

//     if (node)
//     {
//         if (tp_tlv_tree_node_get_data(node)->tag)
//         {
//             if (!tp_tlv_tree_serialize_recursive(node, &buffer, &remaining_size))
//                 return false;
//         }
//         else
//         {
//             const tp_tree_node_t *_node = node;

//             TP_LIST_RANGELOOP(child_node, _node->children)
//             if (!tp_tlv_tree_serialize_recursive(child_node->data, &buffer, &remaining_size))
//                 return false;
//         }
//     }

//     if (serialized_size)
//         *serialized_size = size - remaining_size;

//     return true;
// }

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

// bool tp_tlv_tree_tag_is_primitive(tp_tlv_tag_t tag)
// {
//     union
//     {
//         tp_tlv_tag_t tag;
//         uint8_t byte[sizeof(tp_tlv_tag_t)];
//     } tag_bytes;

//     tag_bytes.tag = tag;

//     for (ssize_t i = sizeof(tp_tlv_tag_t) - 1; i > -1; i--)
//         if (tag_bytes.byte[i])
//             return !(tag_bytes.byte[i] & 0b00100000);

//     return false;
// }