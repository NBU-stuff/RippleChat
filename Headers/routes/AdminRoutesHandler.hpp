#ifndef ADMINROUTEHANDLER_HPP
#define ADMINROUTEHANDLER_HPP

#include <string>


class AdminRouteHandler {
public:
    static std::string handleAdminRequest(const std::string& request, const std::string& method);
};


#endif 
