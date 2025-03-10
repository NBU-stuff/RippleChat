
#include <nlohmann/json.hpp>
#include <fmt/core.h>

#include "../../Headers/utils/HttpResponseBuilder.hpp"
#include "../../Headers/db/Database.hpp"
#include "../../Headers/routes/JwtHandler.hpp"
#include "../../Headers/routes/RegisterHandler.hpp"
#include "../../Headers/utils/PasswordHasher.hpp"


#include <crow.h>
using json = nlohmann::json;

crow::response routes::RegisterHandler::handleRegister(std::string method, const std::string& username, const std::string& password, const std::string& email) {
    try {


        // Use database singleton to execute queries
        auto& db = Database::getInstance();

        // Check if user exists
        std::string query = "SELECT 1 FROM users WHERE username = $1 LIMIT 1";
        auto userExists = db.executeQuery(query, username);
        std::cout << "userExists.empty() " << userExists.empty() << std::endl;

        if (!userExists.empty()) {
            nlohmann::json errorResponse;
            errorResponse["error"] = "Username already exists.";
            return utils::build_http_response(errorResponse.dump(), "409 Conflict");
        }

        // Hash the password before storing
        std::string hashedPassword = PasswordHasher::hashPassword(password);



        std::string registerQuery = "INSERT INTO users (username, email ,password_hash) VALUES ($1,$2, $3)";

        // Register new user
        auto registerResult = db.executeQuery(registerQuery, username, email, hashedPassword);
        std::string userQuery = "SELECT user_id, password_hash FROM users WHERE username = $1";

        auto userResult = db.executeQuery(userQuery, username);
        // Generate JWT token

        std::string userId = userResult[0]["user_id"].as<std::string>();
        std::cout << "userId " << userId << std::endl;

        int expiryHours = 168;
        std::string token = JwtHandler::generateToken(userId, expiryHours);

        // Return success response
        nlohmann::json successResponse;
        successResponse["message"] = "User registered successfully";
        successResponse["token"] = token;
        successResponse["userId"] = userId;
        return utils::build_http_response(successResponse.dump(), "201 Created");
    }
    catch (const nlohmann::json::parse_error& e) {
        nlohmann::json errorResponse;
        errorResponse["error"] = "Invalid JSON format.";
        return utils::build_http_response(errorResponse.dump(), "400 Bad Request");
    }
    catch (const std::exception& e) {
        nlohmann::json errorResponse;
        errorResponse["error"] = std::string("Error during registration: ") + e.what();
        return utils::build_http_response(errorResponse.dump(), "500 Internal Server Error");
    }

}