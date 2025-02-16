#ifndef PASSWORD_HASHER_HPP
#define PASSWORD_HASHER_HPP

#include <string>
#include <random>
#include <openssl/evp.h>
#include <openssl/sha.h>
#include <sstream>
#include <iomanip>
#include <vector>

class PasswordHasher {
public:
    static std::string hashPassword(const std::string& password) {
        std::string salt = generateSalt();
        std::string hashedPassword = hashWithSalt(password, salt);
        return salt + "$" + hashedPassword; // Store salt with hash
    }

    static bool verifyPassword(const std::string& password, const std::string& storedHash) {
        size_t delimiterPos = storedHash.find('$');
        if (delimiterPos == std::string::npos) return false;

        std::string salt = storedHash.substr(0, delimiterPos);
        std::string hash = storedHash.substr(delimiterPos + 1);

        std::string newHash = hashWithSalt(password, salt);
        return newHash == hash;
    }

private:
    static std::string generateSalt(size_t length = 16) {
        static const char charset[] =
            "0123456789"
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "abcdefghijklmnopqrstuvwxyz";

        std::random_device rd;
        std::mt19937 generator(rd());
        std::uniform_int_distribution<> distribution(0, sizeof(charset) - 2);

        std::string salt;
        salt.reserve(length);
        for (size_t i = 0; i < length; ++i) {
            salt += charset[distribution(generator)];
        }
        return salt;
    }

    static std::string hashWithSalt(const std::string& password, const std::string& salt) {
        // Combine password and salt
        std::string combined = password + salt;

        // Create a buffer for the hash
        unsigned char hash[SHA256_DIGEST_LENGTH];

        // Calculate SHA256 hash
        SHA256_CTX sha256;
        SHA256_Init(&sha256);
        SHA256_Update(&sha256, combined.c_str(), combined.size());
        SHA256_Final(hash, &sha256);

        // Convert hash to hex string
        std::stringstream ss;
        for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
            ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(hash[i]);
        }

        return ss.str();
    }
};

#endif // PASSWORD_HASHER_HPP