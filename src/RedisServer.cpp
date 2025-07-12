#include "RedisServer.hpp"
#include "RedisCommandHandler.hpp"
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>
#include <netinet/in.h>
#include <string>
#include <atomic>
#include <thread>
#include <cstring>

RedisServer::RedisServer(int port) : _port(port), _server_socket(-1), _running(true) {}

void RedisServer::run() {
    _server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (_server_socket < 0) {
        std::cerr << "Error Creating Server Socket \n";
    }

    int opt = 1;
    setsockopt(_server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(_port);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(_server_socket, (struct sockaddr*)& serverAddr, sizeof(serverAddr))) {
        std::cerr << "Error Binding Server Socket \n";
        return;
    }

    if (listen(_server_socket, 10) < 0) {
        std::cerr << "Error Listening \n";
    }

    std::cout << "Redis Server Listening On Port : " << _port << "\n";

    std::vector<std::thread> threads;
    RedisCommandHandler cmdHandler;

    while(_running) {
        int client_socket = accept(_server_socket, nullptr, nullptr);
        if (client_socket < 0) {
            if (_running)
                std::cerr << "-ERR Can't accept connection \n";
            break;
        }

        threads.emplace_back([client_socket, &cmdHandler]() {
            char buffer[1024];
            while (true) {
                memset (buffer, 0, sizeof(buffer));
                int bytes = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
                if (bytes <= 0) break;
                std::string req(buffer, bytes);
                std::string res = cmdHandler.processCommand(req);
                send(client_socket, res.c_str(), res.size(), 0);
            }
            close(client_socket); 
        });
    }

    for (auto &t : threads) {
        if (t.joinable()) t.join();
    }
}

void RedisServer::shutdown() {
    _running = false;
    if (_server_socket != -1) {
        close(_server_socket);
    }
    std::cout << "Server shutdown! \n";
}