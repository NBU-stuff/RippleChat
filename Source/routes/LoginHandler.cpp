#include "../Headers/routes/LoginHandler.hpp"
#include <nlohmann/json.hpp>
#include <map>
#include <fmt/core.h>
#include <iostream>
#include "../Headers/utils/HttpResponseBuilder.hpp"
#include "../Headers/db/Database.hpp"
#include "../Headers/routes/JwtHandler.hpp"
#include "../Headers/utils/PasswordHasher.hpp"
#include "../Headers/utils/Logger.hpp"  // Include logger
#include <crow.h>

namespace routes {

    bool LoginHandler::verifyPassword(const std::string& inputPassword, const std::string& storedPassword) {
        return PasswordHasher::verifyPassword(inputPassword, storedPassword);
    }

    crow::response LoginHandler::handleLogin(std::string method, const std::string& username, const std::string& password) {
        try {
            utils::Logger::log("Login request received for username: " + username, utils::Logger::LogLevel::INFO);

            // Get database instance
            auto& db = Database::getInstance();
            utils::Logger::log("Database instance retrieved", utils::Logger::LogLevel::INFO);

            // Check if user exists and get their password
            std::string query = "SELECT user_id, password_hash FROM users WHERE username = $1";
            utils::Logger::log("Executing query: " + query, utils::Logger::LogLevel::INFO);

            auto userResult = db.executeQuery(query, username);
            std::cout << "Query returned " << userResult.size() << " rows." << std::endl;

            // Check if the result is valid
            if (!userResult.empty()) {
                try {
                    std::string userId = userResult[0]["user_id"].as<std::string>();
                    std::string storedPassword = userResult[0]["password_hash"].as<std::string>();
                    std::cout << "User ID: " << userId << ", Stored Password: " << storedPassword << std::endl;

                    // Continue with password verification here
                }
                catch (const std::exception& e) {
                    std::cout << "Error accessing result data: " << e.what() << std::endl;
                    // Return a response if needed
                }
            }
            else {
                std::cout << "No results found for username: " << username << std::endl;
                // Handle this case appropriately, e.g., return error response
            }



            // Extract user data
            std::string userId = userResult[0]["user_id"].as<std::string>();
            std::string storedPassword = userResult[0]["password_hash"].as<std::string>();
            utils::Logger::log("User found. ID: " + userId, utils::Logger::LogLevel::INFO);

            // Verify password
            if (!verifyPassword(password, storedPassword)) {
                utils::Logger::log("Password verification failed for user ID: " + userId, utils::Logger::LogLevel::WARN);
                nlohmann::json errorResponse;
                errorResponse["error"] = "Invalid username or password";
                return utils::build_http_response(errorResponse.dump(), "401 Unauthorized");
            }

            utils::Logger::log("Password verified successfully for user ID: " + userId, utils::Logger::LogLevel::INFO);

            // Generate JWT token
            int expiryHours = 168; // Tokens from login last longer than registration
            std::string token = JwtHandler::generateToken(userId, expiryHours);
            utils::Logger::log("JWT token generated for user ID: " + userId, utils::Logger::LogLevel::INFO);

            // Create success response
            nlohmann::json successResponse;
            successResponse["message"] = "Login successful";
            successResponse["token"] = token;
            successResponse["userId"] = userId;

            utils::Logger::log("Login successful for user ID: " + userId, utils::Logger::LogLevel::INFO);
            return utils::build_http_response(successResponse.dump(), "200 OK");
        }
        catch (const nlohmann::json::parse_error& e) {
            utils::Logger::log("JSON parsing error: " + std::string(e.what()), utils::Logger::LogLevel::ERR);
            nlohmann::json errorResponse;
            errorResponse["error"] = "Invalid JSON format";
            return utils::build_http_response(errorResponse.dump(), "400 Bad Request");
        }
        catch (const std::exception& e) {
            utils::Logger::log("Exception in login: " + std::string(e.what()), utils::Logger::LogLevel::ERR);
            nlohmann::json errorResponse;
            errorResponse["error"] = std::string("Error during login: ") + e.what();
            return utils::build_http_response(errorResponse.dump(), "500 Internal Server Error");
        }
    }
}
