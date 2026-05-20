#include "ipc.hpp"
#include <string>

IPC::IPC() {

};

bool IPC::init_master() {
    return false;
};

bool IPC::init_slave() {
    return false;
};

bool IPC::send_order(int id, int sender, std::vector<std::string> args) {
    orders.push_back(Order {
        id,
        sender,
        args
    });
    update_ipc();
    return false;
};

