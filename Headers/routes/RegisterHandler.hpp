#ifndef REGISTER_HANDLER_HPP
#define REGISTER_HANDLER_HPP

#include <string>
#include <crow.h>
namespace routes {


    class RegisterHandler {
    public:
        static crow::response handleRegister(std::string method, const std::string& username, const std::string& password, const std::string& email); // Keep handleRegister static
        //static void insertUser(pqxx::connection& conn, const std::string& username, const std::string& hashedPassword);  // Make insertUser static
    };


}

#endif // REGISTER_HANDLER_HPP