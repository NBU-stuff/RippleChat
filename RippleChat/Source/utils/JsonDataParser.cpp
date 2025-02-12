#include <string>
#include "../../Headers/utils/JsonDataParser.hpp"

namespace utils {

    std::string ParseJsonFromRequest(const std::string& request) {

        // Find the start and end of the JSON data if available
        size_t startPos = request.find('{');
        size_t endPos = request.find_last_of('}');

        std::string jsonData = request.substr(startPos, endPos - startPos + 1);



        return jsonData;
    }




}
