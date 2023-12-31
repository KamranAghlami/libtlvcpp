#include "utilities/hexdump.h"
#include "utilities/scoped_timer.h"
#include "tlvcpp/tlv_tree.h"

int main()
{
    tlvcpp::tlv_tree_node tlvt(0b00100000U | 0x00U, "This is the root node.");

    // auto &n1 = tlvt.add_child(0b00100000U | 0x01U);
    // tlvt.add_child(0x02U, "There's a pattern as you see...");
    auto &n3 = tlvt.add_child(0b00100000U | 0x03U);

    // n1.add_child(0x04U, "According to the tlv standard...");
    // n1.add_child(0x05U, "Nodes that have children...");

    n3.add_child(0x06U, "They should have 6'th bit set in the highest byte.");
    auto &n7 = n3.add_child(0b00100000U | 0x07U);

    n7.add_child(0x08U, "The serializer and deserializer depend on this.");
    n7.add_child(0x09U, "So they either have some data or children, not both.");
    // n7.add_child(0x0AU, "That's why the root node's data is missing in the serialized/deserialized output.");

    std::cout << "tree:\n";
    tlvt.dump();

    std::vector<uint8_t> buffer;
    bool serialized = false;

    {
        scoped_timer t("serialize");

        serialized = tlvt.serialize(buffer);
    }

    if (serialized)
    {
        std::cout << "serialized:\n";
        tlvcpp::hexdump(buffer.data(), buffer.size());
    }
    else
        std::cout << "serialization failed!\n";

    tlvcpp::tlv_tree_node tlvt2;
    bool deserialized = false;

    {
        scoped_timer t("deserialize");

        deserialized = tlvt2.deserialize(buffer);
    }

    if (deserialized)
    {
        std::cout << "deserialized:\n";
        tlvt2.dump();
    }
    else
        std::cout << "deserialization failed!\n";
}
