#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <string>

namespace utils {

    class Logger {
    public:
        enum class LogLevel {
            INFO,
            WARN,
            ERR
        };

        static void log(const std::string& message, Logger::LogLevel level = Logger::LogLevel::INFO);
    };

} // namespace utils

#endif // LOGGER_HPP
