#pragma once

#include <utility>

namespace tlvcpp
{
    template <typename T>
    class scoped_pointer
    {
    public:
        template <typename... Args>
        scoped_pointer(Args &&...args) : m_ptr(new T(std::forward<Args>(args)...))
        {
        }

        ~scoped_pointer()
        {
            if (m_ptr)
                delete m_ptr;
        }

        scoped_pointer(const scoped_pointer &other) : m_ptr(new T(*other.m_ptr)){};
        scoped_pointer(scoped_pointer &other) : scoped_pointer(const_cast<const scoped_pointer &>(other)){};

        scoped_pointer &operator=(const scoped_pointer &other)
        {
            if (this == &other)
                return *this;

            *m_ptr = *other.m_ptr;

            return *this;
        };

        scoped_pointer(scoped_pointer &&other) noexcept : m_ptr(other.m_ptr)
        {
            other.m_ptr = nullptr;
        };

        scoped_pointer &operator=(scoped_pointer &&other) noexcept
        {
            if (this == &other)
                return *this;

            std::swap(m_ptr, other.m_ptr);

            return *this;
        };

        operator T *()
        {
            return m_ptr;
        }

        operator const T *() const
        {
            return m_ptr;
        }

    private:
        T *m_ptr;
    };
}
