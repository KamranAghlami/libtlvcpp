#pragma once

#include <cstdint>
#include <ostream>

#include "utilities/life_time.h"

namespace ka
{
    using tag = uint32_t;
    using length = uint32_t;
    using value = uint8_t;

    class tlv : private life_time
    {
    public:
        tlv(const tag &tag = 0, const length &length = 0, const value *value = nullptr);

        template <typename T>
        tlv(const tag &tag, const T &&type);

        explicit tlv(const tag &tag, const char *string);

        ~tlv();

        tlv(const tlv &other);
        tlv &operator=(const tlv &other);

        tlv(tlv &&other) noexcept;
        tlv &operator=(tlv &&other) noexcept;

        friend std::ostream &operator<<(std::ostream &stream, const tlv &tlv);

    private:
        tag m_tag;
        length m_length;
        value *m_value;
    };
}