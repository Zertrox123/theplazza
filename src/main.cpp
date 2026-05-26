#include "ipc.hpp"

int main() {
    IPC ipc;
    ipc.init_master();
    if (!ipc.init_master())
        return 0;
}
