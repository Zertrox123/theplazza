#pragma once

#include <cstdint>
#include <mutex>
#include <sstream>
#include <random>
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

enum order_type
{
    OCOOK_PIZZA = 1,
    ODESTRUCT = 99,
};

class Order{
    public:
        // what type of order
        int id = 0;
        int unique_id = 0;
        // who send
        int sender = 0;
        std::vector<std::string> args;
        bool operator==(const Order& rhs) const;
        Order deserialize(std::vector<uint8_t> buf);
        std::vector<uint8_t> serialize(std::vector<uint8_t> buf);
};

class IPC {
    public:
        IPC();
        ~IPC();
        bool init_master();
        bool init_slave(std::string id);
        bool send_order(std::string target, int id, int sender, std::vector<std::string> args);
        bool set_order_done(Order order);
        std::vector<Order> get_orders();
        std::string getId() const;
        void setId(std::string id);
    private:
        bool update_ipc();
        std::vector<Order> orders;
        std::mutex orders_mutex;
        std::string id = 0;
        bool initialized = false;
        std::string path = "/dev/shm/plazza";
        std::string default_path = "/dev/shm/plazza";
        // c pas de l'ai leo nononon c moi j'ai ecris fallback a la main lettre BY LETTRE, + le /dev/shm c du ramdisk donc sa me parrait logic de pas niquer ton disk for du ipc supprime si ta lu + /tmp c du ramdisk des fois pas 100% du temps
        std::string path_fallback = "/tmp/plazza";
};
//TODO: add eq overload 
