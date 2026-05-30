#include "ipc.hpp"
#include "Utils.hpp"
#include "Kitchen.hpp"
#include <iostream>
#include <regex>
#include <unistd.h>

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
    std::vector<std::string> active_kitchens;
    int next_kitchen_id = 3;
    while (true) {
        std::cout << "$> ";
        std::getline(std::cin, input_user);
        std::stringstream ss(input_user);
        std::string order;
        if (input_user == "exit")
            break;
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
                        ipc.setId(k_id);
                        int load = ipc.get_orders().size();
                        if (load < min_load) {
                            min_load = load;
                            best_kitchen = k_id;
                        }
                        ipc.setId("0");
                    }
                    if (best_kitchen == "") {
                        best_kitchen = "k" + std::to_string(next_kitchen_id);
                        next_kitchen_id++;
                        active_kitchens.push_back(best_kitchen);
                        pid_t pid = fork();
                        if (pid == 0) {
                            Kitchen new_kitchen(mult, cooks_kitchen, replace_time);
                            new_kitchen.loop(best_kitchen); 
                            exit(0);
                        }
                    }
                    ipc.send_order(best_kitchen, OCOOK_PIZZA, 0, std::vector<std::string>{pizza_name, pizza_size}); 
                }
            } else {
                std::cout << "Format invalide: " << order << "\n";
            }
        }
    }
}