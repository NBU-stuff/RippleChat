#ifndef WEBSOCKET_SERVER_HPP
#define WEBSOCKET_SERVER_HPP

#include <crow.h>
#include "WebSocketHandler.hpp"
#include "../Headers/utils/Logger.hpp"
#include "../Headers/routes/LoginHandler.hpp"
#include <crow/middlewares/cors.h>
#include "../Headers/routes/RegisterHandler.hpp"
#include "../Headers/routes/FriendHandler.hpp"
#include "../Headers/routes/PublicUserHandler.hpp"
#include "../Headers/routes/FriendRequestHandler.hpp"

namespace ws {

    class WebSocketServer {
    public:
        WebSocketServer(int port) : port_(port) {
        }

        void start() {
            setupRoutes();
            setupWebSocket();

            utils::Logger::log("Starting server on port " + std::to_string(port_),
                utils::Logger::LogLevel::INFO);

            app_.port(port_)
                .multithreaded()
                .run();
        }

        void stop() {
            utils::Logger::log("Stopping server", utils::Logger::LogLevel::INFO);
            app_.stop();
        }

    private:
        void setupCORS() {
            // Enable CORS for all origins during development
            auto& middleware = app_.get_middleware<crow::CORSHandler>();

            // Configure CORS
            middleware
                .global()
                .headers("*")
                .methods("POST"_method, "GET"_method, "PUT"_method, "DELETE"_method, "OPTIONS"_method)
                .origin("*")
                .prefix("/auth") // Apply to /auth routes
                .prefix("/users"); // Apply to /users routes
        }

