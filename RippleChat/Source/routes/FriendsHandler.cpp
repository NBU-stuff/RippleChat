#include <pqxx/pqxx>
#include <nlohmann/json.hpp>
#include <iostream>

#include "../Headers/routes/FriendHandler.hpp"
#include "../Headers/db/Database.hpp"
#include "../Headers/utils/HttpResponseBuilder.hpp"
namespace routes {
    crow::response FriendHandler::getFriends(const std::string& userId) {
        try {
            // Get database instance
            auto& db = Database::getInstance();

            // Step 1: Query to get both user1_id and user2_id where either user1_id or user2_id matches the provided userId
            // Also filter by the friendship status being 'accepted'
            auto friendIdsQuery = R"(
        SELECT user1_id, user2_id
        FROM friendships
        WHERE (user1_id = $1 OR user2_id = $1)
        AND status = 'accepted';
    )";

            auto friendIdsResult = db.executeQuery(friendIdsQuery, userId);

            // Prepare a JSON response
            nlohmann::json successResponse;
            successResponse["message"] = "Successful response";
            successResponse["friends"] = nlohmann::json::array();

            // Step 2: Iterate through the results and add each friend (either user1_id or user2_id) except the current userId
            for (const auto& row : friendIdsResult) {
                int friendUserId = (row["user1_id"].as<int>() == std::stoi(userId)) ? row["user2_id"].as<int>() : row["user1_id"].as<int>();

                // Skip if the friend is the user themselves
                if (friendUserId == std::stoi(userId)) continue;

                // Query the users table for each friend ID
                auto userQuery = R"(
            SELECT user_id, username
            FROM users
            WHERE user_id = $1;
        )";
                auto userResult = db.executeQuery(userQuery, friendUserId);

                if (!userResult.empty()) {
                    nlohmann::json friendJson;
                    friendJson["user_id"] = userResult[0]["user_id"].as<int>();
                    friendJson["username"] = userResult[0]["username"].as<std::string>();
                    successResponse["friends"].push_back(friendJson);
                }
            }

            // Now you can send the successResponse as part of your response
            return utils::build_http_response(successResponse.dump(), "200 OK");

        }
        catch (const std::exception& e) {
            nlohmann::json errorResponse;
            errorResponse["error"] = std::string("Error: ") + e.what();
            return utils::build_http_response(errorResponse.dump(), "500 Internal Server Error");
        }
    }

    crow::response FriendHandler::getFriend(const std::string& userId, int friendId) {
        try {
            // Get database instance
            auto& db = Database::getInstance();

            // Query to check if the provided friendId is actually a friend of the userId
            auto checkFriendshipQuery = R"(
            SELECT 1
            FROM friendships
            WHERE (user1_id = $1 AND user2_id = $2)
               OR (user1_id = $2 AND user2_id = $1);
        )";

            auto result = db.executeQuery(checkFriendshipQuery, userId, friendId);

            if (result.empty()) {
                nlohmann::json errorResponse;
                errorResponse["error"] = "The specified user is not a friend.";
                return utils::build_http_response(errorResponse.dump(), "404 Not Found");
            }

            // Query to get the friend's details from the users table
            auto friendQuery = R"(
            SELECT user_id, username
            FROM users
            WHERE user_id = $1;
        )";

            auto friendResult = db.executeQuery(friendQuery, friendId);

            if (friendResult.empty()) {
                nlohmann::json errorResponse;
                errorResponse["error"] = "Friend not found in the users table.";
                return utils::build_http_response(errorResponse.dump(), "404 Not Found");
            }

            // Prepare a JSON response with the friend's details
            nlohmann::json successResponse;
            successResponse["message"] = "Successful response";
            successResponse["friend"] = {
                {"user_id", friendResult[0]["user_id"].as<int>()},
                {"username", friendResult[0]["username"].as<std::string>()}
            };

            // Return successful response
            return utils::build_http_response(successResponse.dump(), "200 OK");

        }
        catch (const std::exception& e) {
            nlohmann::json errorResponse;
            errorResponse["error"] = std::string("Error: ") + e.what();
            return utils::build_http_response(errorResponse.dump(), "500 Internal Server Error");
        }
    }

}
