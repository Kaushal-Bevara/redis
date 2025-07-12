#include <iostream>
#include <thread>
#include <chrono>

#include "RedisServer.hpp";

int main(int argc, char*argv[]) {
    int port = 6379;
    if (argc >= 2) port = std::stoi(argv[1]);

    RedisServer server(port);

    // Background Persistence: Backup the database every 300 seconds
    std::thread persistenceThread([]() {
        while (true) {
            std::this_thread::sleep_for(std::chrono::seconds(300));
            // TODO: dump database
        }
    });
    persistenceThread.detach();

    server.run();
    return 0;
}