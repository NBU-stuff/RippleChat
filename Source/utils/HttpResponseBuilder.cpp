#include "../../Headers/utils/HttpResponseBuilder.hpp"

#include <crow.h>
namespace utils {
    crow::response build_http_response(const std::string& body, const std::string& status) {
        // Create a Crow response object
        crow::response res;

        std::cout << "Buidler-Response-Body" << body << std::endl;

        // Set the body (Crow will automatically set the appropriate Content-Type for strings)
        res.body = body;

        // Set the status code (Crow uses integer status codes, so you may need to map the status string)
        if (status == "200 OK") {
            res.code = 200;
        }
        else if (status == "404 Not Found") {
            res.code = 404;
        }
        else if (status == "409 Conflict") {

            res.code = 409;
        }
        else if (status == "500 Internal Server Error") {
            res.code = 500;
        }
        else {
            res.code = 200;  // Default to 200 if the status isn't recognized
        }

        // Add your custom headers
        res.add_header("Content-Type", "application/json");
        res.add_header("Access-Control-Allow-Origin", "*");
        res.add_header("Access-Control-Allow-Methods", "POST, GET, OPTIONS");
        res.add_header("Access-Control-Allow-Headers", "Content-Type, Authorization");
        res.add_header("Access-Control-Max-Age", "86400");  // 24 hours
        res.add_header("Connection", "close");

        return res;  // Return the Crow response object
    }
}
