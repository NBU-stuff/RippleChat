#ifndef HTTP_RESPONSE_BUILDER_HPP
#define HTTP_RESPONSE_BUILDER_HPP


#include <iostream>
#include <string>
#include <sstream>
#include <crow.h>
namespace utils {

	/**
	 * @brief Builds an HTTP response with a given body and status code.
	 *
	 * @param body The body of the response (typically in JSON format).
	 * @param status The HTTP status code and message (e.g., "200 OK", "404 Not Found").
	 * @return A string representing the full HTTP response.
	 */
	 //std::string build_http_response(const std::string& body, const std::string& status);
	crow::response build_http_response(const std::string& body, const std::string& status);

}  // namespace utils

#endif  // HTTP_RESPONSE_BUILDER_HPP