        void setupRoutes() {
            setupCORS();
            // Authentication routes
            CROW_ROUTE(app_, "/auth/login")
                .methods("POST"_method, "OPTIONS"_method)
                ([](const crow::request& req) {

                try {
                    auto bodyArgs = crow::json::load(req.body);

                    // Access JSON fields with proper type checking
                    if (!bodyArgs.has("username") || !bodyArgs.has("password")) {
                        return crow::response(400, "Missing username or password");
                    }

                    std::string username = bodyArgs["username"].s();  // .s() for string
                    std::string password = bodyArgs["password"].s();


                    return routes::LoginHandler::handleLogin("POST", username, password);


                }

                catch (const std::exception& e) {
                    return crow::response(500, std::string("Server error: ") + e.what());
                }
            });

            CROW_ROUTE(app_, "/auth/register").methods("POST"_method)
                ([](const crow::request& req) {
                try {
                    auto bodyArgs = crow::json::load(req.body);

                    // Access JSON fields with proper type checking
                    if (!bodyArgs.has("username") || !bodyArgs.has("password")) {
                        return crow::response(400, "Missing username or password");
                    }

                    std::string username = bodyArgs["username"].s();  // .s() for string
                    std::string email = bodyArgs["email"].s();
                    std::string password = bodyArgs["password"].s();


                    return routes::RegisterHandler::handleRegister("POST", username, password, email);

                }

                catch (const std::exception& e) {
                    return crow::response(400, "Invalid request");

                }

            });

            CROW_ROUTE(app_, "/user/friends").methods("GET"_method)
                ([&](const crow::request& req) {
                try {
                    std::string userId = req.url_params.get("userId");

                    // Extract query parameter 'username'



                    if (userId.empty()) {
                        return crow::response(400, "Missing 'id' query parameter");
                    }


                    // Extract JWT token from the "Authorization" header
                    auto jwtToken = req.get_header_value("Authorization");


                    // Check if the header starts with "Bearer " and extract the token
                    if (jwtToken.size() < 7 || jwtToken.substr(0, 7) != "Bearer ") {
                        return crow::response(401, "Unauthorized");
                    }

                    std::string token = jwtToken.substr(7);  // Extract token after "Bearer "

                    // Validate the token
                    if (!JwtHandler::validateToken(token, userId)) {
                        return crow::response(401, "Unauthorized");
                    }


                    return routes::FriendHandler::getFriends(userId);

                }
                catch (const std::exception& e) {
                    return crow::response(500, std::string("Error: ") + e.what());
                }
            });
            CROW_ROUTE(app_, "/user/friend/<int>").methods("GET"_method)
                ([&](const crow::request& req, int friendId) {
                try {
                    // Extract 'userId' from query parameters
                    std::string userId = req.url_params.get("userId");
                    if (userId.empty()) {
                        return crow::response(400, "Missing 'userId' query parameter");
                    }

                    // Extract JWT token from the "Authorization" header
                    auto jwtToken = req.get_header_value("Authorization");
                    if (jwtToken.size() < 7 || jwtToken.substr(0, 7) != "Bearer ") {
                        return crow::response(401, "Unauthorized");
                    }

                    std::string token = jwtToken.substr(7); // Extract token after "Bearer "

                    // Validate the token
                    if (!JwtHandler::validateToken(token, userId)) {
                        return crow::response(401, "Unauthorized");
                    }

                    // Successfully extract and validate data
                    std::cout << "User ID: " << userId << ", Friend ID: " << friendId << ", Token: " << token << std::endl;
                    return routes::FriendHandler::getFriend(userId, friendId);


                }
                catch (const std::exception& e) {
                    return crow::response(500, std::string("Error: ") + e.what());
                }
            });

            CROW_ROUTE(app_, "/user/messages/<int>").methods("GET"_method)
                ([&](const crow::request& req, int receiverId) {
                try {


                    // Extract 'userId' from query parameters
                    std::string userId = req.url_params.get("userId");
                    std::string before = req.url_params.get("before");
                    if (userId.empty()) {
                        return crow::response(400, "Missing 'userId' query parameter");
                    }

                    // Extract JWT token from the "Authorization" header
                    auto jwtToken = req.get_header_value("Authorization");
                    if (jwtToken.size() < 7 || jwtToken.substr(0, 7) != "Bearer ") {
                        return crow::response(401, "Unauthorized");
                    }

                    std::string token = jwtToken.substr(7); // Extract token after "Bearer "

                    // Validate the token
                    if (!JwtHandler::validateToken(token, userId)) {
                        return crow::response(401, "Unauthorized");
                    }

                    // Successfully extract and validate data
                    std::cout << "User ID: " << userId << ", Receiver ID: " << receiverId << ", Token: " << token << std::endl;

                    // Implement function to retrieve messages based on senderId and receiverId
                    // Replace this with your actual implementation using your database handler
                    return routes::MessageHandler::getMessages(userId, receiverId, before);



                }
                catch (const std::exception& e) {
                    return crow::response(500, std::string("Error: ") + e.what());
                }
            });

            CROW_ROUTE(app_, "/user/messages")
                .methods("GET"_method)
                ([&](const crow::request& req) {
                try {
                    // Extract 'userId' from query parameters
                    std::string userId = req.url_params.get("userId");
                    if (userId.empty()) {
                        return crow::response(400, "Missing 'userId' query parameter");
                    }

                    // Extract JWT token from the "Authorization" header
                    auto jwtToken = req.get_header_value("Authorization");
                    if (jwtToken.size() < 7 || jwtToken.substr(0, 7) != "Bearer ") {
                        return crow::response(401, "Unauthorized");
                    }

                    std::string token = jwtToken.substr(7); // Extract token after "Bearer "

                    // Validate the token
                    if (!JwtHandler::validateToken(token, userId)) {
                        return crow::response(401, "Unauthorized");
                    }



                    // Implement function to retrieve all messages for the user
                    return routes::MessageHandler::getAllUserMessages(userId);

                }
                catch (const std::exception& e) {
                    return crow::response(500, std::string("Error: ") + e.what());
                }
            });


            CROW_ROUTE(app_, "/user/messages/read")
                .methods("PUT"_method)
                ([&](const crow::request& req) {
                try {

                    // Extract JWT token from the "Authorization" header
                    auto jwtToken = req.get_header_value("Authorization");
                    if (jwtToken.size() < 7 || jwtToken.substr(0, 7) != "Bearer ") {
                        return crow::response(401, "Unauthorized");
                    }

                    std::string token = jwtToken.substr(7); // Extract token after "Bearer "

                    // Parse request body

                    auto body = crow::json::load(req.body);
                    if (!body || !body.has("sender_id") || !body.has("receiver_id")) {
                        return crow::response(400, "Missing required parameters");
                    }

                    std::string senderId = body["sender_id"].s();
                    std::string receiverId = body["receiver_id"].s();
                    std::string sent_at = body["sent_at"].s();
                    std::string readAt = body["read_at"].s();

                    // Validate JWT token against the receiver ID
                    if (!JwtHandler::validateToken(token, receiverId)) {
                        return crow::response(401, "Unauthorized");
                    }

                    // Call function to mark messages as read
                    return routes::MessageHandler::markMessagesAsRead(senderId, receiverId, sent_at, readAt);
                }
                catch (const std::exception& e) {
                    return crow::response(500, std::string("Error: ") + e.what());
                }
            });


            CROW_ROUTE(app_, "/users")
                .methods("GET"_method)
                ([&](const crow::request& req) {
                try {

                    std::string userId = req.url_params.get("userId");
                    // Extract 'letter' from query parameters (optional)
                    std::string letter = req.url_params.get("letter") ? req.url_params.get("letter") : "";
                    // Extract the "status" parameter to handle pending users
                    std::string status = req.url_params.get("status") ? req.url_params.get("status") : "";

                    // Extract JWT token from the "Authorization" header
                    auto jwtToken = req.get_header_value("Authorization");
                    if (jwtToken.size() < 7 || jwtToken.substr(0, 7) != "Bearer ") {
                        return crow::response(401, "Unauthorized");
                    }

                    std::string token = jwtToken.substr(7); // Extract token after "Bearer "

                    if (!JwtHandler::validateToken(token, userId)) {
                        return crow::response(401, "Unauthorized");
                    }

                    // Fetch users based on optional letter filter and the status (if specified)
                    if (status == "pending") {
                        return routes::PublicUserHandler::getPendingUsers(userId, letter);
                    }
                    else if (status == "sent") {
                        return routes::PublicUserHandler::getSentUsers(userId, letter);
                    }
                    else {
                        return routes::PublicUserHandler::getUsers(userId, letter); // Default, fetch accepted users
                    }

                }
                catch (const std::exception& e) {
                    return crow::response(500, std::string("Error: ") + e.what());
                }
            });


            CROW_ROUTE(app_, "/friendship").methods("POST"_method)
                ([](const crow::request& req) {
                try {
                    auto bodyArgs = crow::json::load(req.body);

                    // Ensure both senderId and receiverId are provided
                    if (!bodyArgs.has("user1_id") || !bodyArgs.has("user2_id")) {
                        return crow::response(400, "Sender and Receiver IDs are required");
                    }

                    std::string senderId = bodyArgs["user1_id"].s();  // Extract sender ID
                    std::string receiverId = bodyArgs["user2_id"].s();  // Extract receiver ID
                    std::string status = bodyArgs["status"].s();  // Extract status, should be "pending"

                    // Extract JWT token from the "Authorization" header
                    auto jwtToken = req.get_header_value("Authorization");
                    if (jwtToken.size() < 7 || jwtToken.substr(0, 7) != "Bearer ") {
                        return crow::response(401, "Unauthorized");
                    }

                    std::string token = jwtToken.substr(7); // Extract token after "Bearer "

                    if (!JwtHandler::validateToken(token, senderId)) {
                        return crow::response(401, "Unauthorized");
                    }

                    // Call the handler function to create the friend request
                    return routes::FriendRequestHandler::handleCreateFriendRequest(senderId, receiverId, status);
                }
                catch (const std::exception& e) {
                    return crow::response(400, "Invalid request");
                }
            });

            CROW_ROUTE(app_, "/friendship/accept").methods("PUT"_method)
                ([](const crow::request& req) {
                try {
                    // Parse the request body as JSON
                    auto bodyArgs = crow::json::load(req.body);

                    // Ensure both senderId and receiverId are provided
                    if (!bodyArgs.has("user1_id") || !bodyArgs.has("user2_id")) {
                        return crow::response(400, "Sender and Receiver IDs are required");
                    }



                    std::string senderId = bodyArgs["user1_id"].s();
                    std::string receiverId = bodyArgs["user2_id"].s();



                    std::cout << "senderId" << senderId << std::endl;
                    std::cout << "receiverId" << receiverId << std::endl;

                    // Extract JWT token from the "Authorization" header
                    auto jwtToken = req.get_header_value("Authorization");
                    if (jwtToken.size() < 7 || jwtToken.substr(0, 7) != "Bearer ") {
                        return crow::response(401, "Unauthorized");
                    }

                    std::string token = jwtToken.substr(7); // Extract token after "Bearer "

                    if (!JwtHandler::validateToken(token, receiverId)) {
                        return crow::response(401, "Unauthorized");
                    }



                    // Check if sender and receiver are the same
                    if (senderId == receiverId) {
                        return crow::response(400, "Sender and Receiver cannot be the same");
                    }

                    // Use the handler to process the accept request
                    return routes::FriendRequestHandler::handleAcceptFriendRequest(senderId, receiverId);
                }
                catch (const std::exception& e) {
                    return crow::response(400, "Invalid request");
                }
            });

            CROW_ROUTE(app_, "/friendship/remove").methods("DELETE"_method)
                ([](const crow::request& req) {
                try {
                    // Parse the request body as JSON
                    auto bodyArgs = crow::json::load(req.body);

                    // Ensure both user IDs are provided
                    if (!bodyArgs.has("senderId") || !bodyArgs.has("receiverId")) {
                        return crow::response(400, "User1 and User2 IDs are required");
                    }

                    std::string senderId = bodyArgs["senderId"].s();
                    std::string receiverId = bodyArgs["receiverId"].s();

                    std::cout << "senderId: " << senderId << std::endl;
                    std::cout << "receiverId: " << receiverId << std::endl;

                    // Extract JWT token from the "Authorization" header
                    auto jwtToken = req.get_header_value("Authorization");
                    if (jwtToken.size() < 7 || jwtToken.substr(0, 7) != "Bearer ") {
                        return crow::response(401, "Unauthorized");
                    }

                    std::string token = jwtToken.substr(7); // Extract token after "Bearer "

                    // Validate token (Ensure the requester has permission)
                    if (!JwtHandler::validateToken(token, senderId)) {
                        return crow::response(401, "Unauthorized");
                    }

                    // Check if user1 and user2 are the same
                    if (senderId == receiverId) {
                        return crow::response(400, "Cannot remove friendship with yourself");
                    }

                    // Use the handler to process the removal request
                    return routes::FriendRequestHandler::handleRemoveFriendship(senderId, receiverId);
                }
                catch (const std::exception& e) {
                    return crow::response(400, "Invalid request");
                }
            });





            // User routes
            CROW_ROUTE(app_, "/users/<int>").methods("GET"_method)
                ([](int user_id) {
                // TODO: Implement user profile retrieval
                return crow::response(200);
            });
        }

        void setupWebSocket() {
            CROW_WEBSOCKET_ROUTE(app_, "/ws")
                .onopen([](crow::websocket::connection& conn) {
                //std::cout << "DATA IN CONNECTION " << std::endl;
                WebSocketHandler::handleConnection(conn);
            })
                .onmessage([](crow::websocket::connection& conn, const std::string& data, bool is_binary) {
                std::cout << "DATA-BEFORE-HANDLER " << data << std::endl;




                WebSocketHandler::handleMessage(conn, data, is_binary);

            })
                .onclose([](crow::websocket::connection& conn, const std::string& reason) {
                WebSocketHandler::handleDisconnection(conn);
            });
        }
        crow::App<crow::CORSHandler> app_;
        int port_;
    };

} // namespace ws

#endif // WEBSOCKET_SERVER_HPP

