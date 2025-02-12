
// HttpParser.cpp


#include <sstream>
#include <iostream>
#include "../../Headers/utils/HttpParser.hpp"
#include <vector>


using namespace utils;



HttpRequest HttpRequest::HttpParser::parseRequest(const std::string& rawRequest) {
    HttpRequest request;
    std::istringstream stream(rawRequest);
    std::string line;

    // Parse request line (GET /path HTTP/1.1)
    std::getline(stream, line);
    if (!line.empty() && line[line.length() - 1] == '\r')
        line = line.substr(0, line.length() - 1);
    parseRequestLine(line, request);

    // Parse headers
    parseHeaders(stream, request);

    // Parse body if Content-Length exists
    auto contentLengthIt = request.headers.find("Content-Length");
    if (contentLengthIt != request.headers.end()) {
        int contentLength = std::stoi(contentLengthIt->second);
        parseBody(stream, request, contentLength);
    }

    return request;
}

void HttpRequest::HttpParser::parseRequestLine(const std::string& requestLine, HttpRequest& request) {
    std::istringstream lineStream(requestLine);
    lineStream >> request.method >> request.path >> request.version;
}

void HttpRequest::HttpParser::parseHeaders(std::istringstream& stream, HttpRequest& request) {
    std::string line;
    while (std::getline(stream, line) && !line.empty()) {
        if (line[line.length() - 1] == '\r')
            line = line.substr(0, line.length() - 1);

        if (line.empty()) break; // Empty line signals end of headers

        size_t colonPos = line.find(':');
        if (colonPos != std::string::npos) {
            std::string key = line.substr(0, colonPos);
            std::string value = line.substr(colonPos + 1);
            // Trim leading whitespace from value
            while (!value.empty() && value[0] == ' ')
                value = value.substr(1);
            request.headers[key] = value;
        }
    }
}

void HttpRequest::HttpParser::parseBody(std::istringstream& stream, HttpRequest& request, int contentLength) {
    if (contentLength > 0) {
        std::vector<char> body(contentLength);
        stream.read(body.data(), contentLength);
        request.body = std::string(body.data(), contentLength);
    }
}
