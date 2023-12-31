#pragma once

#include <iostream>

namespace tlvcpp
{
    void hexdump(const void *data, const size_t &size, const size_t &indentation = 1, std::ostream &stream = std::cout);
}