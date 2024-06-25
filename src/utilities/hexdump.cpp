#include "tlvcpp/utilities/hexdump.h"

#include <cstdint>
#include <iomanip>

#include "tlvcpp/utilities/stream_guard.h"

namespace tlvcpp
{
    void hexdump(const void *data, const size_t &size, const size_t &indentation, std::ostream &stream)
    {
        auto _data = static_cast<const uint8_t *>(data);

        size_t i, j;
        uint8_t ascii[17];

        ascii[16] = '\0';

        for (i = 0; i < size; i++)
        {
            if (!(i % 16))
                stream << std::setw(static_cast<int>(indentation)) << "";

            {
                stream_guard guard{stream};

                stream << std::hex << std::uppercase << std::setw(2) << std::setfill('0') << (int)_data[i] << " ";
            }

            if (_data[i] >= ' ' && _data[i] <= '~')
                ascii[i % 16] = _data[i];
            else
                ascii[i % 16] = '.';

            if ((i + 1) % 8 == 0 || i + 1 == size)
            {
                stream << " ";

                if ((i + 1) % 16 == 0)
                    stream << "|  " << ascii << '\n';
                else if (i + 1 == size)
                {
                    ascii[(i + 1) % 16] = '\0';

                    if ((i + 1) % 16 <= 8)
                        stream << " ";

                    for (j = (i + 1) % 16; j < 16; ++j)
                        stream << "   ";

                    stream << "|  " << ascii << '\n';
                }
            }
        }
    }
}