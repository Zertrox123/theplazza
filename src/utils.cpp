#include "Utils.hpp"
#include "Kitchen.hpp"
#include <fstream>
#include <iostream>
#include <sstream>
#include <unistd.h>
#include <sys/file.h>

void utils::init(double mult, int nb_cooks, int regen)
{
    for (int i = 0; i < 0; i++) {
        pid_t pid = fork();
        if (pid == 0) {
            Kitchen k(mult, nb_cooks, regen);
            k.loop(std::to_string(i));
            exit(0);
        }
    }
}

bool utils::chk_args(int argc, char **argv, double &mult, int &nb_cooks, int &regen)
{
    if (argc != 4)
        return false;
    mult = std::stod(argv[1]);
    nb_cooks = std::stoi(argv[2]);
    regen = std::stoi(argv[3]);
    if (nb_cooks < 1 || regen < 1 || mult <= 0)
        return false;
    return true;
}

std::string utils::ipc_path()
{
    if (access("/dev/shm/plazza", F_OK) == 0)
        return "/dev/shm/plazza";
    return "/tmp/plazza";
}

std::string utils::kid(std::string id)
{
    return "k" + id;
}

bool utils::k_up(std::string id)
{
    return access((ipc_path() + "/" + kid(id)).c_str(), F_OK) == 0;
}

void utils::kprint(std::string msg)
{
    flock(fileno(stdout), LOCK_EX);
    write(fileno(stdout), msg.c_str(), msg.size());
    flock(fileno(stdout), LOCK_UN);
}

void utils::prompt()
{
    flock(fileno(stdout), LOCK_EX);
    std::cout << "$> ";
    flock(fileno(stdout), LOCK_UN);
}

void utils::write_status(std::string id, std::string txt)
{
    std::ofstream f(ipc_path() + "/" + kid(id) + "/status");
    f << txt;
}

std::string utils::mk_status(std::string id, int busy, int nb_cooks, int pending,
    int d, int t, int g, int h, int m, int s, int e, int go, int ch)
{
    std::stringstream ss;
    ss << "=== Cuisine " << id << " ===" << std::endl;
    ss << "Cooks: " << busy << "/" << nb_cooks << " occupes" << std::endl;
    ss << "Commandes en attente: " << pending << std::endl;
    ss << "Stock: dough=" << d
        << " tomato=" << t
        << " gruyere=" << g
        << " ham=" << h
        << " mushrooms=" << m
        << " steak=" << s
        << " eggplant=" << e
        << " goat=" << go
        << " chief=" << ch << std::endl;
    return ss.str();
}

void utils::do_status(IPC &ipc, std::vector<std::string> &kitchens)
{
    int alive = 0;
    for (std::string &id : kitchens) {
        if (k_up(id))
            alive++;
    }
    ipc.setId("");
    std::vector<Order> old = ipc.get_orders();
    for (long unsigned i = 0; i < old.size(); i++)
        ipc.remove_broadcast(old[i].unique_id);
    ipc.send_order_broadcast(OSTATUS, 0, {});
    for (int t = 0; t < 20; t++) {
        usleep(100000);
        int got = 0;
        for (std::string &id : kitchens) {
            std::ifstream f(ipc_path() + "/" + kid(id) + "/status");
            if (f.is_open()) {
                got++;
                f.close();
            }
        }
        if (got >= alive)
            break;
    }
    for (std::string &id : kitchens) {
        std::ifstream f(ipc_path() + "/" + kid(id) + "/status");
        if (f.is_open() == false)
            continue;
        std::string line;
        while (std::getline(f, line))
            std::cout << line << std::endl;
        f.close();
        remove((ipc_path() + "/" + kid(id) + "/status").c_str());
    }
    ipc.setId("");
    std::vector<Order> rep = ipc.get_orders();
    for (long unsigned i = 0; i < rep.size(); i++) {
        if (rep[i].id == OSTATUS)
            ipc.remove_broadcast(rep[i].unique_id);
    }
}

std::string utils::get_best(IPC &ipc, std::vector<std::string> &kitchens, int max_load)
{
    int min = max_load;
    std::string best = "";
    for (std::string &id : kitchens) {
        if (k_up(id) == false)
            continue;
        ipc.setId(kid(id));
        int load = ipc.get_orders().size();
        if (load < min) {
            min = load;
            best = id;
        }
    }
    return best;
}

std::string utils::get_free(IPC &ipc, std::vector<std::string> &kitchens, int max_load)
{
    for (std::string &id : kitchens) {
        if (k_up(id) == false)
            continue;
        ipc.setId(kid(id));
        if ((int)ipc.get_orders().size() < max_load)
            return id;
    }
    return "";
}

void utils::fork_kitchen(std::string id, double mult, int nb_cooks, int regen)
{
    pid_t pid = fork();
    if (pid == 0) {
        Kitchen k(mult, nb_cooks, regen);
        k.loop(id);
        exit(0);
    }
    usleep(50000);
}
