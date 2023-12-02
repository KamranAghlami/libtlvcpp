#pragma once

#include <iostream>

namespace ka
{
    void hexdump(const void *data, const size_t &size, const size_t &indentation = 0, std::ostream &stream = std::cout);
}