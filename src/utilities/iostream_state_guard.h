#pragma once

#include <iomanip>
#include <iostream>

namespace ka
{
    template <typename stream_type>
    class iostream_state_guard
    {
    public:
        iostream_state_guard(stream_type &stream) : m_stream(stream),
                                                    m_flags(stream.flags()),
                                                    m_width(stream.width()),
                                                    m_precision(stream.precision()),
                                                    m_fill(stream.fill()) {}

        ~iostream_state_guard()
        {
            m_stream.flags(m_flags);
            m_stream.width(m_width);
            m_stream.precision(m_precision);
            m_stream.fill(m_fill);
        }

    private:
        stream_type &m_stream;
        const std::ios::fmtflags m_flags;
        const std::streamsize m_width;
        const std::streamsize m_precision;
        const char m_fill;
    };
}
