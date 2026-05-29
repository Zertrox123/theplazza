#include "Utils.hpp"
#include "Kitchen.hpp"
#include <string>
#include <unistd.h>

void utils::init() {
    for (int i = 0; i < 3; i++) {
        pid_t pid = fork();
        if (pid == 0) {
            Kitchen k;
            k.loop(std::to_string(i));
        }
    }
};
