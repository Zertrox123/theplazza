#include "Utils.hpp"
#include "Kitchen.hpp"
#include <string>
#include <unistd.h>

void utils::init(double multiplier, int cooksPerKitchen, int replaceTime) {
    for (int i = 0; i < 3; i++) {
        pid_t pid = fork();
        if (pid == 0) {
            Kitchen k(multiplier, cooksPerKitchen, replaceTime);
            k.loop(std::to_string(i));
            exit(0);
        }
    }
};
