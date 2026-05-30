#include "Kitchen.hpp"
#include "ipc.hpp"
#include <cstdlib>
#include <iostream>
#include <string>
#include <thread>

Kitchen::Kitchen() {}
Kitchen::~Kitchen() {}

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

    std::cout << "Kitchen " << ipc.getId() << " is up" << std::endl;
    while (true) {
        for (auto i : ipc.get_orders()) {
            std::cout << "aa" << std::endl;
            switch (i.id) {
                case OCOOK_PIZZA:
                    break;
                case ODESTRUCT:
                    break;
            }
            ipc.set_order_done(i);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}
