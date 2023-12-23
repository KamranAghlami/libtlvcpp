#include "tlv_tree.h"
#include "utilities/hexdump.h"

int main()
{
    ka::tree_node<ka::tlv> tlvt;

    auto &l11 = tlvt.add_child(0b00100001U);
    auto &l12 = tlvt.add_child(0b00100010U);

    l11.add_child(1U, "Hello, world!");
    l11.add_child(2U, "0123456789abcdefABCDEF");

    l12.add_child(3U, "Hello again, world!");
    l12.add_child(4U, "A not so long sentence!");

    std::cout << "tree:\n";
    tlvt.dump();

    std::vector<uint8_t> buffer;
    tlvt.serialize(buffer);

    std::cout << "serialized:\n";
    ka::hexdump(buffer.data(), buffer.size());
}