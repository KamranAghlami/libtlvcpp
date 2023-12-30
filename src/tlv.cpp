#include "tlv.h"

#include <cstring>

#include "limits"
#include "utilities/hexdump.h"
#include "utilities/iostream_state_guard.h"

namespace ka
{
    tlv::tlv(const tag_t &tag, const length_t &length, const value_t *value) : /** life_time(__func__ + std::string("_") + std::to_string(tag)), */
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
    tlv::tlv(const tag_t &tag, const std::string_view &&string) : tlv(
                                                                      tag,
                                                                      static_cast<length_t>(string.size()),
                                                                      reinterpret_cast<const value_t *>(string.data()))
    {
    }

    template <>
    tlv::tlv(const tag_t &tag, const std::string &&string) : tlv(tag, std::string_view{string})
    {
    }

    tlv::tlv(const tag_t &tag, const char *string) : tlv(tag, std::string_view{string})
    {
    }

    tlv::~tlv()
    {
        if (m_value)
            delete[] m_value;
    }

    tlv::tlv(const tlv &other) : /** life_time(other), */
                                 m_tag(other.m_tag),
                                 m_length(other.m_length),
                                 m_value(nullptr)
    {
        if (m_length)
        {
            m_value = new uint8_t[m_length];

            std::memcpy(m_value, other.m_value, m_length);
        }
    }

    tlv &tlv::operator=(const tlv &other)
    {
        if (&other == this)
            return *this;

        m_tag = other.m_tag;
        m_length = other.m_length;

        if (m_value)
            delete[] m_value;

        if (m_length)
        {
            m_value = new uint8_t[m_length];

            std::memcpy(m_value, other.m_value, m_length);
        }
        else
            m_value = nullptr;

        return *this;
    }

    tlv::tlv(tlv &&other) noexcept /** : life_time(std::move(other)) */
    {
        m_tag = std::move(other.m_tag);
        m_length = std::move(other.m_length);
        m_value = std::move(other.m_value);

        other.m_tag = 0;
        other.m_length = 0;
        other.m_value = nullptr;
    }

    tlv &tlv::operator=(tlv &&other) noexcept
    {
        if (&other == this)
            return *this;

        m_tag = std::move(other.m_tag);
        m_length = std::move(other.m_length);

        if (m_value)
            delete[] m_value;

        m_value = std::move(other.m_value);

        other.m_tag = 0;
        other.m_length = 0;
        other.m_value = nullptr;

        return *this;
    }

    std::ostream &operator<<(std::ostream &stream, const tlv &tlv)
    {
        const auto width = static_cast<size_t>(stream.width());

        {
            iostream_state_guard state_guard{stream};

            stream << ""
                   << "* tag: 0x"
                   << std::hex << std::setw(2) << std::setfill('0')
                   << tlv.m_tag;
        }

        stream << ", length: " << tlv.m_length << ", value:";

        if (tlv.m_length)
        {
            stream << '\n';

            hexdump(tlv.m_value, tlv.m_length, width + 4, stream);
        }
        else
            stream << " " << nullptr << '\n';

        return stream;
    }

    bool operator==(const tlv &tlv, const tag_t &tag)
    {
        return tlv.m_tag == tag;
    }
}