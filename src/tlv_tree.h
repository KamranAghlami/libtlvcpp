#pragma once

#include "tree.h"
#include "tlv.h"

namespace ka
{
    template <>
    class tree_node<tlv> : public _tree_node<tlv>
    {
    public:
        bool serialize()
        {
            return false;
        }

        bool deserialize()
        {
            return false;
        }
    };
}