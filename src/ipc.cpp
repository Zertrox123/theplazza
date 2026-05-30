#include <algorithm>
#include <cstdio>
#include <mutex>
#include <string>
#include <sys/mman.h>
#include <sys/stat.h>        /* For mode constants */
#include <fcntl.h>           /* For O_* constants */
#include <cstdlib>
#include <stdlib.h>
#include "ipc.hpp"
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <fstream>
#include <ios>
#include <iostream>
#include <ostream>
#include <sstream>
#include <string>
#include <fstream>

namespace fs = std::filesystem;

IPC::IPC() {
    if (fs::exists("/dev/shm")) {
        path = default_path;
    } else {
        path = path_fallback;
    }
};

IPC::~IPC() {
    if (initialized) {
        fs::remove_all(path);
    }
}

bool IPC::init_master() {
    std::ofstream file;
    bool exist = fs::exists(path);
    if (exist) {
        std::cout << "a master already exist" << std::endl;
        return false;
    }
    fs::create_directory(path);
    fs::create_directory(path + "/0");
    fs::create_directory(path + "/broadcast");
    initialized = true;
    return true;
};

bool IPC::init_slave(std::string _id) {
    bool exist = fs::exists(path);
    if (!exist) {
        std::cout << "a master doens't exist" << std::endl;
        return false;
    }
    bool exist2 = fs::exists(path + '/' + _id);
    if (exist2) {
        std::cerr << "ERR: id already used" << std::endl;
    }
    fs::create_directory(path + '/' + _id);
    id = _id;
    return false;
};

bool IPC::send_order(std::string target, int id, int sender, std::vector<std::string> args) {
    static std::random_device              rd;
    static std::mt19937                    gen(rd());
    static std::uniform_int_distribution<> dis(1, 105);

    int private_id = dis(gen) * dis(gen) * dis(gen) * dis(gen);
    std::string private_id_str = std::to_string(private_id);
    std::unique_lock<std::mutex> guard(orders_mutex);
    Order order_to_write = Order {
        id,
        private_id,
        sender,
        args
    };
    bool a = fs::exists(path + '/' + target);
    if (a) {
        std::vector<uint8_t> buf = Order::serialize(order_to_write);
        std::ofstream file(path + '/' + target + '/' + std::to_string(private_id), std::ios::binary | std::ios::trunc);
        if (file.is_open()) {
            file.write(reinterpret_cast<const char*>(buf.data()), buf.size());
            file.close();
        }
    } else {
        return false;
    }
    guard.unlock();
    update_ipc();
    return true;
};

bool IPC::send_order_broadcast(int id, int sender, std::vector<std::string> args) {
    static std::random_device              rd;
    static std::mt19937                    gen(rd());
    static std::uniform_int_distribution<> dis(1, 105);

    int private_id = dis(gen) * dis(gen) * dis(gen) * dis(gen);
    std::string private_id_str = std::to_string(private_id);
    std::unique_lock<std::mutex> guard(orders_mutex);
    Order order_to_write = Order {
        id,
        private_id,
        sender,
        args
    };
    bool a = fs::exists(path + "/broadcast");
    printf("%i\n", a);
    if (a) {
        std::cout << path + "/broadcast/" + std::to_string(private_id) << std::endl;
        std::vector<uint8_t> buf = Order::serialize(order_to_write);
        std::ofstream file(path + "/broadcast/" + std::to_string(private_id), std::ios::binary | std::ios::trunc);
        file.write(reinterpret_cast<const char*>(buf.data()), buf.size());
        file.close();
    } else {
        return false;
    }
    guard.unlock();
    update_ipc();
    return true;
};

bool Order::operator==(const Order& rhs) const
{
    return rhs.unique_id == unique_id;
}

std::vector<Order> IPC::get_orders() {
    update_ipc();
    return orders;
}

std::string IPC::getId() const{
    return id;
}

void IPC::setId(std::string _id) {
    id = _id;
}
bool IPC::update_ipc() {
    std::unique_lock<std::mutex> guard(orders_mutex);
    std::vector<Order> new_orders;
    if (!fs::exists(path))
        return false;
    for (const auto & entry : fs::directory_iterator(path + '/' + id)) {
        if (entry.path().string().find(".done") != std::string::npos)
            continue;
        std::ifstream file;
        file.open(entry.path());
        std::ifstream filee(path, std::ios::binary | std::ios::ate);
        size_t size = file.tellg();
        filee.seekg(0, std::ios::beg);
        std::vector<uint8_t> buf(size);
        filee.read(reinterpret_cast<char*>(buf.data()), size);
        filee.close();
     
        Order orders;
        orders = Order::deserialize(buf);
        new_orders.push_back(orders);
    }
    for (const auto & entry : fs::directory_iterator(path + "/broadcast")) {
        if (entry.path().string().find(".done") != std::string::npos)
            continue;
        std::ifstream file;
        file.open(entry.path());
        std::ifstream filee(path, std::ios::binary | std::ios::ate);
        size_t size = file.tellg();
        filee.seekg(0, std::ios::beg);
        std::vector<uint8_t> buf(size);
        filee.read(reinterpret_cast<char*>(buf.data()), size);
        filee.close();
     
        Order orders;
        orders = Order::deserialize(buf);
        new_orders.push_back(orders);
    }
    orders = new_orders;
    return false;
};


bool IPC::set_order_done(Order order) {
    std::unique_lock<std::mutex> guard(orders_mutex);
    int index = 0;
    for (; orders.at(index).unique_id != order.unique_id; index++);
    orders.erase(orders.begin() + index);
    fs::remove(path + '/' + id + '/' + std::to_string(order.unique_id));
    update_ipc();
    return true;
}
