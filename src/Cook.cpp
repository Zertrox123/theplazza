#include "Cook.hpp"
#include "ipc.hpp"
#include <chrono>
#include <iostream>
#include <ostream>
#include <thread>

void Cook::start(std::string id) {
    IPC ipc;
    ipc.init_slave("c" + id);

    while (true) {
        for (auto i : ipc.get_orders()) {
            std::cout << "cook " << ipc.getId() << " received orders" << std::endl;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}
