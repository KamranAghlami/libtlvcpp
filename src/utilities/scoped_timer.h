#pragma once

#include <chrono>
#include <iostream>

class scoped_timer
{
private:
    const std::string m_name;
    const std::chrono::system_clock::time_point m_start;

public:
    scoped_timer(const std::string &name = "") : m_name(name),
                                                 m_start(std::chrono::high_resolution_clock::now())
    {
    }

    ~scoped_timer()
    {
        const auto end = std::chrono::high_resolution_clock::now();
        const auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - m_start);
        const auto microseconds = duration.count();

        std::cout << "[scoped_timer::" << m_name.c_str() << "] " << microseconds << "us" << '\n';
    }
};