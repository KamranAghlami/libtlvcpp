#include <gtest/gtest.h>

#include <limits>
#include <new>

#include "tlvcpp/tlv_tree.h"

template <class T>
struct test_allocator
{
    typedef T value_type;

    test_allocator() = default;

    template <class U>
    constexpr test_allocator(const test_allocator<U> &) noexcept {}

    [[nodiscard]] T *allocate(std::size_t n)
    {
        if (n > std::numeric_limits<std::size_t>::max() / sizeof(T))
            throw std::bad_array_new_length();

        if (auto p = static_cast<T *>(std::malloc(n * sizeof(T))))
        {
            report(p, n, true);

            return p;
        }

        throw std::bad_alloc();
    }

    void deallocate(T *p, std::size_t n) noexcept
    {
        report(p, n, false);

        std::free(p);
    }

private:
    void report(T *p, std::size_t n, bool alloc) const
    {
        tlvcpp::stream_guard guard{std::cout};

        std::cout << (alloc ? " + " : " - ") << sizeof(T) * n
                  << " bytes at " << std::hex << std::showbase
                  << reinterpret_cast<void *>(p) << std::dec << '\n';
    }
};

template <class T, class U>
bool operator==(const test_allocator<T> &, const test_allocator<U> &) { return true; }

template <class T, class U>
bool operator!=(const test_allocator<T> &, const test_allocator<U> &) { return false; }

class decode_encode : public ::testing::TestWithParam<std::tuple<const char *, size_t>>
{
protected:
    void SetUp() override
    {
        const char *buffer = std::get<0>(GetParam());
        const size_t size = std::get<1>(GetParam());

        deserialized = std::vector<uint8_t>(buffer, buffer + size);
    }

    void TearDown() override
    {
    }

    std::vector<uint8_t> deserialized;
    tlvcpp::tlv_tree_node<test_allocator<tlvcpp::value_t>> m_tlv_tree;
};

TEST_P(decode_encode, buffer2vector)
{
    std::vector<uint8_t> serialized;

    ASSERT_TRUE(tlvcpp::deserialize(deserialized.data(), deserialized.size(), m_tlv_tree));
    ASSERT_TRUE(tlvcpp::serialize(m_tlv_tree, serialized));
    ASSERT_EQ(serialized.size(), deserialized.size());
    ASSERT_EQ(memcmp(serialized.data(), deserialized.data(), deserialized.size()), 0);
}

TEST_P(decode_encode, vector2vector)
{
    std::vector<uint8_t> serialized;

    ASSERT_TRUE(tlvcpp::deserialize(deserialized, m_tlv_tree));
    ASSERT_TRUE(tlvcpp::serialize(m_tlv_tree, serialized));
    ASSERT_EQ(serialized.size(), deserialized.size());
    ASSERT_EQ(memcmp(serialized.data(), deserialized.data(), deserialized.size()), 0);
}

std::vector<std::tuple<const char *, size_t>> cases = {
    {
        "",
        0,
    },
    {
        "\x6F\x1A\x84\x0E\x31\x50\x41\x59\x2E\x53\x59\x53\x2E\x44\x44\x46"
        "\x30\x31\xA5\x08\x88\x01\x02\x5F\x2D\x02\x65\x6E",
        28,
    },
    {
        "\x7F\x2D\x1B\x84\x0E\x31\x50\x41\x59\x2E\x53\x59\x53\x2E\x44\x44\x46"
        "\x30\x31\x7F\x2D\x08\x88\x01\x02\x5F\x2D\x02\x65\x6E",
        30,
    },
    {
        "\x5F\x2A\x02\x01\x24\x5F\x34\x01\x01\x82\x02\x1C\x00\x84\x07\xA0"
        "\x00\x00\x00\x03\x10\x10\x95\x05\x80\x00\x00\x00\x00\x9A\x03\x11"
        "\x02\x24\x9B\x02\x68\x00\x9C\x01\x00\x9F\x02\x06\x00\x00\x00\x00"
        "\x00\x00\x9F\x03\x06\x00\x00\x00\x00\x00\x00\x9F\x06\x07\xA0\x00"
        "\x00\x00\x03\x10\x10\x9F\x08\x02\x00\x8C\x9F\x09\x02\x00\x8C\x9F"
        "\x10\x07\x06\x01\x0A\x03\x90\x00\x00\x9F\x1A\x02\x01\x24\x9F\x26"
        "\x08\x42\x31\x58\x93\x6E\xD6\xC3\x8F\x9F\x27\x01\x80\x9F\x33\x03"
        "\xE0\xB0\xC8\x9F\x34\x03\x41\x03\x02\x9F\x35\x01\x22\x9F\x36\x02"
        "\x00\x01\x9F\x37\x04\xAC\xAC\x66\xE8\x9F\x58\x00\xDF\x01\x00\xDF"
        "\x02\x00\xDF\x04\x00",
        149,
    },
    {
        "\x9F\x26\x08\x07\x99\x6A\x19\x96\x7C\x3C\x73\x95\x05\x00\x80\x00"
        "\x00\x00\x9F\x33\x03\xE0\xF0\xC0\x9F\x36\x02\x00\x92\x82\x02\x39"
        "\x00\x84\x07\xA0\x00\x00\x00\x04\x10\x10\x9F\x37\x04\xD5\xCB\x3A"
        "\x44\x9F\x34\x03\x44\x03\x02\x9F\x35\x01\x22\x9F\x10\x12\x08\x10"
        "\x97\x80\x03\x02\x00\x00\xA7\xEB\x00\x00\x00\x00\x15\x00\x00\xFF"
        "\x9F\x27\x01\x40\x9A\x03\x22\x12\x23\x9B\x02\xE8\x00\x9C\x01\x00"
        "\x5F\x2A\x02\x09\x49\x5F\x34\x01\x01\x9F\x02\x06\x00\x00\x00\x00"
        "\x10\x00\x9F\x03\x06\x00\x00\x00\x00\x00\x00\x9F\x09\x02\x00\x00"
        "\x9F\x1A\x02\x07\x92\x9F\x41\x04\x00\x00\x01\x28\x9F\x1E\x08\x30"
        "\x30\x30\x30\x30\x30\x30\x33\x9F\x53\x01\x52",
        155,
    },
};

INSTANTIATE_TEST_SUITE_P(Parameterized, decode_encode, testing::ValuesIn(cases));
