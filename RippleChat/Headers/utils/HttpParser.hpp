#ifndef HTTP_PARSER_HPP
#define HTTP_PARSER_HPP

#include <string>
#include <map>


namespace utils {




    struct HttpRequest {
        std::string method;
        std::string path;
        std::string version;
        std::map<std::string, std::string> headers;
        std::string body;

        // Helper method to get Authorization token
        std::string getAuthToken() const {
            auto it = headers.find("Authorization");
            if (it != headers.end()) {
                std::string auth = it->second;
                // Remove "Bearer " prefix if present
                if (auth.substr(0, 7) == "Bearer ") {
                    return auth.substr(7);
                }
                return auth;
            }
            return "";
        };

        class HttpParser {
        public:
            static HttpRequest parseRequest(const std::string& rawRequest);

        private:
            static void parseRequestLine(const std::string& requestLine, HttpRequest& request);
            static void parseHeaders(std::istringstream& stream, HttpRequest& request);
            static void parseBody(std::istringstream& stream, HttpRequest& request, int contentLength);
        };
    };

}
#endif