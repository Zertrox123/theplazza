#include "ipc.hpp"
#include <filesystem>
#include <iostream>
#include <ostream>
#include <sstream>
#include <string>
#include <fstream>

IPC::IPC() {

};
IPC::~IPC() {
    if (initialized)
        std::filesystem::remove("/tmp/plazza.ipc");
}

bool IPC::init_master() {
    std::ofstream file;
    bool exist = std::filesystem::exists("/tmp/plazza.ipc");
    if (exist) {
        std::cout << "a master already exist" << std::endl;
        return false;
    }
    file.open("/tmp/plazza.ipc");
    if (!file.is_open()) {
        return false;
    }
    file.close();
    initialized = true;
    return true;
};

bool IPC::init_slave() {
    bool exist = std::filesystem::exists("/tmp/plazza.ipc");
    if (!exist) {
        std::cout << "a master already exist" << std::endl;
        return false;
    }
    return false;
};

bool IPC::send_order(int id, int sender, std::vector<std::string> args) {
    orders.push_back(Order {
        id,
        sender,
        args
    });
    return update_ipc();
};

bool IPC::update_ipc() {
    std::lock_guard<std::mutex> guard(orders_mutex);
    std::vector<Order> orders_to_merge;
    std::ifstream file;
    file.open("/tmp/plazza.ipc");
    std::string line;
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string word;
        std::string id;
        std::vector<std::string> args;
        std::getline(ss, id, ' ');
        while (std::getline(ss, word, ' ')) {
            args.push_back(word);
        }
        //orders_to_merge.push_back(Order{})
    }
    return false;
};
