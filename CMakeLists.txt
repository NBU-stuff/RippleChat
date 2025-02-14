cmake_minimum_required(VERSION 3.8)

# Project name
project(RippleChat)

# C++ standard settings
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Set output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Binaries/${CMAKE_BUILD_TYPE})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Binaries/Intermediates/${CMAKE_BUILD_TYPE})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Binaries/Intermediates/${CMAKE_BUILD_TYPE})

# Set vcpkg toolchain file based on environment
if(DEFINED ENV{VCPKG_ROOT})
    # Use environment variable if set
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
        CACHE STRING "Vcpkg toolchain file")
elseif(WIN32)
    # Windows default path
    set(CMAKE_TOOLCHAIN_FILE "D:/C_Users_Source_Backup_File/LibrariesC++/vcpkg/scripts/buildsystems/vcpkg.cmake"
        CACHE STRING "Vcpkg toolchain file")
else()
    # Docker/Linux default path
    set(CMAKE_TOOLCHAIN_FILE "/opt/vcpkg/scripts/buildsystems/vcpkg.cmake"
        CACHE STRING "Vcpkg toolchain file")
endif()

include_directories("${CMAKE_SOURCE_DIR}/Headers")

# Add subdirectory for source code
add_subdirectory(RippleChat)