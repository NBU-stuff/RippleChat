// QueryBuilder.hpp
#ifndef QUERY_BUILDER_HPP
#define QUERY_BUILDER_HPP

#include <string>
#include <vector>
#include <sstream>
#include <stdexcept>
#include <memory>
#include <regex>
#include <unordered_map>

namespace utils {

    class QueryBuilder {
    public:
        template<typename... Args>
        static std::string build(const std::string& query, Args&&... args) {
            std::vector<std::string> params;
            (params.push_back(toString(std::forward<Args>(args))), ...);
            return buildQuery(query, params);
        }

    private:
        static std::string buildQuery(const std::string& query, const std::vector<std::string>& params) {
            std::string result = query;
            std::unordered_map<std::string, std::string> paramMap;

            // First pass: collect all unique placeholders and their corresponding values
            std::regex placeholder_pattern("\\$([0-9]+)");
            std::smatch match;
            std::string::const_iterator searchStart(query.cbegin());

            while (std::regex_search(searchStart, query.cend(), match, placeholder_pattern)) {
                int index = std::stoi(match[1]) - 1;
                if (index >= params.size()) {
                    throw std::out_of_range("Parameter index out of range");
                }
                paramMap[match[0]] = params[index];
                searchStart = match.suffix().first;
            }

            // Second pass: replace all placeholders with their values
            for (const auto& [placeholder, value] : paramMap) {
                size_t pos = 0;
                while ((pos = result.find(placeholder, pos)) != std::string::npos) {
                    result.replace(pos, placeholder.length(), value);
                    pos += value.length();
                }
            }

            return result;
        }

        static std::string toString(const std::string& value) {
            return "'" + escapeString(value) + "'";
        }

        static std::string toString(int value) {
            return std::to_string(value);
        }

        static std::string toString(long value) {
            return std::to_string(value);
        }

        static std::string toString(double value) {
            return std::to_string(value);
        }

        static std::string toString(bool value) {
            return value ? "TRUE" : "FALSE";
        }

        static std::string toString(const char* value) {
            return toString(std::string(value));
        }

        template<typename T>
        static std::string toString(const std::optional<T>& value) {
            if (!value.has_value()) {
                return "NULL";
            }
            return toString(value.value());
        }

        static std::string toString(std::nullptr_t) {
            return "NULL";
        }

        static std::string escapeString(const std::string& str) {
            std::string escaped;
            escaped.reserve(str.length());

            for (char c : str) {
                switch (c) {
                case '\'':
                    escaped += "''";  // Double single quotes for PostgreSQL
                    break;
                case '\\':
                    escaped += "\\\\";
                    break;
                case '\0':
                    escaped += "\\0";
                    break;
                case '\n':
                    escaped += "\\n";
                    break;
                case '\r':
                    escaped += "\\r";
                    break;
                case '\t':
                    escaped += "\\t";
                    break;
                default:
                    escaped += c;
                }
            }

            return escaped;
        }
    };

} // namespace utils

#endif // QUERY_BUILDER_HPP