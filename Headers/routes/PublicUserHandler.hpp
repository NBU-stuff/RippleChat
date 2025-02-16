#ifndef PUBLIC_USER_HANDLER_HPP
#define PUBLIC_USER_HANDLER_HPP

#include "crow.h"
#include "../Headers/db/Database.hpp"
#include <string>
#include <nlohmann/json.hpp>


namespace routes {
    class PublicUserHandler {
    public:
        static crow::response getUsers(const std::string& userId, const std::string& letter);
        static   crow::response getPendingUsers(const std::string& userId, const std::string& letter);
        static crow::response getSentUsers(const std::string& userId, const std::string& letter);
    };
}

#endif // PUBLIC_USER_HANDLER_HPP
