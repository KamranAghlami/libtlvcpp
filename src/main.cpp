#include "tlv_tree.h"
#include "utilities/hexdump.h"

int main()
{
    ka::tlv_tree_node tlvt(0b00100001U);

    // auto &l11 = tlvt.add_child(0b00100001U);
    // auto &l12 = tlvt.add_child(0b00100010U);

    tlvt.add_child(1U, "Hello, world!");
    tlvt.add_child(2U, "0123456789abcdefABCDEF");

    // l12.add_child(3U, "Hello again, world!");
    // l12.add_child(4U, "A not so long sentence!");

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