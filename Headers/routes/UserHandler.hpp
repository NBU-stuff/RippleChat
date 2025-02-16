#ifndef USERHANDLER_HPP
#define USERHANDLER_HPP

#include <string>

namespace routes {

    class UserHandler {
    public:

        static std::string HandleUsers(std::string& jsonData, const std::string& method);
        static std::string HandleUserById(std::string& jsonData, const std::string& method, std::string id);
        // Get a list of all users
        static std::string getAllUsers();

        // Get a specific user by ID
        static std::string getUserById(const std::string& userId);

        // Create a new user
        static std::string createUser(const std::string& request);

        // Update an existing user's details
        static std::string updateUser(const std::string& userId, const std::string& request);

        // Delete a user by ID
        static std::string deleteUser(const std::string& userId);
    };

}

#endif // USERHANDLER_HPP
