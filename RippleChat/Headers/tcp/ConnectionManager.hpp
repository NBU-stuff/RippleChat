#ifndef CONNECTION_MANAGER_HPP
#define CONNECTION_MANAGER_HPP

#include <unordered_map>
#include <mutex>
#include <crow.h>
#include "WebSocketTypes.hpp"
#include "../Headers/utils/Logger.hpp"

namespace ws {

    class ConnectionManager {
    public:
        static ConnectionManager& getInstance() {
            static ConnectionManager instance;
            return instance;
        }

        // Add a new connection
        void addConnection(const std::string& userId, crow::websocket::connection* conn) {
            std::lock_guard<std::mutex> lock(mutex_);
            connections_[userId] = conn;
            utils::Logger::log("User connected: " + userId, utils::Logger::LogLevel::INFO);
        }

        // Remove a connection
        void removeConnection(const std::string& userId) {
            std::lock_guard<std::mutex> lock(mutex_);
            connections_.erase(userId);
            utils::Logger::log("User disconnected: " + userId, utils::Logger::LogLevel::INFO);
        }

        // Send message to a specific user
        void sendToUser(const std::string& receiverId, const WebSocketMessage& message) {


            std::cout << "THE RECEIVING ID" << receiverId << std::endl;
            std::lock_guard<std::mutex> lock(mutex_);
            auto it = connections_.find(receiverId);


            /* std::cout << "Connection " << connections_.find(receiverId) << std::endl;*/

            if (it != connections_.end() && it->second != nullptr) {

                std::cout << "IN THE CONNECTIN IF STATEMENT" << std::endl;


                it->second->send_text(message.toJson().dump());
            }
        }

        // Broadcast message to all users except sender
        void broadcast(const std::string& senderId, const WebSocketMessage& message) {
            std::lock_guard<std::mutex> lock(mutex_);
            for (const auto& [userId, conn] : connections_) {
                if (userId != senderId && conn != nullptr) {
                    conn->send_text(message.toJson().dump());
                }
            }
        }

        // Check if user is connected
        bool isUserConnected(const std::string& userId) {
            std::lock_guard<std::mutex> lock(mutex_);
            return connections_.find(userId) != connections_.end();
        }

        // Get number of active connections
        size_t getActiveConnections() {
            std::lock_guard<std::mutex> lock(mutex_);
            return connections_.size();
        }

    private:
        ConnectionManager() = default;
        ~ConnectionManager() = default;
        ConnectionManager(const ConnectionManager&) = delete;
        ConnectionManager& operator=(const ConnectionManager&) = delete;

        std::unordered_map<std::string, crow::websocket::connection*> connections_;
        std::mutex mutex_;
    };

} // namespace ws

#endif // CONNECTION_MANAGER_HPP