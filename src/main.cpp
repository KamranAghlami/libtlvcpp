#include "utilities/hexdump.h"
#include "utilities/scoped_timer.h"
#include "tlv_tree.h"

int main()
{
    ka::tlv_tree_node tlvt(0b00100000U | 0x00U, "This is the root node.");

    auto &n1 = tlvt.add_child(0b00100000U | 0x01U);
    tlvt.add_child(0x02U, "There's a pattern as you see...");
    auto &n3 = tlvt.add_child(0b00100000U | 0x03U);

    n1.add_child(0x04U, "According to the tlv standard...");
    n1.add_child(0x05U, "Nodes that have children...");

    n3.add_child(0x06U, "They should have 6'th bit set in the highest byte.");
    auto &n7 = n3.add_child(0b00100000U | 0x07U);

    n7.add_child(0x08U, "The serializer and deserializer depend on this.");
    n7.add_child(0x09U, "So they either have some data or children, not both.");
    n7.add_child(0x0AU, "That's why the root node's data is missing in the serialized/deserialized output.");

    std::cout << "tree:\n";
    tlvt.dump();

    std::vector<uint8_t> buffer;

    {
        scoped_timer t("serialize");

        tlvt.serialize(buffer);
    }

    std::cout << "serialized:\n";
    ka::hexdump(buffer.data(), buffer.size());

    ka::tlv_tree_node tlvt2;

    {
        scoped_timer t("deserialize");

        tlvt2.deserialize(buffer);
    }

    std::cout << "deserialized:\n";
    tlvt2.dump();
}