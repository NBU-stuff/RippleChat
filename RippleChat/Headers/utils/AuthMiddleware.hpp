#ifndef AUTH_MIDDLEWARE_HPP
#define AUTH_MIDDLEWARE_HPP


#include <string>

#include "JwtHandler.hpp"
#include "HttpResponseBuilder.hpp"
#include "HttpParser.hpp"
using namespace utils;

class AuthMiddleware {
public:
    static bool validateAuth(const HttpRequest& request, std::string& error) {
        // Skip authentication for login and register endpoints
        if (request.path == "/auth/login" || request.path == "/auth/register") {
            return true;
        }

        std::string token = request.getAuthToken();
        if (token.empty()) {
            error = "No authorization token provided";
            return false;
        }

        // Your JWT secret key should be stored securely (e.g., in Config)
        const std::string secretKey = "your-secret-key"; // Replace with your actual secret key

        if (!JwtHandler::validateToken(token, secretKey)) {
            error = "Invalid or expired token";
            return false;
        }

        return true;
    }

    static std::string handleUnauthorized(const std::string& error) {
        nlohmann::json response;
        response["error"] = error;
        return utils::build_http_response(response.dump(), "401 Unauthorized");
    }
};

#endif // AUTH_MIDDLEWARE_HPP