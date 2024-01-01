#pragma once

#include <string>

namespace tlvcpp
{
    class life_time
    {
    public:
        life_time(const std::string &name);
        ~life_time();

        life_time(const life_time &other);
        life_time &operator=(const life_time &other);

        life_time(life_time &&other) noexcept;
        life_time &operator=(life_time &&other) noexcept;

    private:
        std::string m_name;
    };
}