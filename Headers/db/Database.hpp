#ifndef DATABASE_HPP
#define DATABASE_HPP
#define _CRT_SECURE_NO_WARNINGS
#include <stdlib.h>

#include <pqxx/pqxx>
#include <memory>
#include <string>
#include <queue>
#include <mutex>
#include "../Headers/utils/Config.hpp"
#include "../Headers/utils/Logger.hpp"
#include "../Headers/utils/QueryBuilder.hpp"

class Database {
public:
    // Get database instance (singleton)
    static Database& getInstance() {
        static Database instance;
        return instance;
    }

    // Get a connection from the pool
    std::shared_ptr<pqxx::connection> getConnection() {
        std::lock_guard<std::mutex> lock(poolMutex);  // Ensure thread-safety
        if (connectionPool.empty()) {
            initializeConnection();  // Add a new connection to the pool if empty
        }

        auto conn = connectionPool.front();
        connectionPool.pop();
        return conn;
    }

    // Return a connection back to the pool
    void returnConnection(std::shared_ptr<pqxx::connection> conn) {
        std::lock_guard<std::mutex> lock(poolMutex);  // Ensure thread-safety
        connectionPool.push(conn);
    }

    // Execute a query with error handling
    template <typename... Args>
    auto executeQuery(const std::string& query, Args&&... params) {
        try {
            // Get a connection from the pool
            auto conn = getConnection();

           


            // Build the query by replacing the placeholders
            std::string finalQuery = utils::QueryBuilder::build(query, std::forward<Args>(params)...);

            // Now you can execute the finalQuery with your database connection
      ;

            pqxx::work txn(*conn);

            // Execute the query with the provided parameters
            auto result = txn.exec(finalQuery);



            // Commit the transaction
            txn.commit();

            // Return the connection to the pool
            returnConnection(conn);

            return result;
        }
        catch (const std::exception& e) {
            // Log the error
            utils::Logger::log(std::string("Database error: ") + e.what(), utils::Logger::LogLevel::ERR);
            throw;
        }
    }


private:
    // Private constructor for singleton
    Database() {
        initializeConnection();
    }

    void initializeConnection() {
        try {

            const char* databaseUrl = std::getenv("DATABASE_URL");

            if (!databaseUrl) {

                utils::Logger::log("Error: DATABASE_URL not set!", utils::Logger::LogLevel::ERR);

                throw std::runtime_error("DATABASE_URL environment variable is not set.");
                
            }



            std::string connStr(databaseUrl);

   
            // Create multiple connections and add them to the pool
            for (int i = 0; i < 5; ++i) {  // Let's assume we want a pool of 5 connections
                auto conn = std::make_shared<pqxx::connection>(connStr);
                if (conn->is_open()) {
                    connectionPool.push(conn);
                    utils::Logger::log("Database connection established successfully", utils::Logger::LogLevel::INFO);
                }
                else {
                    utils::Logger::log("Failed to establish a connection", utils::Logger::LogLevel::ERR);
                }
            }
        }
        catch (const std::exception& e) {
            utils::Logger::log(std::string("Failed to connect to database: ") + e.what(), utils::Logger::LogLevel::ERR);
            throw;
        }
    }

    // Prevent copying
    Database(const Database&) = delete;
    Database& operator=(const Database&) = delete;

    // A thread-safe queue to hold available connections
    std::queue<std::shared_ptr<pqxx::connection>> connectionPool;
    std::mutex poolMutex;  // Mutex to ensure thread-safety
};

#endif // DATABASE_HPP
