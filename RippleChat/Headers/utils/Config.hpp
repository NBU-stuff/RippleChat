#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <string>
#include <unordered_map>

namespace utils {

    class Config {
    public:
        static void load(const std::string& filepath);
        static std::string get(const std::string& key);

    private:
        static std::unordered_map<std::string, std::string> configMap;
    };

} // namespace utils

#endif // CONFIG_HPP
