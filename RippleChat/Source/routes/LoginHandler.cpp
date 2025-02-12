

#include "../Headers/routes/LoginHandler.hpp"
#include <nlohmann/json.hpp>
#include <map>
#include <fmt/core.h>
#include <iostream>
#include "../Headers/utils/HttpResponseBuilder.hpp"
#include "../Headers/db/Database.hpp"
#include "../Headers/routes/JwtHandler.hpp"
#include "../Headers/utils/PasswordHasher.hpp"
#include <crow.h>
namespace routes {

    bool LoginHandler::verifyPassword(const std::string& inputPassword, const std::string& storedPassword) {


        return PasswordHasher::verifyPassword(inputPassword, storedPassword);
    }

    crow::response LoginHandler::handleLogin(std::string method, const std::string& username, const std::string& password) {
        try {
            //// Parse the JSON request
            //nlohmann::json requestData = nlohmann::json::parse(jsonData);

            //// Validate required fields
            //if (requestData.find("username") == requestData.end() ||
            //    requestData.find("password") == requestData.end()) {
            //    nlohmann::json errorResponse;
            //    errorResponse["error"] = "Invalid JSON data. 'username' and 'password' are required.";
            //    return utils::build_http_response(errorResponse.dump(), "400 Bad Request");
            //}

            //std::string username = requestData["username"];
            //std::string password = requestData["password"];


            // Get database instance
            auto& db = Database::getInstance();

            // Check if user exists and get their password
            std::string query = "SELECT user_id, password_hash FROM users WHERE username = $1";


            auto userResult = db.executeQuery(query, username);

            if (userResult.empty()) {
                nlohmann::json errorResponse;
                errorResponse["error"] = "Invalid username or password";
                return utils::build_http_response(errorResponse.dump(), "401 Unauthorized");
            }

            // Verify password
            std::string userId = userResult[0]["user_id"].as<std::string>();
            std::cout << "userId " << userId << std::endl;
            std::string storedPassword = userResult[0]["password_hash"].as<std::string>();
            if (!verifyPassword(password, storedPassword)) {
                nlohmann::json errorResponse;
                errorResponse["error"] = "Invalid username or password";
                return utils::build_http_response(errorResponse.dump(), "401 Unauthorized");
            }

            // Generate JWT token


            int expiryHours = 168; // Tokens from login last longer than registration
            std::string token = JwtHandler::generateToken(userId, expiryHours);

            // Create success response
            nlohmann::json successResponse;
            successResponse["message"] = "Login successful";
            successResponse["token"] = token;
            successResponse["userId"] = userId;

            return utils::build_http_response(successResponse.dump(), "200 OK");
        }
        catch (const nlohmann::json::parse_error& e) {
            nlohmann::json errorResponse;
            errorResponse["error"] = "Invalid JSON format";
            return utils::build_http_response(errorResponse.dump(), "400 Bad Request");
        }
        catch (const std::exception& e) {
            nlohmann::json errorResponse;
            errorResponse["error"] = std::string("Error during login: ") + e.what();
            return utils::build_http_response(errorResponse.dump(), "500 Internal Server Error");
        }
    }

}