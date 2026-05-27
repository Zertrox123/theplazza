#include "ipc.hpp"
#include <iostream>

int slave() {
    IPC ipc;
    for (int i = 1; i != 4; i++)
        ipc.init_slave(i);
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
    std::string input_user;
    std::regex pattern(R"(\s*([a-zA-Z]+)\s+(S|M|L|XL|XXL)\s+x([1-9][0-9]*)\s*)");

    if (!ipc.init_master()) {
        slave();
        return 0;
    }
    while (true) {
        std::cout << "$> ";
        std::getline(std::cin, input_user);
        std::stringstream ss(input_user);
        std::string order;
        while (std::getline(ss, order, ';')) {
            std::smatch match;
            if (std::regex_match(order, match, pattern)) {
                std::string pizza_name = match[1].str();
                std::string pizza_size = match[2].str();
                std::string pizza_count = match[3].str().substr(1);
                ipc.send_order(1, 1, std::vector<std::string>{pizza_name, pizza_size, pizza_count});
            } else {
                std::cout << "Format invalide: " << order << "\n";
            }
        }
    }
}
