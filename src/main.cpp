#include "tlv_tree.h"
#include "utilities/hexdump.h"

int main()
{
    ka::tree_node<ka::tlv> tlvt;

    auto &l0 = tlvt;
    auto &l11 = l0.add_child(1U, "Hello, World!");
    auto &l12 = l0.add_child(2U, "0123456789abcdefABCDEF");

    l11.add_child(3U, "Hello, World!!!!");
    l12.add_child(4U, "A not so long sentence!");

    std::cout << "tree:\n";
    tlvt.dump();

    std::vector<uint8_t> buffer;
    tlvt.serialize(buffer);

    std::cout << "serialized:\n";
    ka::hexdump(buffer.data(), buffer.size());
}