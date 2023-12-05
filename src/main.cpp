#include "tlv_tree.h"

int main()
{
    ka::tlv_tree tlvt;

    auto &l0 = tlvt.root();
    auto &l11 = l0.add_node(1U, "Hello, World!");
    auto &l12 = l0.add_node(2U, "0123456789abcdefABCDEF");

    l11.add_node(3U, "Hello, World!!!!");
    l12.add_node(4U, "A not so long sentence!");

    tlvt.dump();
}