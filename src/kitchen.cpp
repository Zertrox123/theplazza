#include "Kitchen.hpp"
#include "ipc.hpp"
#include <cstdlib>
#include <iostream>
#include <string>
#include <thread>


void Kitchen::add_cook(Cook cook) {
    cooks.push_back(cook);
}

void Kitchen::loop(std::string id) {
    IPC ipc;
    ipc.init_slave("k" + id);
    int cook_count = 0;
    for (auto cook : cooks) {
        cook_count++;
        std::thread t(cook.start, "c" + std::to_string(cook_count)); 
    }

    while (true) {
        for (auto i : ipc.get_orders()) {
            std::cout << "Kitchen " << ipc.getId() << " received orders" << std::endl;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}
