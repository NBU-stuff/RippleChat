#ifndef WEBSOCKET_HANDLER_HPP
#define WEBSOCKET_HANDLER_HPP

#include <crow.h>
#include "WebSocketTypes.hpp"
#include "ConnectionManager.hpp"
#include "../Headers/routes/JwtHandler.hpp"
#include <string>
#include "../Headers/routes/MessageHandler.hpp"

namespace ws {

    class WebSocketHandler {
    public:
        static void handleConnection(crow::websocket::connection& conn) {
            // Connection established - waiting for authentication
            utils::Logger::log("New WebSocket connection established", utils::Logger::LogLevel::INFO);

            //// Extract the URL (this gives you the full WebSocket URL, e.g., ws://localhost:8080/ws?userId=12345)
            //std::string url = conn.get_request().url;
            //conn.

            //// Find the position of the query parameter (after '?')
            //size_t pos = url.find("userId=");
            //if (pos != std::string::npos) {
            //    // Extract userId by cutting out the value after 'userId='
            //    std::string userId = url.substr(pos + 7);  // 7 is the length of "userId="

            //    // You might want to split it by '&' if there are multiple parameters
            //    size_t endPos = userId.find('&');
            //    if (endPos != std::string::npos) {
            //        userId = userId.substr(0, endPos);
            //    }

            //    // Now, you have the userId, you can use it to authenticate the user
            //    utils::Logger::log("User ID extracted: " + userId, utils::Logger::LogLevel::INFO);

            //    // If you need to validate the userId or authenticate further, you can add that here

            //    // Add the connection to the manager
            //    ConnectionManager::getInstance().addConnection(userId, &conn);

            //}
            //else {
            //    // Handle the case where userId is not provided
            //    utils::Logger::log("No userId found in WebSocket URL", utils::Logger::LogLevel::ERR);
            //    conn.close("No userId provided");
            //}
        }


        static void handleMessage(crow::websocket::connection& conn, const std::string& data, bool is_binary) {
            if (is_binary) {
                utils::Logger::log("Binary messages not supported", utils::Logger::LogLevel::WARN);
                return;
            }

            try {
                nlohmann::json json = nlohmann::json::parse(data);





                auto message = WebSocketMessage::fromJson(json);

                std::cout << "Message-In-The-Handler-THE-Receiver " << message.receiver << std::endl;
                std::cout << "Message-In-The-Handler " << message.toJson() << std::endl;




                switch (message.type) {
                case MessageType::AUTHENTICATION:
                    handleAuthentication(conn, message);
                    break;
                case MessageType::CHAT_MESSAGE:
                    handleChatMessage(message);
                    break;
                case MessageType::USER_STATUS:
                    handleStatusUpdate(message);
                    break;
                case MessageType::TYPING_STATUS:
                    handleTypingStatus(message);
                    break;
                case MessageType::ERR:
                    handleErrorMessage(conn, message);
                default:
                    utils::Logger::log("Unknown message type received", utils::Logger::LogLevel::WARN);
                }
            }
            catch (const std::exception& e) {
                utils::Logger::log("Error processing message: " + std::string(e.what()),
                    utils::Logger::LogLevel::ERR);
            }
        }

        static void handleDisconnection(crow::websocket::connection& conn) {
            // TODO: Implement user cleanup on disconnection
            utils::Logger::log("WebSocket connection closed", utils::Logger::LogLevel::INFO);
        }

    private:
        static void handleAuthentication(crow::websocket::connection& conn, const WebSocketMessage& message) {
            try {
                // Verify JWT token from message
                std::string token = message.token; // Token should be in content
                std::string sender = message.sender; // Token should be in content


                if (JwtHandler::validateToken(token, sender)) {

                    // Add connection to manager
                    ConnectionManager::getInstance().addConnection(message.sender, &conn);

                    // Send success response
                    WebSocketMessage response{
                        MessageType::AUTHENTICATION,
                        "server",
                        "Authentication successful",
                        ""  // Timestamp will be set on client side
                    };
                    conn.send_text(response.toJson().dump());

                }
                else {
                    // Send error response and close connection
                    WebSocketMessage response{
                        MessageType::ERR,
                        "server",
                        "Authentication failed",
                        ""
                    };
                    conn.send_text(response.toJson().dump());
                    conn.close("Authentication failed");
                }


            }
            catch (const std::exception& e) {
                utils::Logger::log("Authentication error: " + std::string(e.what()),
                    utils::Logger::LogLevel::ERR);
            }
        }


        static void handleChatMessage(const WebSocketMessage& message) {
            // Broadcast message to user
            ConnectionManager::getInstance().sendToUser(message.receiver, message);


            // TODO: Store message in database
            routes::MessageHandler::storeMessage(message.sender, message.receiver, message.content, message.timestamp);
        }
        static void handleErrorMessage(crow::websocket::connection& conn, const WebSocketMessage& errorMsg) {

            conn.send_text(errorMsg.toJson().dump());


            utils::Logger::log("Error: Missing required fields in the message", utils::Logger::LogLevel::ERR);

        }

        static void handleStatusUpdate(const WebSocketMessage& message) {
            // Broadcast status update to all users
            ConnectionManager::getInstance().broadcast(message.sender, message);
        }

        static void handleTypingStatus(const WebSocketMessage& message) {
            // Broadcast typing status to all users in the chat
            ConnectionManager::getInstance().broadcast(message.sender, message);
        }
    };

} // namespace ws

#endif // WEBSOCKET_HANDLER_HPP