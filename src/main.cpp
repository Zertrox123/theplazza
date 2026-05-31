#include "ipc.hpp"
#include "Utils.hpp"
#include <memory>
#include <sys/wait.h>
#include "Kitchen.hpp"
#include <iostream>
#include <regex>
#include <sstream>
#include <unistd.h>

int main(int argc, char **argv) {
    IPC ipc;
    std::string in;
    std::regex pat(R"(\s*([a-zA-Z]+)\s+(S|M|L|XL|XXL)\s+x([1-9][0-9]*)\s*)");
    double mult;
    int nb_cooks;
    int regen;
    if (utils::chk_args(argc, argv, mult, nb_cooks, regen) == false) {
        std::cerr << "Usage: ./plazza <multiplier> <cooks_per_kitchen> <time_to_replace_ingredients>" << std::endl;
        return 84;
    }
    if (!ipc.init_master()) {
        std::cerr << "ERR: another plazza is running" << std::endl;
        return 0;
    }
    utils::init(mult, nb_cooks, regen);
    int max_load = 2 * nb_cooks;
    std::vector<std::string> kitchens = {"0", "1", "2"};
    int next_id = 3;
    while (true) {
        if (isatty(0)) {
            utils::prompt();
        }
        if (!std::getline(std::cin, in)) {
            while (true) {
                int pending_orders = 0;
                for (const std::string& kid : kitchens) {
                    if (utils::k_up(kid)) {
                        ipc.setId(utils::kid(kid));
                        pending_orders += ipc.get_orders().size();
                    }
                }
                
                if (pending_orders == 0) {
                    break;
                }
                usleep(100000); 
            }

            ipc.send_order_broadcast(ODESTRUCT, 0, std::vector<std::string> {""});
            int status;
    pid_t wpid;
            while ((wpid = wait(&status)) > 0 || (wpid == -1 && errno == EINTR));
            break;
        }
        std::stringstream ss(in);
        std::string line;
        if (in == "exit")
            break;
        if (in == "status") {
            utils::do_status(ipc, kitchens);
            continue;
        }
        while (std::getline(ss, line, ';')) {
            if (line.length() < 1)
                continue;
            std::smatch m;
            if (std::regex_match(line, m, pat)) {
                std::string name = m[1].str();
                std::string size = m[2].str();
                int nb = std::stoi(m[3].str());
                for (int i = 0; i < nb; i++) {
                    std::string best = utils::get_best(ipc, kitchens, max_load);
                    int load = max_load;
                    if (best != "") {
                        ipc.setId(utils::kid(best));
                        load = ipc.get_orders().size();
                    }
                    if (best == "" || load >= max_load) {
                        best = utils::get_free(ipc, kitchens, max_load);
                        if (best == "") {
                            best = std::to_string(next_id);
                            next_id++;
                            kitchens.push_back(best);
                            utils::fork_kitchen(best, mult, nb_cooks, regen);
                        }
                    }
                    ipc.send_order(utils::kid(best), OCOOK_PIZZA, 0,
                        std::vector<std::string>{name, size});
                }
            } else {
                std::cout << "Format invalide: " << line << "\n";
            }
        }
        if (isatty(0)) {
            ipc.send_order_broadcast(ODESTRUCT, 0, std::vector<std::string> {""});
            break;
        }
    }
    ipc.send_order_broadcast(ODESTRUCT, 0, std::vector<std::string>{}); 
}
