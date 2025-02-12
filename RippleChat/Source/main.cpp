#include "../Headers/tcp/WebSocketServer.hpp"
#include "../Headers/utils/Logger.hpp"
#include <iostream>

int main() {
    try {
        ws::WebSocketServer server(8080);
        server.start();
    }
    catch (const std::exception& e) {
        utils::Logger::log("Fatal error: " + std::string(e.what()),
            utils::Logger::LogLevel::ERR);
        return 1;
    }
    return 0;
}