#include "RedisCommandHandler.hpp"

#include <vector>
#include <sstream>
#include <iostream>
#include <algorithm>
using std::cout;
// Resp Parser based on : https://redis.io/docs/latest/develop/reference/protocol-spec/

std::vector<std::string> parseRespCommand(const std::string &input) {

    std::vector<std::string> tokens;
    if (input.empty()) return tokens;

    // if it doesn't start with '*', fallback to splitting by whitespace
    if (input[0] != '*') {
        std::istringstream iss(input);
        std::string token;
        while(iss >> token) tokens.push_back(token);
        return tokens;
    }

    size_t pos = 0;
    pos++; // skip '*'


    // finds carriage return line feed
    size_t crlf = input.find("\r\n", pos);
    if (crlf == std::string::npos) return tokens;


    // extract numElements
    int numElements = std::stoi(input.substr(pos, crlf-pos));
    
    pos = crlf + 2;

    for (int i = 0; i < numElements;  i++) {
        if (pos >= input.size() || input[pos] != '$') break;
        pos++;

        crlf = input.find("\r\n", pos);
        if (crlf == std::string::npos) break;
        int len = std::stoi(input.substr(pos, crlf-pos));
        pos = crlf + 2;

        if (pos + len > input.size()) break;
        tokens.push_back(std::move(input.substr(pos, len)));
        pos +=  len + 2; // skip token and crlf
    }
    return tokens;
}

RedisCommandHandler::RedisCommandHandler() = default;

std::string RedisCommandHandler::processCommand(const std::string& commandLine) {
    // Use RESP parser
    auto tokens = parseRespCommand(commandLine);
    if (tokens.empty()) return "-ERR Empty Command";

    std::string cmd = tokens[0];
    std::transform(cmd.begin(), cmd.end(), cmd.begin(), ::toupper);
    std::ostringstream response;
    


    return response.str();
}

int main() {
    std::vector<std::string> v;
    v = parseRespCommand("*3\r\n$4\r\npoop\r\n$4\r\nshit\r\n$3\r\npee\r\n");
    for (auto &s : v) std::cout << s << "\n";

    
}