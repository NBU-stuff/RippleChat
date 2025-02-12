// friend_request_handler.h
#ifndef FRIEND_REQUEST_HANDLER_H
#define FRIEND_REQUEST_HANDLER_H

#include <string>
#include <crow.h>
#include <nlohmann/json.hpp>


namespace routes {
	class FriendRequestHandler {
	public:
		// Static function to handle creating a friend request
		static crow::response handleCreateFriendRequest(const std::string& senderId, const std::string& receiverId, const std::string& status);
		static crow::response handleAcceptFriendRequest(const std::string& senderId, const std::string& receiverId);
		static crow::response handleRemoveFriendship(const std::string& user1Id, const std::string& user2Id);
	};
}

#endif // FRIEND_REQUEST_HANDLER_H
