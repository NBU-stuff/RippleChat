#include <unordered_set>
#include "../Headers/routes/JwtHandler.hpp"
#include <jwt-cpp/jwt.h>
#include <chrono>
#include <iostream>
#include <stdexcept>

// Assuming JwtHandler has a member for storing blacklisted tokens
std::unordered_set<std::string> JwtHandler::blacklistedTokens;

std::string JwtHandler::generateToken(const std::string& userId, int expiryHours) {
    const char* secretKey = std::getenv("JWT_SECRET");
    if (!secretKey) {
        throw std::runtime_error("JWT_SECRET environment variable is not set");
    }

    // Generate a JWT token with specified userId and expiry time
    auto token = jwt::create()
        .set_issuer("RippleChat")                              // Your app's name or domain
        .set_subject(userId)                                   // The user ID
        .set_issued_at(std::chrono::system_clock::now())       // Current time
        .set_expires_at(std::chrono::system_clock::now() + std::chrono::hours(expiryHours)) // Expiry time
        .sign(jwt::algorithm::hs256{ secretKey });             // Sign with the secret key

    return token;
}

bool JwtHandler::validateToken(const std::string& token, const std::string& expectedUserId) {
    try {
        const char* secretKey = std::getenv("JWT_SECRET");
        if (!secretKey) {
            throw std::runtime_error("JWT_SECRET environment variable is not set");
        }

        // Check if token is blacklisted
        if (blacklistedTokens.count(token) > 0) {
            std::cerr << "Token is blacklisted" << std::endl;
            return false;
        }

        // Decode the token
        auto decodedToken = jwt::decode(token);

        // Verify the token
        auto verifier = jwt::verify()
            .allow_algorithm(jwt::algorithm::hs256{ secretKey }) // Verify with the same secret key
            .with_issuer("RippleChat");                          // Verify the issuer

        verifier.verify(decodedToken);

        // Extract userId from the token
        auto tokenUserId = decodedToken.get_subject(); // Assuming `userId` is stored in the subject claim
        if (tokenUserId != expectedUserId) {
            std::cerr << "Token user_id does not match the expected user_id" << std::endl;
            return false;
        }

        return true; // Token is valid and userId matches
    }
    catch (const std::exception& e) {
        std::cerr << "Token verification failed: " << e.what() << std::endl;
        return false;
    }
}

void JwtHandler::invalidateToken(const std::string& token) {
    // Add token to blacklist
    blacklistedTokens.insert(token);
}
