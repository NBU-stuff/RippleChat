cmake_minimum_required(VERSION 3.10)
project(RippleChat VERSION 1.0)

# Create executable
add_executable(RippleChat)

# Collect source files
file(GLOB_RECURSE SOURCES "Source/*.cpp" "Source/*.hpp" "Headers/*.hpp")
target_sources(RippleChat PRIVATE ${SOURCES})

# Try to find packages using vcpkg first
find_package(OpenSSL REQUIRED)
find_package(PostgreSQL REQUIRED)
find_package(libpqxx CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)
find_package(Crow CONFIG REQUIRED)

# Include directories with fallback paths
if(WIN32)
    target_include_directories(RippleChat PRIVATE
        "Source"
        "D:/C_Users_Source_Backup_File/LibrariesC++/vcpkg/installed/x64-windows/include"
        "D:/C_Users_Source_Backup_File/LibrariesC++/vcpkg/installed/x64-windows/include/libpqxx"
        "D:/C_Users_Source_Backup_File/LibrariesC++/vcpkg/installed/x64-windows/include/openssl"
        "D:/C_Users_Source_Backup_File/LibrariesC++/vcpkg/installed/x64-windows/include/crow"
        ${OPENSSL_INCLUDE_DIR}
        ${PostgreSQL_INCLUDE_DIRS}
    )

    # Windows-specific library directories
    target_link_directories(RippleChat PRIVATE
        "D:/C_Users_Source_Backup_File/LibrariesC++/vcpkg/installed/x64-windows/lib"
    )

    # Windows-specific DLL copying
    add_custom_command(TARGET RippleChat POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "D:/C_Users_Source_Backup_File/LibrariesC++/vcpkg/installed/x64-windows/bin/pqxx.dll"
            $<TARGET_FILE_DIR:RippleChat>/pqxx.dll
    )
else()
    # Linux/Docker include directories
    target_include_directories(RippleChat PRIVATE
        "Source"
        ${OPENSSL_INCLUDE_DIR}
        ${PostgreSQL_INCLUDE_DIRS}
    )
endif()

# Common link libraries using modern CMake targets
target_link_libraries(RippleChat PRIVATE
    OpenSSL::SSL
    OpenSSL::Crypto
    PostgreSQL::PostgreSQL
    libpqxx::pqxx
    nlohmann_json::nlohmann_json
    fmt::fmt
    Crow::Crow
)

# Preprocessor definitions
target_compile_definitions(RippleChat PRIVATE
    CROW_MAIN
    DATABASE_PGSQL
)

# Windows-specific settings
if(WIN32)
    target_compile_definitions(RippleChat PRIVATE WINDOWS)
    target_compile_options(RippleChat PRIVATE "/utf-8")
endif()

# Debug Configuration
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG")

# Release Configuration
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -DRELEASE -O2")

# Distribution Configuration
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} -DDIST -O2")
set(CMAKE_CXX_FLAGS_MINSIZEREL "${CMAKE_CXX_FLAGS_MINSIZEREL} -DDIST -O2 -s")

if(WIN32)
    # Set OpenSSL root directory for Windows
    set(OPENSSL_ROOT_DIR "D:/C_Users_Source_Backup_File/LibrariesC++/vcpkg/installed/x64-windows")
endif()