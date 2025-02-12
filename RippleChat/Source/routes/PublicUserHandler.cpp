#include "../Headers/routes/PublicUserHandler.hpp"
#include "../Headers/utils/HttpResponseBuilder.hpp"





crow::response routes::PublicUserHandler::getUsers(const std::string& userId, const std::string& letter) {
    try {

        std::cout << "LETTER: " << letter << std::endl;

        // Get database instance
        auto& db = Database::getInstance();

        // Base query to fetch users
        std::string getUsersQuery = R"(
    SELECT user_id, username, email, created_at
    FROM users u
    WHERE u.user_id NOT IN (
        SELECT f.user1_id
        FROM friendships f
        WHERE (f.user1_id = $1 OR f.user2_id = $1)
        AND f.status IN ('accepted', 'pending')
        UNION
        SELECT f.user2_id
        FROM friendships f
        WHERE (f.user1_id = $1 OR f.user2_id = $1)
        AND f.status IN ('accepted', 'pending')
    )
)";


        std::string properLetter;
        pqxx::result usersResult;

        // Apply filter if 'letter' is provided
        if (!letter.empty()) {
            getUsersQuery += " AND u.username ILIKE $2";
            properLetter = "%" + letter + "%";
            getUsersQuery += " ORDER BY u.created_at DESC LIMIT 7;";
            usersResult = db.executeQuery(getUsersQuery, userId, properLetter);
        }
        else {
            getUsersQuery += " ORDER BY u.created_at DESC LIMIT 7;";
            usersResult = db.executeQuery(getUsersQuery, userId);
        }

        // Prepare JSON response
        nlohmann::json successResponse;
        successResponse["message"] = "Users retrieved successfully";
        successResponse["users"] = nlohmann::json::array();

        for (const auto& row : usersResult) {
            successResponse["users"].push_back({
                {"user_id", row["user_id"].as<int>()},
                {"username", row["username"].as<std::string>()},
                {"email", row["email"].as<std::string>()},
                {"created_at", row["created_at"].as<std::string>()}
                });
        }

        // Return success response
        return utils::build_http_response(successResponse.dump(), "200 OK");
    }
    catch (const std::exception& e) {
        std::cerr << "Database query failed: " << e.what() << std::endl;
        return utils::build_http_response("{\"error\": \"Failed to retrieve users\"}", "500 Internal Server Error");
    }
}

    
    crow::response routes::PublicUserHandler::getPendingUsers(const std::string& userId, const std::string& letter) {
        try {
            // Get database instance
            auto& db = Database::getInstance();

            // Base query to fetch pending users that are in a pending friendship with userId
            std::string getPendingUsersQuery = R"(
    SELECT u.user_id, u.username, u.email, u.created_at
    FROM users u
    INNER JOIN friendships f 
        ON (f.user1_id = u.user_id OR f.user2_id = u.user_id)
    WHERE f.status = 'pending'
    AND f.user2_id = $1
    AND u.user_id != $1
)";



            std::string properLetter = "";
            pqxx::result pendingUsersResult;
            // Apply letter filter if it's provided
            if (!letter.empty()) {
                getPendingUsersQuery += " AND u.username ILIKE $2 ";
                properLetter = "%" + letter + "%";
            std::cout << "LETTER STUFF" << properLetter << std::endl;
                pendingUsersResult = db.executeQuery(getPendingUsersQuery, userId, properLetter);
            }

            // Execute query
            
            std::cout << "LETTER STUFF" << properLetter << std::endl;
          
            pendingUsersResult = db.executeQuery(getPendingUsersQuery, userId);

            // Prepare JSON response
            nlohmann::json successResponse;
            successResponse["message"] = "Pending users retrieved successfully";
            successResponse["users"] = nlohmann::json::array();

            for (const auto& row : pendingUsersResult) {
                successResponse["users"].push_back({
                    {"user_id", row["user_id"].as<int>()},
                    {"username", row["username"].as<std::string>()},
                    {"email", row["email"].as<std::string>()},
                    {"created_at", row["created_at"].as<std::string>()}
                    });
            }

            // Return the success response
            return utils::build_http_response(successResponse.dump(), "200 OK");

        }
        catch (const std::exception& e) {
            std::cerr << "Database query failed: " << e.what() << std::endl;
            return utils::build_http_response("{\"error\": \"Failed to retrieve pending users\"}", "500 Internal Server Error");
        }
    }

    crow::response routes::PublicUserHandler::getSentUsers(const std::string& userId, const std::string& letter) {
        try {
            // Get database instance
            auto& db = Database::getInstance();

            // Base query to fetch sent users that are in a pending friendship with userId
            std::string getSentUsersQuery = R"(
            SELECT u.user_id, u.username, u.email, u.created_at
            FROM users u
            INNER JOIN friendships f 
                ON (f.user1_id = u.user_id OR f.user2_id = u.user_id)
            WHERE f.status = 'pending'
            AND f.user1_id = $1
            AND u.user_id != $1
        )";

            std::string properLetter = "";
            pqxx::result sentUsersResult;

            // Apply letter filter if it's provided
            if (!letter.empty()) {
                getSentUsersQuery += " AND u.username ILIKE $2 ";
                properLetter = "%" + letter + "%";
                std::cout << "LETTER STUFF: " << properLetter << std::endl;
                sentUsersResult = db.executeQuery(getSentUsersQuery, userId, properLetter);
            }
            else {
                // Execute query without letter filter
                sentUsersResult = db.executeQuery(getSentUsersQuery, userId);
            }

            // Prepare JSON response
            nlohmann::json successResponse;
            successResponse["message"] = "Sent users retrieved successfully";
            successResponse["users"] = nlohmann::json::array();

            // Process the query result and create JSON response
            for (const auto& row : sentUsersResult) {
                successResponse["users"].push_back({
                    {"user_id", row["user_id"].as<int>()},
                    {"username", row["username"].as<std::string>()},
                    {"email", row["email"].as<std::string>()},
                    {"created_at", row["created_at"].as<std::string>()}
                    });
            }

            // Return the success response
            return utils::build_http_response(successResponse.dump(), "200 OK");

        }
        catch (const std::exception& e) {
            std::cerr << "Database query failed: " << e.what() << std::endl;
            return utils::build_http_response("{\"error\": \"Failed to retrieve sent users\"}", "500 Internal Server Error");
        }
    }
