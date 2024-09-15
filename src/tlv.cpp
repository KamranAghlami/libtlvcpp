#include "tlvcpp/tlv.h"

namespace tlvcpp
{
    tag_parser g_tag_parser = nullptr;

    void set_tag_parser(tag_parser parser)
    {
        g_tag_parser = parser;
    }

    bool tag_is_primitive(tag_t tag)
    {
        union
        {
            tag_t tag;
            uint8_t byte[sizeof(tag_t)];
        } tag_bytes;

        tag_bytes.tag = tag;

        for (size_t i = sizeof(tag_t) - 1; i != static_cast<std::size_t>(-1); i--)
            if (tag_bytes.byte[i])
                return !(tag_bytes.byte[i] & 0b00100000);

        return false;
    }
}