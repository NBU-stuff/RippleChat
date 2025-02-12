#include "../Headers/routes/FriendRequestHandler.hpp"

#include <pqxx/pqxx>
#include <nlohmann/json.hpp>
#include <iostream>

#include "../Headers/db/Database.hpp"
#include "../Headers/utils/HttpResponseBuilder.hpp"
namespace routes {

	crow::response FriendRequestHandler::handleCreateFriendRequest(const std::string& senderId, const std::string& receiverId, const std::string& status) {
		try {
			// Use database singleton to execute queries
			auto& db = Database::getInstance();

			// Check if the sender and receiver are different
			if (senderId == receiverId) {
				nlohmann::json errorResponse;
				errorResponse["error"] = "Sender and receiver cannot be the same.";
				return utils::build_http_response(errorResponse.dump(), "400 Bad Request");
			}

			// Check if a pending friend request already exists between sender and receiver
			std::string checkQuery = "SELECT 1 FROM friendships WHERE ((user1_id = $1 AND user2_id = $2) OR (user1_id = $2 AND user2_id = $1)) AND status IN ('accepted', 'pending') LIMIT 1";
			std::cout << "senderId" << senderId << "receiverId" << receiverId << std::endl;
			auto existingRequest = db.executeQuery(checkQuery, senderId, receiverId);
			if (!existingRequest.empty()) {
				nlohmann::json errorResponse;
				errorResponse["error"] = "Friend request already exists between these users.";
				return utils::build_http_response(errorResponse.dump(), "409 Conflict");
			}

			// Insert the friend request with the "pending" status
			std::string insertQuery = "INSERT INTO friendships (user1_id, user2_id, status) VALUES ($1, $2, $3)";
			db.executeQuery(insertQuery, senderId, receiverId, status);

			// Return success response
			nlohmann::json successResponse;
			successResponse["message"] = "Friend request sent successfully";
			return utils::build_http_response(successResponse.dump(), "201 Created");
		}
		catch (const std::exception& e) {
			nlohmann::json errorResponse;
			errorResponse["error"] = std::string("Error during friend request creation: ") + e.what();
			return utils::build_http_response(errorResponse.dump(), "500 Internal Server Error");
		}
	}


	 crow::response FriendRequestHandler::handleAcceptFriendRequest(const std::string& senderId, const std::string& receiverId) {
		try {
			// Use database singleton to execute queries
			auto& db = Database::getInstance();

			// Check if a pending friend request exists between sender and receiver
			std::string checkQuery = "SELECT friendship_id FROM friendships WHERE (user1_id = $1 AND user2_id = $2)  AND status = 'pending' LIMIT 1";
			auto existingRequest = db.executeQuery(checkQuery, senderId, receiverId);

			if (existingRequest.empty()) {
				nlohmann::json errorResponse;
				errorResponse["error"] = "No pending friend request found between the users.";
				return utils::build_http_response(errorResponse.dump(), "404 Not Found");
			}

			// Update the status to 'accepted'
			std::string updateQuery = "UPDATE friendships SET status = 'accepted' WHERE friendship_id = $1";
			db.executeQuery(updateQuery, existingRequest[0]["friendship_id"].as<std::string>());

			// Return success response
			nlohmann::json successResponse;
			successResponse["message"] = "Friend request accepted successfully";
			return utils::build_http_response(successResponse.dump(), "200 OK");
		}
		catch (const std::exception& e) {
			nlohmann::json errorResponse;
			errorResponse["error"] = std::string("Error during friend request acceptance: ") + e.what();
			return utils::build_http_response(errorResponse.dump(), "500 Internal Server Error");
		}
	}


	 crow::response routes::FriendRequestHandler::handleRemoveFriendship(const std::string& user1Id, const std::string& user2Id) {
		 try {
			 // Use database singleton to execute queries
			 auto& db = Database::getInstance();

			 // Check if a friendship exists between user1 and user2
			 std::string checkQuery =
				 "SELECT friendship_id FROM friendships WHERE "
				 "(user1_id = $1 AND user2_id = $2) OR (user1_id = $2 AND user2_id = $1) LIMIT 1";

			 auto existingFriendship = db.executeQuery(checkQuery, user1Id, user2Id);

			 if (existingFriendship.empty()) {
				 nlohmann::json errorResponse;
				 errorResponse["error"] = "No friendship found between the users.";
				 return utils::build_http_response(errorResponse.dump(), "404 Not Found");
			 }

			 // Delete the friendship
			 std::string deleteQuery = "DELETE FROM friendships WHERE friendship_id = $1";
			 db.executeQuery(deleteQuery, existingFriendship[0]["friendship_id"].as<std::string>());

			 // Return success response
			 nlohmann::json successResponse;
			 successResponse["message"] = "Friendship removed successfully";
			 return utils::build_http_response(successResponse.dump(), "200 OK");
		 }
		 catch (const std::exception& e) {
			 nlohmann::json errorResponse;
			 errorResponse["error"] = std::string("Error during friendship removal: ") + e.what();

		 }
	 }




};



