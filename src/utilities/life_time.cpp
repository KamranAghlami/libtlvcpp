#include "life_time.h"

#include <iostream>

namespace tlvcpp
{
    life_time::life_time(const std::string &name) : m_name(name)
    {
        std::cout << '[' << m_name << "] "
                  << "constructor" << '\n';
    }

    life_time::~life_time()
    {
        std::cout << '[' << m_name << "] "
                  << "destructor" << '\n';
    }

    life_time::life_time(const life_time &other) : m_name(other.m_name)
    {
        std::cout << '[' << m_name << "] "
                  << "copy constructor" << '\n';
    }

    life_time &life_time::operator=(const life_time &other)
    {
        std::cout << '[' << m_name << "] "
                  << "copy assignment" << '\n';

        if (&other == this)
            return *this;

        m_name = other.m_name;

        return *this;
    }

    life_time::life_time(life_time &&other) noexcept : m_name(other.m_name)
    {
        std::cout << '[' << m_name << "] "
                  << "move constructor" << '\n';
    }

    life_time &life_time::operator=(life_time &&other) noexcept
    {
        std::cout << '[' << m_name << "] "
                  << "move assignment" << '\n';

        if (&other == this)
            return *this;

        m_name = other.m_name;

        return *this;
    }
}