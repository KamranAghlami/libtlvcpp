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
        bool serialize(std::vector<uint8_t> &buffer) const;
        bool deserialize(const std::vector<uint8_t> &buffer) const;
    };
}