#include "../Headers/utils/Logger.hpp"
#include <iostream>

namespace utils {

    void Logger::log(const std::string& message, LogLevel level) {
        switch (level) {
        case LogLevel::INFO:
            std::cout << "[INFO] " << message << std::endl;
            break;
        case LogLevel::WARN:
            std::cout << "[WARN] " << message << std::endl;
            break;
        case LogLevel::ERR:
            std::cerr << "[ERROR] " << message << std::endl;
            break;
        }
    }

} // namespace utils
