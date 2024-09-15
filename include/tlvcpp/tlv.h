#pragma once

#include <cstdint>
#include <cstring>
#include <ostream>
#include <memory>
#include <iomanip>

#include "limits"
#include "tlvcpp/utilities/hexdump.h"
#include "tlvcpp/utilities/stream_guard.h"

namespace tlvcpp
{
    using tag_t = uint32_t;
    using length_t = uint32_t;
    using value_t = uint8_t;
    using tag_parser = const char *(*)(const tag_t);

    extern tag_parser g_tag_parser;

    void set_tag_parser(tag_parser parser);
    bool tag_is_primitive(tag_t tag);

    /*
     * TODO:
     *  * Deal with:
     *       template <typename T>
     *       tlv(const tag_t &tag, const T &type, const allocator &alloc = allocator());
     *
     *  * Deciding what happens with statefull allocators when copying or moving...
     *      https://en.cppreference.com/w/cpp/named_req/AllocatorAwareContainer
     *      https://en.cppreference.com/w/cpp/memory/allocator_traits
     */

    template <typename allocator = std::allocator<value_t>>
    class tlv
    {
    public:
        tlv(const tag_t &tag = 0, const length_t &length = 0, const value_t *value = nullptr, const allocator &alloc = allocator());

        template <typename T>
        tlv(const tag_t &tag, const T &type, const allocator &alloc = allocator());

        ~tlv();

        tlv(const tlv &other);
        tlv &operator=(const tlv &other);

        tlv(tlv &&other) noexcept;
        tlv &operator=(tlv &&other) noexcept;

        template <typename a>
        friend std::ostream &operator<<(std::ostream &stream, const tlv<a> &tlv);

        template <typename a>
        friend bool operator==(const tlv<a> &tlv, const tag_t &tag);

        tag_t tag() const
        {
            return m_tag;
        }

        length_t length() const
        {
            return m_length;
        }

        const value_t *value() const
        {
            return m_value;
        }

    private:
        tag_t m_tag = 0;
        length_t m_length = 0;
        value_t *m_value = nullptr;
        allocator m_alloc;
    };

    template <typename allocator>
    tlv<allocator>::tlv(const tag_t &tag, const length_t &length, const value_t *value, const allocator &alloc)
        : m_tag(tag), m_length(length), m_value(nullptr), m_alloc(alloc)
    {
        if (m_length)
        {
            m_value = m_alloc.allocate(m_length);
            std::memcpy(m_value, value, m_length);
        }
    }

    template <typename allocator>
    tlv<allocator>::~tlv()
    {
        if (m_value)
            m_alloc.deallocate(m_value, m_length);
    }

    template <typename allocator>
    tlv<allocator>::tlv(const tlv &other)
        : m_tag(other.m_tag), m_length(other.m_length), m_value(nullptr), m_alloc(other.m_alloc)
    {
        if (m_length)
        {
            m_value = m_alloc.allocate(m_length);
            std::memcpy(m_value, other.m_value, m_length);
        }
    }

    template <typename allocator>
    tlv<allocator> &tlv<allocator>::operator=(const tlv &other)
    {
        if (&other == this)
            return *this;

        m_tag = other.m_tag;
        m_length = other.m_length;

        if (m_value)
            m_alloc.deallocate(m_value, m_length);

        if (m_length)
        {
            m_value = m_alloc.allocate(m_length);
            std::memcpy(m_value, other.m_value, m_length);
        }
        else
            m_value = nullptr;

        m_alloc = other.m_alloc;

        return *this;
    }

    template <typename allocator>
    tlv<allocator>::tlv(tlv &&other) noexcept
        : m_tag(std::move(other.m_tag)),
          m_length(std::move(other.m_length)),
          m_value(std::move(other.m_value)),
          m_alloc(std::move(other.m_alloc))
    {
        other.m_tag = 0;
        other.m_length = 0;
        other.m_value = nullptr;
    }

    template <typename allocator>
    tlv<allocator> &tlv<allocator>::operator=(tlv &&other) noexcept
    {
        if (&other == this)
            return *this;

        m_tag = std::move(other.m_tag);
        m_length = std::move(other.m_length);

        if (m_value)
            m_alloc.deallocate(m_value, m_length);

        m_value = std::move(other.m_value);
        m_alloc = std::move(other.m_alloc);

        other.m_tag = 0;
        other.m_length = 0;
        other.m_value = nullptr;

        return *this;
    }

    template <typename a>
    std::ostream &operator<<(std::ostream &stream, const tlv<a> &tlv)
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

    template <typename a>
    bool operator==(const tlv<a> &tlv, const tag_t &tag)
    {
        return tlv.m_tag == tag;
    }
}
