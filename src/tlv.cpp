#include "tlvcpp/tlv.h"

#include <cstring>
#include <iomanip>

#include "limits"
#include "tlvcpp/utilities/hexdump.h"
#include "tlvcpp/utilities/stream_guard.h"

namespace tlvcpp
{
    static tag_parser g_tag_parser = nullptr;

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

    tlv::tlv(const tag_t &tag, const length_t &length, const value_t *value) : m_tag(tag),
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
    tlv::tlv(const tag_t &tag, const std::string &&string) : tlv(tag,
                                                                 static_cast<length_t>(string.size() + 1),
                                                                 reinterpret_cast<const value_t *>(string.c_str()))
    {
    }

    tlv::tlv(const tag_t &tag, const char *string) : tlv(tag,
                                                         static_cast<length_t>(strlen(string) + 1),
                                                         reinterpret_cast<const value_t *>(string))
    {
    }

    tlv::~tlv()
    {
        if (m_value)
            delete[] m_value;
    }

    tlv::tlv(const tlv &other) : m_tag(other.m_tag),
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

    tlv::tlv(tlv &&other) noexcept
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
            stream_guard guard{stream};

            stream << "";

            if (g_tag_parser)
            {
                stream << "* tag: "
                       << g_tag_parser(tlv.m_tag);
            }
            else
            {
                stream << "* tag: 0x"
                       << std::hex << std::setw(2) << std::setfill('0')
                       << tlv.m_tag;
            }
        }

        if (tlv.m_length)
        {
            stream << ", length: " << tlv.m_length << ", value:\n";

            hexdump(tlv.m_value, tlv.m_length, width + 4, stream);
        }
        else
            stream << '\n';

        return stream;
    }

    bool operator==(const tlv &tlv, const tag_t &tag)
    {
        return tlv.m_tag == tag;
    }
}