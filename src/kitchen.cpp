#include "Kitchen.hpp"
#include "Utils.hpp"
#include "ipc.hpp"
#include <iostream>
#include <unistd.h>

Kitchen::Kitchen(double mult, int nb_cooks, int regen)
    : mult(mult), nb_cooks(nb_cooks), regen(regen), run(true)
{
    st_dough = 5;
    st_tomato = 5;
    st_gruyere = 5;
    st_ham = 5;
    st_mush = 5;
    st_steak = 5;
    st_egg = 5;
    st_goat = 5;
    st_chief = 5;
    nb_busy = 0;
}

Kitchen::~Kitchen() {
    run = false;
    bell.notifyAll();
    for (long unsigned i = 0; i < threads.size(); i++) {
        if (threads[i].joinable())
            threads[i].join();
    }
}

bool Kitchen::takeStock(std::string name)
{
    st_mx.lock();
    bool ok = false;
    if (name == "Margarita" || name == "margarita") {
        if (st_dough > 0 && st_tomato > 0 && st_gruyere > 0) {
            st_dough--;
            st_tomato--;
            st_gruyere--;
            ok = true;
        }
    } else if (name == "Regina" || name == "regina") {
        if (st_dough > 0 && st_tomato > 0 && st_gruyere > 0 && st_ham > 0 && st_mush > 0) {
            st_dough--;
            st_tomato--;
            st_gruyere--;
            st_ham--;
            st_mush--;
            ok = true;
        }
    } else if (name == "Americana" || name == "americana") {
        if (st_dough > 0 && st_tomato > 0 && st_gruyere > 0 && st_steak > 0) {
            st_dough--;
            st_tomato--;
            st_gruyere--;
            st_steak--;
            ok = true;
        }
    } else if (name == "Fantasia" || name == "fantasia") {
        if (st_dough > 0 && st_tomato > 0 && st_egg > 0 && st_goat > 0 && st_chief > 0) {
            st_dough--;
            st_tomato--;
            st_egg--;
            st_goat--;
            st_chief--;
            ok = true;
        }
    }
    st_mx.unlock();
    return ok;
}

void Kitchen::cookStart()
{
    bell_mx.lock();
    nb_busy++;
    bell_mx.unlock();
}

void Kitchen::cookEnd()
{
    bell_mx.lock();
    nb_busy--;
    bell_mx.unlock();
}

int Kitchen::getBusy()
{
    bell_mx.lock();
    int b = nb_busy;
    bell_mx.unlock();
    return b;
}

void Kitchen::loop(std::string id) {
    IPC ipc;
    ipc.init_slave(utils::kid(id));
    for (int i = 0; i < nb_cooks; i++) {
        Cook *c = new Cook(i + 1, orders, bell, bell_mx, run, mult, id, *this);
        cooks.push_back(c);
        threads.push_back(std::thread(&Cook::work, c));
    }
    std::vector<int> qid;
    std::vector<int> sid;
    int idle = 0;
    int reg_t = 0;
    while (run) {
        std::vector<Order> cmds = ipc.get_orders();
        bool got_new = false;
        for (long unsigned i = 0; i < cmds.size(); i++) {
            Order cmd = cmds[i];
            if (cmd.id == OSTATUS) {
                if (cmd.sender == 0) {
                    bool done = false;
                    for (long unsigned s = 0; s < sid.size(); s++) {
                        if (sid[s] == cmd.unique_id) {
                            done = true;
                            break;
                        }
                    }
                    if (done == false) {
                        sid.push_back(cmd.unique_id);
                        int busy = getBusy();
                        int pending = orders.getSize() + busy;
                        st_mx.lock();
                        utils::write_status(id, utils::mk_status(id, busy, nb_cooks, pending,
                            st_dough, st_tomato, st_gruyere, st_ham, st_mush,
                            st_steak, st_egg, st_goat, st_chief));
                        st_mx.unlock();
                        ipc.send_order_broadcast(OSTATUS, std::stoi(id) + 1, {});
                    }
                }
                continue;
            }
            bool known = false;
            for (long unsigned j = 0; j < qid.size(); j++) {
                if (qid[j] == cmd.unique_id) {
                    known = true;
                    break;
                }
            }
            if (known == false && orders.getSize() < 2 * nb_cooks) {
                orders.push(cmd);
                qid.push_back(cmd.unique_id);
                bell.notifyOne();
                got_new = true;
            }
        }
        for (long unsigned j = 0; j < qid.size(); ) {
            bool found = false;
            for (long unsigned i = 0; i < cmds.size(); i++) {
                if (qid[j] == cmds[i].unique_id) {
                    found = true;
                    break;
                }
            }
            if (found == false)
                qid.erase(qid.begin() + j);
            else
                j++;
        }
        if (got_new == true || getBusy() > 0)
            idle = 0;
        else if (orders.isEmpty() == true && qid.empty() == true)
            idle++;
        reg_t += 50;
        if (reg_t >= regen) {
            st_mx.lock();
            st_dough++;
            st_tomato++;
            st_gruyere++;
            st_ham++;
            st_mush++;
            st_steak++;
            st_egg++;
            st_goat++;
            st_chief++;
            st_mx.unlock();
            reg_t = 0;
            bell.notifyAll();
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        if (idle >= 100) {
            utils::kprint("Cuisine " + id + "inactive depuis 5 secondes (fermeture)\n");
            run = false;
            break;
        }
    }
}
