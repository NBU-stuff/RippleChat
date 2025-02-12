#ifndef FRIEND_HANDLER_HPP
#define FRIEND_HANDLER_HPP

#include <string>
#include <crow.h>
#include <nlohmann/json.hpp>
#include "../Headers/db/Database.hpp"
#include "../Headers/utils/HttpResponseBuilder.hpp"
#include "JwtHandler.hpp"
#include "../Headers/utils/PasswordHasher.hpp"

namespace routes {

    class FriendHandler {
    public:
        static crow::response getFriends(const std::string& username);
        static  crow::response getFriend(const std::string& userId, int friendId);
    };

} // namespace routes

#endif // FRIEND_HANDLER_HPP
