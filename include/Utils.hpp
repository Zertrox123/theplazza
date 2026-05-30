#pragma once
#include "ipc.hpp"
#include <string>
#include <vector>

namespace utils {
    void init(double mult, int nb_cooks, int regen);
    bool chk_args(int argc, char **argv, double &mult, int &nb_cooks, int &regen);
    std::string ipc_path();
    std::string kid(std::string id);
    bool k_up(std::string id);
    void kprint(std::string msg);
    void prompt();
    void do_status(IPC &ipc, std::vector<std::string> &kitchens);
    void write_status(std::string id, std::string txt);
    std::string mk_status(std::string id, int busy, int nb_cooks, int pending,
        int d, int t, int g, int h, int m, int s, int e, int go, int ch);
    std::string get_best(IPC &ipc, std::vector<std::string> &kitchens, int max_load);
    std::string get_free(IPC &ipc, std::vector<std::string> &kitchens, int max_load);
    void fork_kitchen(std::string id, double mult, int nb_cooks, int regen);
}
