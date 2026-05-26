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
        fs::remove(path);
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
    initialized = true;
    return true;
};

bool IPC::init_slave(int id) {
    bool exist = fs::exists(path);
    if (!exist) {
        std::cout << "a master doens't exist" << std::endl;
        return false;
    }
    bool exist2 = fs::exists(path + '/' + std::to_string(id));
    if (exist2) {
        std::cerr << "ERR: id already used" << std::endl;
    }
    fs::create_directory(path + '/' + std::to_string(id));
    return false;
};

bool IPC::send_order(int id, int sender, std::vector<std::string> args) {
    static std::random_device              rd;
    static std::mt19937                    gen(rd());
    static std::uniform_int_distribution<> dis(1, 105);
    std::unique_lock<std::mutex> guard(orders_mutex);
    orders.push_back(Order {
        id,
        sender,
        dis(gen) * dis(gen) * dis(gen) * dis(gen),
        args
    });
    guard.unlock();
    return update_ipc();
};

bool Order::operator==(const Order& rhs) const
{
    return rhs.unique_id == unique_id;
}

std::vector<Order> IPC::get_orders() {
    update_ipc();
    std::unique_lock<std::mutex> guard(orders_mutex);
    return orders;
}

bool IPC::update_ipc() {
    std::unique_lock<std::mutex> guard(orders_mutex);
    //std::ifstream file;
    //file.open(path);
    //std::string line;
    //while (std::getline(file, line)) {
    //    std::stringstream ss(line);
    //    std::string word;
    //    std::string id;
    //    std::string unique_id;
    //    std::string sender;
    //    std::vector<std::string> args;
    //    std::getline(ss, id, ' ');
    //    std::getline(ss, unique_id, ' ');
    //    std::getline(ss, sender, ' ');

    //    while (std::getline(ss, word, ' ')) {
    //        args.push_back(word);
    //    }

    //    orders_to_merge.push_back(Order{
    //            std::atoi(id.c_str()),
    //            std::atoi(sender.c_str()),
    //            std::atoi(unique_id.c_str()),
    //            args
    //    });
    //}
    //file.close();
    
    //std::ofstream wfile;
    //wfile.open(path);
    //for (auto to_write : orders_to_write) {
    //    wfile << to_write.id << " "
    //          << to_write.unique_id << " " 
    //          << to_write.sender << " ";
    //    for (auto args : to_write.args) {
    //        wfile << args << " ";
    //    }
    //    wfile << std::endl;
    //}
    //wfile.close();

    std::vector<Order> new_orders;
    std::string path = "/path/to/directory";
    for (const auto & entry : fs::directory_iterator(path)) {
        std::cout << entry.path() << std::endl;
        if (entry.path().string().find(".done") == std::string::npos)
            continue;
        std::ifstream file;
        file.open(entry.path());
        std::string line;
        while (std::getline(file, line)) {
            std::stringstream ss(line);
            std::string word;
            std::string id;
            std::string unique_id;
            std::string sender;
            std::vector<std::string> args;
            std::getline(ss, id, ' ');
            std::getline(ss, unique_id, ' ');
            std::getline(ss, sender, ' ');

            while (std::getline(ss, word, ' ')) {
                args.push_back(word);
            }

            new_orders.push_back(Order{
                    std::atoi(id.c_str()),
                    std::atoi(sender.c_str()),
                    std::atoi(unique_id.c_str()),
                    args
            });

            file.close();
        }
     
        orders = new_orders;
    }
    return false;
};
