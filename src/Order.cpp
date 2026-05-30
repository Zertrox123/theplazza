#include "ipc.hpp"
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>
#include <vector>

std::vector<uint8_t> Order::serialize(Order o) {
    OrderHeader hdr;
    hdr.id = o.id;
    hdr.unique_id = o.unique_id;
    hdr.sender = o.sender;
    hdr.args_count = o.args.size();

    std::vector<uint8_t> buf(sizeof(OrderHeader));
    memcpy(buf.data(), &hdr, sizeof(hdr));
    for (auto s : o.args) {
        uint32_t len = s.size();
        size_t offset = buf.size();
        buf.resize(offset + sizeof(uint32_t) + len);
        memcpy(buf.data() + offset, &len, sizeof(uint32_t));
        memcpy(buf.data() + offset + sizeof(uint32_t), s.data(), len);
    }

    return buf;
};

Order Order::deserialize(std::vector<uint8_t> buf) {
    if (buf.size() < sizeof(OrderHeader))
        return Order {};

    OrderHeader hdr;
    memcpy(&hdr, buf.data(), sizeof(hdr));

    Order o;
    o.id        = hdr.id;
    o.unique_id = hdr.unique_id;
    o.sender    = hdr.sender;

    size_t padding = sizeof(OrderHeader);
    for (uint32_t i = 0; i < hdr.args_count; ++i) {
        if (padding + sizeof(uint32_t) > buf.size())
            std::cerr << "error parser " << i << std::endl;

        uint32_t len;
        memcpy(&len, buf.data() + padding, sizeof(uint32_t));
        padding += sizeof(uint32_t);

        if (padding + len > buf.size())
            std::cerr << "error parser " << i << std::endl;

        std::string a(reinterpret_cast<const char*>(buf.data() + padding), len);
        o.args.push_back(a);
        padding += len;
    }

    return o;
}
