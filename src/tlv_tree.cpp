#include <assert.h>
#include <cstring>

#include "tlvcpp/tlv_tree.h"

namespace tlvcpp
{
    length_t length_of_tag(const tag_t tag)
    {
        union
        {
            tag_t tag;
            uint8_t byte[sizeof(tag_t)];
        } tag_bytes;

        tag_bytes.tag = tag;

        length_t length = 1;

        while (tag_bytes.byte[length])
        {
            length++;

            if (length > sizeof(tag_t))
            {
                length = sizeof(tag_t);

                break;
            }
        }

        return length;
    }

    length_t length_of_length(const length_t length)
    {
        length_t length_of_length = 1;

        if (length < 128)
            return length_of_length;

        union
        {
            length_t length;
            uint8_t byte[sizeof(length_t)];
        } length_bytes;

        length_bytes.length = length;

        while (length_bytes.byte[length_of_length])
        {
            length_of_length++;

            if (length_of_length > sizeof(length_t))
            {
                length_of_length = sizeof(length_t);

                break;
            }
        }

        return length_of_length + 1;
    }

    bool serialize_tag(const tag_t tag, std::vector<uint8_t> &buffer)
    {
        if (!tag)
            return false;

        length_t length = length_of_tag(tag);

        union
        {
            tag_t tag;
            uint8_t byte[sizeof(tag_t)];
        } tag_bytes;

        tag_bytes.tag = tag;

        for (size_t i = length - 1; i != static_cast<std::size_t>(-1); i--)
            buffer.push_back(tag_bytes.byte[i]);

        return true;
    }

    bool serialize_length(const length_t length, std::vector<uint8_t> &buffer)
    {
        if (length < 0b01111111)
        {
            buffer.push_back(static_cast<uint8_t>(length));

            return true;
        }

        union
        {
            length_t length;
            uint8_t byte[sizeof(length_t)];
        } length_bytes;

        length_bytes.length = length;

        uint8_t length_of_length = sizeof(length_t);

        for (size_t i = sizeof(length_t) - 1; i != static_cast<std::size_t>(-1); i--)
            if (!length_bytes.byte[i])
                length_of_length--;
            else
                break;

        if (length_of_length > sizeof(length_t))
            return false;

        buffer.push_back(length_of_length | 0b10000000);

        size_t byte_index = length_of_length - 1;

        while (byte_index != static_cast<std::size_t>(-1))
            buffer.push_back(length_bytes.byte[byte_index--]);

        return true;
    }

    bool serialize_value(const uint8_t *value, const size_t size, std::vector<uint8_t> &buffer)
    {
        if (size)
            buffer.insert(buffer.end(), value, value + size);

        return true;
    }

    bool deserialize_tag(const uint8_t *&buffer, size_t &remaining_size, tag_t &tag)
    {
        if (!remaining_size)
            return false;

        size_t index = 0;
        tag_t _tag = buffer[index++];

        if (!_tag)
            return false;

        if ((_tag & 0b00011111) != 0b00011111)
        {
            buffer += index;
            remaining_size -= index;
            tag = _tag;

            return true;
        }

        do
        {
            if ((index >= remaining_size) || index >= sizeof(tag_t))
                return false;

            _tag = (_tag << 8) | buffer[index++];
        } while (_tag & 0b10000000);

        buffer += index;
        remaining_size -= index;
        tag = _tag;

        return true;
    }

    bool deserialize_length(const uint8_t *&buffer, size_t &remaining_size, length_t &length)
    {
        if (!remaining_size)
            return false;

        size_t index = 0;
        length_t _length = buffer[index++];

        if (!(_length & 0b10000000))
        {
            buffer += 1;
            remaining_size -= 1;
            length = _length & 0b01111111;

            return true;
        }

        uint8_t length_of_length = _length & 0b01111111;

        assert(length_of_length);

        if ((length_of_length + 1U > remaining_size) || (length_of_length > sizeof(length_t)))
            return false;

        _length = 0;

        for (size_t i = 0; i < length_of_length; i++)
            _length = (_length << 8) | buffer[index++];

        buffer += index;
        remaining_size -= index;
        length = _length;

        return true;
    }

}
