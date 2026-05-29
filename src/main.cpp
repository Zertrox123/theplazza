#include "ipc.hpp"
#include "Utils.hpp"
#include <iostream>
#include <regex>

int main() {
    IPC ipc;
    std::string input_user;
    std::regex pattern(R"(\s*([a-zA-Z]+)\s+(S|M|L|XL|XXL)\s+x([1-9][0-9]*)\s*)");

    if (!ipc.init_master()) {
        std::cerr << "ERR: another plazza is running" << std::endl;
        return 0;
    }

    utils::init();

    while (true) {
        std::cout << "$> ";
        std::getline(std::cin, input_user);
        std::stringstream ss(input_user);
        std::string order;
        if (input_user == "exit")
            break;
        while (std::getline(ss, order, ';')) {
            if (order.length() < 1)
                continue;
            std::smatch match;
            if (std::regex_match(order, match, pattern)) {
                std::string pizza_name = match[1].str();
                std::string pizza_size = match[2].str();
                std::string pizza_count = match[3].str().substr(1);
                ipc.send_order("k1", 1, 1, std::vector<std::string>{pizza_name, pizza_size, pizza_count});
            } else {
                std::cout << "Format invalide: " << order << "\n";
            }
        }
    }
}
