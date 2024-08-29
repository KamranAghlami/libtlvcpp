#pragma once

#include <cstdint>
#include <ostream>

namespace tlvcpp
{
    using tag_t = uint32_t;
    using length_t = uint32_t;
    using value_t = uint8_t;
    using tag_parser = const char *(*)(const tag_t);

    void set_tag_parser(tag_parser parser);
    bool tag_is_primitive(tag_t tag);

    class tlv
    {
    public:
        tlv(const tag_t &tag = 0, const length_t &length = 0, const value_t *value = nullptr);

        template <typename T>
        tlv(const tag_t &tag, const T &&type);

        explicit tlv(const tag_t &tag, const char *string);

        ~tlv();

        tlv(const tlv &other);
        tlv &operator=(const tlv &other);

        tlv(tlv &&other) noexcept;
        tlv &operator=(tlv &&other) noexcept;

        friend std::ostream &operator<<(std::ostream &stream, const tlv &tlv);
        friend bool operator==(const tlv &tlv, const tag_t &tag);

        tag_t tag() const { return m_tag; }
        length_t length() const { return m_length; }
        const value_t *value() const { return m_value; }

    private:
        tag_t m_tag = 0;
        length_t m_length = 0;
        value_t *m_value = nullptr;
    };
}
