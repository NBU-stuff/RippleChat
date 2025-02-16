#include "WebSocketTypes.hpp"
#include<string>
struct WebSocketMessage {
    ws::MessageType type;
    std::string sender;
    std::string content;
    // other fields...
};