#include "tlv_tree.h"

int main()
{
    ka::tlv_tree tlvt{};

    auto &l0 = tlvt.root();
    auto &l11 = l0.add_node(1, "Hello, World!");
    auto &l12 = l0.add_node(2, "0123456789abcdefABCDEF");
    auto &l21 = l11.add_node(3, "Hello, World!!!!");
    auto &l22 = l12.add_node(4, "A not so long sentence!");

    l0.dump();
}