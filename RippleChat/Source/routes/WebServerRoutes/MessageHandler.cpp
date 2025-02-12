#include "../../../Headers/routes/MessageHandler.hpp"
#include "../../../Headers/db/Database.hpp"
#include <pqxx/pqxx>
#include <nlohmann/json.hpp>
#include <fmt/core.h>


namespace routes {

	crow::response MessageHandler::markMessagesAsRead(const std::string& senderId, const std::string& receiverId, const std::string& sentAt, const std::string& readAt) {
		try {
			nlohmann::json successResponse;
			// Get database instance
			auto& db = Database::getInstance();

			// Query to update unread messages before or at a given timestamp
			std::string updateQuery = "UPDATE messages SET read_at = $1 "
				"WHERE sender_id = $2 AND receiver_id = $3 "
				"AND sent_at <= $4 AND read_at IS NULL";

			// Execute the update query
			auto result = db.executeQuery(updateQuery, readAt, senderId, receiverId, sentAt);

			// Check if any messages were updated
			if (result.affected_rows() == 0) {
				successResponse["error"] = "Issue with setMessageAsRead";
				utils::Logger::log("No unread messages found to update", utils::Logger::LogLevel::WARN);
				return utils::build_http_response(successResponse.dump(), "404 Not Found ");
			}

			successResponse["message"] = "Successfully marked messages as read";
			utils::Logger::log("Successfully marked messages as read", utils::Logger::LogLevel::INFO);
			return utils::build_http_response(successResponse.dump(), "200 OK");
		}
		catch (const std::exception& e) {
			nlohmann::json errorResponse;
			std::cerr << "Error marking messages as read: " << e.what() << std::endl;
			utils::Logger::log("Error marking messages as read: " + std::string(e.what()), utils::Logger::LogLevel::ERR);
			errorResponse["error"] = "Error marking messages as read: ";
			return utils::build_http_response(errorResponse.dump(), "500 Internal Server Error");
		}
	}


	bool MessageHandler::storeMessage(const std::string& senderId, const std::string& receiverId, const std::string& content, const std::string& timestamp) {
		try {
			// Get database instance
			auto& db = Database::getInstance();

			// Query to get the friendship_id for the sender and receiver
			std::string friendshipQuery = "SELECT friendship_id FROM friendships WHERE "
				"(user1_id = $1 AND user2_id = $2) OR (user1_id = $2 AND user2_id = $1) LIMIT 1";

			// Fetch the friendship_id
			auto friendshipResult = db.executeQuery(friendshipQuery, senderId, receiverId);

			// Check if the friendship_id exists
			if (friendshipResult.empty()) {
				nlohmann::json errorResponse;
				utils::Logger::log("Friendship not found between the users", utils::Logger::LogLevel::ERR);
				return false;  // No friendship exists, handle the error accordingly
			}

			int friendshipId = friendshipResult[0]["friendship_id"].as<int>();

			// Now, proceed with inserting the message, including the friendship_id
			std::string messageQuery = "INSERT INTO messages (sender_id, receiver_id, content, friendship_id , sent_at) "
				"VALUES ($1, $2, $3, $4, NOW() AT TIME ZONE 'UTC')";



			// Insert the message into the database
			auto result = db.executeQuery(messageQuery, senderId, receiverId, content, friendshipId);

			// Check if the insert was successful (by checking the affected rows)
			if (result.affected_rows() == 0) {
				std::cout << "[ERROR] Failed to store message for senderId: " << senderId << " and receiverId: " << receiverId << std::endl;
				utils::Logger::log("Issue in store Message Handler", utils::Logger::LogLevel::ERR);
				return false;  // Error inserting the message
			}

			utils::Logger::log("Successfully stored message", utils::Logger::LogLevel::INFO);
			return true;
		}
		catch (const std::exception& e) {
			std::cerr << "Error storing message: " << e.what() << std::endl;
			return false;
		}
	}

