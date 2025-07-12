#pragma once

#include <string>

class RedisCommandHandler {

public:
    RedisCommandHandler();
    // Process command from client and return RESP-formatted response
    std::string processCommand(const std::string& commandLine);

};