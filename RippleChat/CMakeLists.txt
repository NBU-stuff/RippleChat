# Minimum required CMake version
cmake_minimum_required(VERSION 3.10)

# Project name and version
project(RippleChat VERSION 1.0)

# Create executable
add_executable(RippleChat)

# Collect source files
file(GLOB_RECURSE SOURCES "Source/*.cpp" "Source/*.hpp" "Headers/*.hpp")
target_sources(RippleChat PRIVATE ${SOURCES})

# Include directories
target_include_directories(RippleChat PRIVATE
    "Source"
    "D:/C_Users_Source_Backup_File/LibrariesC++/vcpkg/installed/x64-windows/include"
    "D:/C_Users_Source_Backup_File/LibrariesC++/vcpkg/installed/x64-windows/include/libpqxx"
    "D:/C_Users_Source_Backup_File/LibrariesC++/vcpkg/installed/x64-windows/include/openssl"
    "D:/C_Users_Source_Backup_File/LibrariesC++/vcpkg/installed/x64-windows/include/crow"
)

# Library directories
target_link_directories(RippleChat PRIVATE
    "D:/C_Users_Source_Backup_File/LibrariesC++/vcpkg/installed/x64-windows/lib"
)

# Link libraries
target_link_libraries(RippleChat PRIVATE
    pqxx

    OpenSSL::Crypto
    OpenSSL::SSL
)

# Preprocessor definitions
target_compile_definitions(RippleChat PRIVATE
    CROW_MAIN        # Necessary for Crow
    DATABASE_PGSQL   # Define for using PostgreSQL with Crow
)

# Windows-specific settings
if(WIN32)
    target_compile_definitions(RippleChat PRIVATE WINDOWS)
    target_compile_options(RippleChat PRIVATE "/utf-8")
endif()

# Debug Configuration
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG -g")

# Release Configuration
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -DRELEASE -O2 -g")

# Distribution Configuration
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} -DDIST -O2")
set(CMAKE_CXX_FLAGS_MINSIZEREL "${CMAKE_CXX_FLAGS_MINSIZEREL} -DDIST -O2 -s")

# Set OpenSSL root directory
set(OPENSSL_ROOT_DIR "D:/C_Users_Source_Backup_File/LibrariesC++/vcpkg/installed/x64-windows")

# Find OpenSSL
find_package(OpenSSL REQUIRED)

# Copy pqxx.dll and libpq.dll to the build directory after the build
add_custom_command(TARGET RippleChat POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "D:/C_Users_Source_Backup_File/LibrariesC++/vcpkg/installed/x64-windows/bin/pqxx.dll"
        $<TARGET_FILE_DIR:RippleChat>/pqxx.dll
)

