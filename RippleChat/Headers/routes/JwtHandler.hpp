#ifndef JWT_HANDLER_HPP
#define JWT_HANDLER_HPP

#include <string>

#include <nlohmann/json.hpp>
#include <jwt-cpp/jwt.h>
#include <unordered_set>

class JwtHandler {
private:
    static std::unordered_set<std::string> blacklistedTokens;
public:
    static std::string generateToken(const std::string& userId, int expiryHours);
    static bool validateToken(const std::string& token, const std::string& expectedUserId);
    static void invalidateToken(const std::string& token);

};

#endif // JWT_HANDLER_HPP
