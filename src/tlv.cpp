#include "tlv.h"

#include <cstring>

#include "utilities/iostream_state_guard.h"
#include "utilities/hexdump.h"
#include "limits"

namespace ka
{
    tlv::tlv(const tag &tag, const length &length, const value *value) : life_time(__func__),
                                                                         m_tag(tag),
                                                                         m_length(length),
                                                                         m_value(nullptr)
    {
        if (m_length)
        {
            m_value = new uint8_t[m_length];

            std::memcpy(m_value, value, m_length);
        }
    }

    template <>
    tlv::tlv(const tag &tag, const std::string_view &&string) : tlv(
                                                                    tag,
                                                                    static_cast<length>(string.size()),
                                                                    reinterpret_cast<const value *>(string.data()))
    {
    }

    template <>
    tlv::tlv(const tag &tag, const std::string &&string) : tlv(tag, std::string_view{string})
    {
    }

    tlv::tlv(const tag &tag, const char *string) : tlv(tag, std::string_view{string})
    {
    }

    tlv::~tlv()
    {
        if (m_value)
            delete[] m_value;
    }

    std::ostream &operator<<(std::ostream &stream, const tlv &tlv)
    {
        {
            iostream_state_guard state_guard{stream};

            stream << "tag: 0x" << std::hex << std::setw(2) << std::setfill('0') << tlv.m_tag;
        }

        stream << ", length: " << tlv.m_length << ", value:";

        if (tlv.m_length)
        {
            stream << "\n";

            hexdump(tlv.m_value, tlv.m_length, 2, stream);
        }
        else
            stream << " " << nullptr;

        return stream;
    }
}