	crow::response MessageHandler::getMessages(const std::string& senderId, int receiverId, const std::string& before = "") {
		try {
			// Get database instance
			auto& db = Database::getInstance();

			// Update read_at for unread messages before retrieving them
			std::string updateReadAtQuery = R"(
            UPDATE messages 
            SET read_at = NOW() AT TIME ZONE 'UTC' 
            WHERE read_at IS NULL AND receiver_id = $1 
            AND sender_id = $2
        )";

			db.executeQuery(updateReadAtQuery, receiverId, senderId); // Mark as read for the receiver

			// Query to fetch latest 30 messages
			std::string getMessagesQuery = R"(
            SELECT m.message_id, m.sender_id, m.receiver_id, m.content, m.sent_at, m.read_at, ms.read_at AS message_read_at
            FROM messages m
            LEFT JOIN message_status ms ON m.message_id = ms.message_id
            WHERE ((m.sender_id = $1 AND m.receiver_id = $2) OR (m.sender_id = $2 AND m.receiver_id = $1))
        )";

			// If 'before' timestamp is provided, fetch messages older than it
			if (!before.empty()) {
				getMessagesQuery += " AND m.sent_at < $3";
			}

			getMessagesQuery += " ORDER BY m.sent_at DESC LIMIT 30;";

			// Execute the query with or without 'before' parameter
			pqxx::result messagesResult;
			if (!before.empty()) {
				messagesResult = db.executeQuery(getMessagesQuery, senderId, receiverId, before);
			}
			else {
				messagesResult = db.executeQuery(getMessagesQuery, senderId, receiverId);
			}

			// Check if there are no more messages
			if (messagesResult.empty()) {
				nlohmann::json errorResponse;
				errorResponse["error"] = "No more messages found.";
				errorResponse["hasMessages"] = false;
				errorResponse["messages"] = nlohmann::json::array();
				errorResponse["next_before"] = ""; // Indicate no more messages are available
				return utils::build_http_response(errorResponse.dump(), "200 OK");
			}

			// Prepare a JSON response with the messages
			nlohmann::json successResponse;
			successResponse["message"] = "Messages retrieved successfully";
			successResponse["messages"] = nlohmann::json::array();

			std::string lastMessageTimestamp = "";

			for (const auto& row : messagesResult) {
				try {
					int messageId = row["message_id"].as<int>();
					int senderIdField = row["sender_id"].as<int>();
					int receiverIdField = row["receiver_id"].as<int>();
					std::string contentField = row["content"].as<std::string>();
					std::string sentAtField = row["sent_at"].as<std::string>();

					auto readAtField = row["read_at"].is_null() ? "" : row["read_at"].as<std::string>();
					auto messageReadAtField = row["message_read_at"].is_null() ? "" : row["message_read_at"].as<std::string>();

					successResponse["messages"].push_back({
						{"message_id", messageId},
						{"sender", senderIdField},
						{"receiver", receiverIdField},
						{"content", contentField},
						{"sent_at", sentAtField},
						{"read_at", readAtField},
						{"message_read_at", messageReadAtField}
						});

					lastMessageTimestamp = sentAtField; // Store last message timestamp for pagination
				}
				catch (const std::exception& ex) {
					std::cerr << "Error while processing row: " << ex.what() << std::endl;
					return utils::build_http_response("{\"error\": \"Failed to retrieve messages\"}", "500 Internal Server Error");
				}
			}

