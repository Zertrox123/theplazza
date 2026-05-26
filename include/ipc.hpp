#pragma once

#include <mutex>
#include <string>
#include <vector>

/*
 *  id 1 -> make a pizza
 *  args:
 *      - pizzaria id
 *      - pizza name
 *
 *  id 2 -> pizza_finished 
 *  args:
 *      - pizza name
 *
 * id 3 -> self destroy
 *  args:
 *      - pizza name
 *
 */

class Order{
    public:
        // what type of order
        int id = 0;
        // who send
        int sender = 0;
        std::vector<std::string> args;
};

class IPC {
    public:
        IPC();
        ~IPC();
        bool init_master();
        bool init_slave();
        bool send_order(int id, int sender, std::vector<std::string> args);
        std::vector<Order> get_orders();
    private:
        bool update_ipc();
        std::vector<Order> orders;
        std::mutex orders_mutex;
        bool initialized = false;
};
//TODO: add eq overload 
