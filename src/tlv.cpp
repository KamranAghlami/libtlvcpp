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

    tlv::tlv(const tlv &other) : tlv(other.m_tag, other.m_length, other.m_value)
    {
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

    tlv::tlv(tlv &&other) noexcept : tlv(std::move(other.m_tag), std::move(other.m_length), std::move(other.m_value))
    {
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
            stream << "\n";

            hexdump(tlv.m_value, tlv.m_length, width + 4, stream);
        }
        else
            stream << " " << nullptr;

        return stream;
    }
}