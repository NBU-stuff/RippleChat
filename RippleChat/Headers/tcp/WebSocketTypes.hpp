#ifndef WEBSOCKET_TYPES_HPP
#define WEBSOCKET_TYPES_HPP

#include <string>
#include <nlohmann/json.hpp>
#include "../Headers/utils/Logger.hpp"
namespace ws {

    // Message types for WebSocket communication
    enum class MessageType {
        CHAT_MESSAGE,      // Regular chat message
        USER_STATUS,       // User online/offline status
        TYPING_STATUS,     // User typing indicator
        ERR,            // Error message
        CONNECTION,       // Connection-related messages
        AUTHENTICATION    // Authentication-related messages
    };

    // Base message structure
    struct WebSocketMessage {
        MessageType type;
        std::string sender;
        std::string content;
        std::string timestamp;
        std::string receiver;
        std::string token;



        // Convert message to JSON
        nlohmann::json toJson() const {

            if (type == MessageType::AUTHENTICATION) {

                return {
                {"type", static_cast<int>(type)},
                {"sender", sender},

                {"token",token},


                };
            }
            else {
                return {
                    {"type", static_cast<int>(type)},
                    {"sender", sender},
                    {"receiver",receiver},
                    {"content", content},
                    {"token",token},
                    {"timestamp", timestamp},

                };

            }
        }

        // Create message from JSON
        static WebSocketMessage fromJson(const nlohmann::json& json) {



            if (static_cast<MessageType>(json["type"]) == MessageType::AUTHENTICATION) {

                WebSocketMessage msg;
                msg.type = static_cast<MessageType>(json["type"]);
                msg.sender = json["sender"];
                msg.token = json["token"];


                return msg;
            }
            else {

                // C heck if required fields exist in the JSON
                if (!json.contains("type") ||
                    !json.contains("sender") ||
                    !json.contains("receiver") ||
                    !json.contains("content") ||
                    !json.contains("token") ||

                    !json.contains("timestamp")) {

                    // Prepare an error message
                    WebSocketMessage errorMsg;
                    errorMsg.type = MessageType::ERR;  // Set to appropriate error type
                    errorMsg.sender = json.contains("sender") ? json["sender"] : "ErrorSender";
                    errorMsg.receiver = json.contains("reciever") ? json["reciver"] : "ErrorReciver";
                    errorMsg.token = json.contains("token") ? json["token"] : "ErrorToken";

                    errorMsg.content = "Missing required fields: type, sender, or content.";
                    errorMsg.timestamp = "server_timestamp"; // Optionally set timestamp (server-side)



                    // Log the error for server-side visibility
                    utils::Logger::log("Error: Missing required fields in the message", utils::Logger::LogLevel::ERR);

                    return errorMsg;  // Exit early, no further processing needed
                }

                WebSocketMessage msg;
                //std::string pureMessageTimestamp = json["timestamp"];
                //msg.timestamp = formatTimestamp(pureMessageTimestamp);
                msg.type = static_cast<MessageType>(json["type"]);
                msg.sender = json["sender"];
                msg.receiver = json["receiver"];
                msg.content = json["content"];
                msg.token = json["token"];
                msg.timestamp = json["timestamp"];
                return msg;
            }

        }
    };

    //std::string formatTimestamp(const std::string& timestamp) {
    //    // Assuming timestamp is in ISO 8601 format and you need to format it
    //    std::tm tm = {};
    //    std::istringstream ss(timestamp);
    //    ss >> std::get_time(&tm, "%Y-%m-%dT%H:%M:%S");

    //    if (ss.fail()) {
    //        throw std::invalid_argument("Failed to parse timestamp");
    //    }

    //    return fmt::format("{:04}-{:02}-{:02} {:02}:{:02}:{:02}",
    //        tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday,
    //        tm.tm_hour, tm.tm_min, tm.tm_sec);
    //}

} // namespace ws

#endif // WEBSOCKET_TYPES_HPP