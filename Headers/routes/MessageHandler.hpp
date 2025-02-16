#ifndef MESSAGE_HANDLER_HPP
#define MESSAGE_HANDLER_HPP

#include <nlohmann/json.hpp>
#include <pqxx/pqxx>
#include <string>
#include <fmt/core.h>
#include <crow.h>
#include "../Headers/db/Database.hpp"
#include "../Headers/utils/HttpResponseBuilder.hpp"

namespace routes {

    class MessageHandler {
    public:
        static bool storeMessage(const std::string& senderId, const std::string& receiverId, const std::string& content, const std::string& timestamp);
        static crow::response getMessages(const std::string& senderId, int receiverId, const std::string& before);
        static crow::response markMessagesAsRead(const std::string& senderId, const std::string& receiverId, const std::string& sentAt, const std::string& readAt);
        static crow::response getAllUserMessages(const std::string& userId);
        //static crow::response handleSendMessage(const std::string& senderId, const std::string& receiverId, const std::string& content, const std::string& timestamp);
    };

}

#endif // MESSAGE_HANDLER_HPP