			// Add 'before' timestamp for the next request
			successResponse["next_before"] = lastMessageTimestamp;
			successResponse["hasMessages"] = true;
			return utils::build_http_response(successResponse.dump(), "200 OK");

		}
		catch (const std::exception& e) {
			std::cerr << "Database query failed: " << e.what() << std::endl;
			return utils::build_http_response("{\"error\": \"Failed to retrieve messages\"}", "500 Internal Server Error");
		}
	}

	crow::response MessageHandler::getAllUserMessages(const std::string& userId) {
		try {
			// Get database instance
			auto& db = Database::getInstance();

			// Step 1: Get all friendships for the user
			std::string getFriendshipsQuery = R"(
            SELECT friendship_id, user1_id, user2_id
            FROM friendships
            WHERE user1_id = $1 OR user2_id = $1
            AND status = 'accepted';
        )";

			pqxx::result friendshipsResult = db.executeQuery(getFriendshipsQuery, userId);

			// Prepare the response JSON
			nlohmann::json successResponse;
			successResponse["message"] = "All messages retrieved successfully";
			successResponse["messages_by_receiver"] = nlohmann::json::array();

			// Step 2: Fetch messages for each friendship
			for (const auto& friendshipRow : friendshipsResult) {
				int friendshipId = friendshipRow["friendship_id"].as<int>();
				int user1Id = friendshipRow["user1_id"].as<int>();
				int user2Id = friendshipRow["user2_id"].as<int>();

				// Determine who is the sender and receiver based on userId
				int otherUserId = (std::stoi(userId) == user1Id) ? user2Id : user1Id;

				// Query messages for the friendship
				std::string getMessagesQuery = R"(
    SELECT m.message_id, m.sender_id, m.receiver_id, m.content, m.sent_at, m.read_at
    FROM messages m
    WHERE (m.sender_id = $1 AND m.receiver_id = $2) 
       OR (m.sender_id = $2 AND m.receiver_id = $1)
    ORDER BY m.sent_at DESC
    LIMIT 3;
)";

				pqxx::result messagesResult = db.executeQuery(getMessagesQuery, userId, std::to_string(otherUserId));

				// Prepare messages for this friendship
				nlohmann::json messagesForFriend;
				messagesForFriend["friendship_id"] = friendshipId;
				messagesForFriend["friend_id"] = otherUserId;
				messagesForFriend["messages"] = nlohmann::json::array();

				// Add each message to the corresponding friendship
				for (const auto& row : messagesResult) {
					int messageId = row["message_id"].as<int>();
					int senderId = row["sender_id"].as<int>();
					int receiverId = row["receiver_id"].as<int>();
					std::string content = row["content"].as<std::string>();
					std::string sentAt = row["sent_at"].as<std::string>();

					// Handle nullable fields safely
					auto readAt = row["read_at"].is_null() ? "" : row["read_at"].as<std::string>();


					// Add message to the friendship's messages array
					messagesForFriend["messages"].push_back({
						{"message_id", messageId},
						{"sender", senderId},
						{"receiver", receiverId},
						{"content", content},
						{"sent_at", sentAt},
						{"read_at", readAt},

						});
				}

				// Add this friendship's messages to the response
				successResponse["messages_by_receiver"].push_back(messagesForFriend);
			}

			// Return the successful response
			return utils::build_http_response(successResponse.dump(), "200 OK");

		}
		catch (const std::exception& e) {
			std::cerr << "Database query failed: " << e.what() << std::endl;
			return utils::build_http_response("{\"error\": \"Failed to retrieve all messages\"}", "500 Internal Server Error");
		}
	}


	//crow::response MessageHandler::handleSendMessage(const std::string& senderId, const std::string& receiverId, const std::string& content, const std::string& timestamp) {
	//    try {
	//        // Ensure both sender and receiver exist
	//        auto& db = Database::getInstance();

	//        // Check if receiver exists in the database
	//        auto receiverResult = db.executeQuery([&](pqxx::work& txn) {
	//            std::string query = fmt::format(
	//                "SELECT user_id FROM users WHERE user_id = {}",
	//                receiverId
	//            );
	//            return txn.exec(query);
	//        });

	//        if (receiverResult.empty()) {
	//            nlohmann::json errorResponse;
	//            errorResponse["error"] = "Receiver not found";
	//            return utils::build_http_response(errorResponse.dump(), "404 Not Found");
	//        }

	//        // Store the message
	//        if (!storeMessage(senderId, receiverId, content, timestamp)) {
	//            nlohmann::json errorResponse;
	//            errorResponse["error"] = "Failed to store message";
	//            return utils::build_http_response(errorResponse.dump(), "500 Internal Server Error");
	//        }

	//        // Prepare success response
	//        nlohmann::json successResponse;
	//        successResponse["message"] = "Message sent successfully";

	//        return utils::build_http_response(successResponse.dump(), "200 OK");
	//    }
	//    catch (const std::exception& e) {
	//        nlohmann::json errorResponse;
	//        errorResponse["error"] = std::string("Error while sending message: ") + e.what();
	//        return utils::build_http_response(errorResponse.dump(), "500 Internal Server Error");
	//    }
	//}

}
