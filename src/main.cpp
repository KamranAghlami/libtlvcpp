#include "tlv_tree.h"
#include "utilities/hexdump.h"

int main()
{
    ka::tlv_tree_node tlvt(0b00100000U | 0x00U);

    auto &n1 = tlvt.add_child(0b00100000U | 0x01U);
    tlvt.add_child(0x02U);
    auto &n3 = tlvt.add_child(0b00100000U | 0x03U);

    n1.add_child(0x04U);
    n1.add_child(0x05U);

    n3.add_child(0x06U);
    auto &n7 = n3.add_child(0b00100000U | 0x07U);

    n7.add_child(0x08U);
    n7.add_child(0x09U);
    n7.add_child(0x0AU);

    std::cout << "tree:\n";
    tlvt.dump();

    std::vector<uint8_t> buffer;
    tlvt.serialize(buffer);

    std::cout << "serialized:\n";
    ka::hexdump(buffer.data(), buffer.size());

    ka::tlv_tree_node tlvt2;

    tlvt2.deserialize(buffer);

    std::cout << "deserialized:\n";
    tlvt2.dump();
}