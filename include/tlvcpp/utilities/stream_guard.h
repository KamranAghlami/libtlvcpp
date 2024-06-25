#pragma once

namespace tlvcpp
{
    template <typename T>
    class stream_guard
    {
    public:
        stream_guard(T &stream) : m_stream(stream),
                                  m_flags(stream.flags()),
                                  m_width(stream.width()),
                                  m_precision(stream.precision()),
                                  m_fill(stream.fill()) {}

        ~stream_guard()
        {
            m_stream.flags(m_flags);
            m_stream.width(m_width);
            m_stream.precision(m_precision);
            m_stream.fill(m_fill);
        }

    private:
        T &m_stream;
        const std::ios::fmtflags m_flags;
        const std::streamsize m_width;
        const std::streamsize m_precision;
        const char m_fill;
    };
}
