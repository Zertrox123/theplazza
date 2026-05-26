#include "ipc.hpp"
#include <iostream>

int slave() {
    IPC ipc;
    ipc.init_slave(1);
    while (true) {
        //std::vector<Order> orders = ipc.get_orders();
        //for (auto i : orders) {
        //    std::cout << i.id << i.unique_id << std::endl;
        //}
    }
    return 0;
}

int main() {
    IPC ipc;
    if (!ipc.init_master()) {
        slave();
        return 0;
    }
    while (true) {
        ipc.send_order(1, 1, std::vector<std::string>{"","",""});
    
    }
}
