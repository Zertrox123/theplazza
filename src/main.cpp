#include "ipc.hpp"
#include <iostream>

int slave() {
    IPC ipc;
    ipc.init_slave(1);
    while (true) {
        std::vector<Order> orders = ipc.get_orders();
        for (auto i : orders) {
            std::cout << "order: " << i.id << " " << i.unique_id << " args: " << std::endl;
            for (auto x : i.args) {
                std::cout << " - " << x << std::endl;
            }
            ipc.set_order_done(i);
        }
    }
    return 0;
}

int main() {
    IPC ipc;
    if (!ipc.init_master()) {
        slave();
        return 0;
    }
    while (ipc.send_order(1, 1, std::vector<std::string>{"arg 1","arg 2","XXL"}) != true) {}
    std::cout << "out" << std::endl;
    while (1) {}
}
