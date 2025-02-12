#ifndef LOGINHANDLER_HPP
#define LOGINHANDLER_HPP
#include <crow.h>
#include <string>

namespace routes {
    class LoginHandler {
    public:
        static  crow::response handleLogin(std::string method, const std::string& username, const std::string& password);
    private:
        static bool verifyPassword(const std::string& inputPassword, const std::string& storedPassword);
    };
}

#endif // LOGINHANDLER_HPP