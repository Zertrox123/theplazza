#include "Cook.hpp"
#include "Kitchen.hpp"
#include "Utils.hpp"
#include "ipc.hpp"
#include <thread>
#include <chrono>

Cook::Cook(int id, Queue& q, PlazzaCondVar& bell, PlazzaMutex& mx, bool& run, double mult, std::string kid, Kitchen& k)
    : id(id), q(q), bell(bell), mx(mx), run(run), mult(mult), kid(kid), k(k) {}

void Cook::work() {
    while (run) {
        Order cmd;
        std::unique_lock<std::mutex> lock(mx.get());
        bell.wait(lock, [this]() {
            return !q.isEmpty() || !run;
        });
        if (!run && q.isEmpty())
            break;
        if (q.tryPop(cmd)) {
            lock.unlock();
            std::string name = cmd.args[0];
            while (run && k.takeStock(name) == false)
                std::this_thread::sleep_for(std::chrono::milliseconds(50));
            if (!run)
                continue;
            k.cookStart();
            utils::kprint("[Cuisine " + kid + "] Cook " + std::to_string(id) + " prepare une " + name + "\n");
            int t = 0;
            if (name == "Margarita" || name == "margarita")
                t = 1;
            else if (name == "Regina" || name == "regina")
                t = 2;
            else if (name == "Americana" || name == "americana")
                t = 2;
            else if (name == "Fantasia" || name == "fantasia")
                t = 4;
            int ms = t * mult * 1000;
            std::this_thread::sleep_for(std::chrono::milliseconds(ms));
            utils::kprint("[Cuisine " + kid + "] Cook " + std::to_string(id) + " a TERMINE la " + name + "\n");
            IPC ipc;
            ipc.setId(utils::kid(kid));
            ipc.set_order_done(cmd);
            k.cookEnd();
        }
    }
}
