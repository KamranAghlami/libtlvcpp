#include "tlv_tree.h"
#include "utilities/hexdump.h"

int main()
{
    ka::tlv_tree_node tlvt(0b00100000U | 0x00);

    auto &n1 = tlvt.add_child(0b00100000U | 0x01);
    tlvt.add_child(0x02);
    auto &n3 = tlvt.add_child(0b00100000U | 0x03);

    n1.add_child(0x04);
    n1.add_child(0x05);

    n3.add_child(0x06);
    auto &n7 = n3.add_child(0b00100000U | 0x07);

    n7.add_child(0x08);
    n7.add_child(0x09);
    n7.add_child(0x0A);

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