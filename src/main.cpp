#include "ipc.hpp"
#include "Utils.hpp"
#include "Kitchen.hpp"
#include <iostream>
#include <regex>
#include <unistd.h>
#include <sys/file.h>

int main(int argc, char **argv) {
    IPC ipc;
    std::string input_user;
    std::regex pattern(R"(\s*([a-zA-Z]+)\s+(S|M|L|XL|XXL)\s+x([1-9][0-9]*)\s*)");
    if (argc != 4) {
        std::cerr << "Usage: ./plazza <multiplier> <cooks_per_kitchen> <time_to_replace_ingredients>" << std::endl;
        return 84;
    }

    if (!ipc.init_master()) {
        std::cerr << "ERR: another plazza is running" << std::endl;
        return 0;
    }
    double mult = std::stod(argv[1]);
    int cooks_kitchen = std::stoi(argv[2]);
    int replace_time = std::stoi(argv[3]);
    utils::init(mult, cooks_kitchen, replace_time);
    int max_capacity_per_kitchen = 2 * cooks_kitchen;
    std::vector<std::string> active_kitchens = {"0", "1", "2"};
    int next_kitchen_id = 3;
    while (true) {
        flock(fileno(stdout), LOCK_EX);
        std::cout << "$> ";
        flock(fileno(stdout), LOCK_UN);
        std::getline(std::cin, input_user);
        std::stringstream ss(input_user);
        std::string order;
        if (input_user == "exit")
            break;
        if (input_user == "status") {
            ipc.setId("");
            std::vector<Order> old = ipc.get_orders();
            for (long unsigned i = 0; i < old.size(); i++)
                ipc.remove_broadcast(old[i].unique_id);
            ipc.send_order_broadcast(OSTATUS, 0, {});
            usleep(300000);
            ipc.setId("");
            std::vector<Order> msgs = ipc.get_orders();
            for (long unsigned i = 0; i < msgs.size(); i++) {
                Order o = msgs[i];
                if (o.id != OSTATUS)
                    continue;
                if (o.sender <= 0) {
                    ipc.remove_broadcast(o.unique_id);
                    continue;
                }
                if (o.args.size() < 13)
                    continue;
                std::cout << "=== Cuisine " << o.args[0] << " ===" << std::endl;
                std::cout << "Cooks: " << o.args[1] << "/" << o.args[2] << " occupes" << std::endl;
                std::cout << "Commandes en attente: " << o.args[3] << std::endl;
                std::cout << "Stock: dough=" << o.args[4]
                    << " tomato=" << o.args[5]
                    << " gruyere=" << o.args[6]
                    << " ham=" << o.args[7]
                    << " mushrooms=" << o.args[8]
                    << " steak=" << o.args[9]
                    << " eggplant=" << o.args[10]
                    << " goat=" << o.args[11]
                    << " chief=" << o.args[12] << std::endl;
                ipc.remove_broadcast(o.unique_id);
            }
            continue;
        }
        while (std::getline(ss, order, ';')) {
            if (order.length() < 1)
                continue;
            std::smatch match;
            if (std::regex_match(order, match, pattern)) {
                std::string pizza_name = match[1].str();
                std::string pizza_size = match[2].str();
                int pizza_count = std::stoi(match[3].str());
                for (int i = 0; i < pizza_count; i++) {
                    int min_load = max_capacity_per_kitchen;
                    std::string best_kitchen = "";
                    for (std::string& k_id : active_kitchens) {
                        std::string kid = "k" + k_id;
                        if (access(("/tmp/plazza/" + kid).c_str(), F_OK) != 0
                            && access(("/dev/shm/plazza/" + kid).c_str(), F_OK) != 0)
                            continue;
                        ipc.setId(kid);
                        int load = ipc.get_orders().size();
                        if (load < min_load) {
                            min_load = load;
                            best_kitchen = k_id;
                        }
                    }
                    if (best_kitchen == "" || min_load >= max_capacity_per_kitchen) {
                        bool found = false;
                        for (std::string& k_id : active_kitchens) {
                            std::string kid = "k" + k_id;
                            if (access(("/tmp/plazza/" + kid).c_str(), F_OK) != 0
                                && access(("/dev/shm/plazza/" + kid).c_str(), F_OK) != 0)
                                continue;
                            ipc.setId(kid);
                            if ((int)ipc.get_orders().size() < max_capacity_per_kitchen) {
                                best_kitchen = k_id;
                                found = true;
                                break;
                            }
                        }
                        if (found == false) {
                            best_kitchen = std::to_string(next_kitchen_id);
                            next_kitchen_id++;
                            active_kitchens.push_back(best_kitchen);
                            pid_t pid = fork();
                            if (pid == 0) {
                                Kitchen new_kitchen(mult, cooks_kitchen, replace_time);
                                new_kitchen.loop(best_kitchen);
                                exit(0);
                            }
                            usleep(50000);
                        }
                    }
                    ipc.send_order("k" + best_kitchen, OCOOK_PIZZA, 0, std::vector<std::string>{pizza_name, pizza_size});
                }
            } else {
                std::cout << "Format invalide: " << order << "\n";
            }
        }
    }
}
