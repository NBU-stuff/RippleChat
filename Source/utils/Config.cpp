
#include "../../Headers/utils/Config.hpp"
#include <fstream>
#include <sstream>
#include <stdexcept>

namespace utils {

    std::unordered_map<std::string, std::string> Config::configMap;

    void Config::load(const std::string& filepath) {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            throw std::runtime_error("Could not open config file: " + filepath);
        }

        std::string line;
        while (std::getline(file, line)) {
            if (line.empty() || line[0] == '#') continue; // Skip empty lines and comments
            auto delimiterPos = line.find('=');
            auto key = line.substr(0, delimiterPos);
            auto value = line.substr(delimiterPos + 1);
            configMap[key] = value;
        }
    }

    std::string Config::get(const std::string& key) {
        if (configMap.find(key) == configMap.end()) {
            throw std::runtime_error("Key not found in config: " + key);
        }
        return configMap[key];
    }

} // namespace utils
