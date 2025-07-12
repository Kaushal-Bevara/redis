#pragma once

#include <atomic>

class RedisServer {
    int _port;
    int _server_socket;
    std::atomic<bool> _running;

public: 
    RedisServer(int port);
    void shutdown();
    void run();

};
