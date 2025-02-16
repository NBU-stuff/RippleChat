cmake_minimum_required(VERSION 3.15)
project(RippleChat VERSION 1.0.0 LANGUAGES CXX)

# Initialize vcpkg early
if(DEFINED ENV{VCPKG_ROOT} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
        CACHE STRING "Vcpkg toolchain file")
endif()

# C++ standard settings
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")

# Logging helper
function(log_find_status package_name)
    if(${package_name}_FOUND)
        message(STATUS "Found ${package_name}")
    else()
        message(STATUS "Could not find ${package_name}")
    endif()
endfunction()

# Find required packages
find_package(libpqxx CONFIG REQUIRED)
log_find_status(libpqxx)

find_package(OpenSSL REQUIRED)
log_find_status(OpenSSL)

find_package(fmt CONFIG REQUIRED)
log_find_status(fmt)

find_package(nlohmann_json CONFIG REQUIRED)
log_find_status(nlohmann_json)

find_package(jwt-cpp CONFIG REQUIRED)
log_find_status(jwt-cpp)

# For Boost, we need specific components
find_package(Boost REQUIRED COMPONENTS system)
log_find_status(Boost)

# Find Crow using vcpkg
find_package(Crow CONFIG REQUIRED)
log_find_status(Crow)


        message( "CURRETN SOURCE DIR - ${CMAKE_CURRENT_SOURCE_DIR}")

# Add source files
file(GLOB_RECURSE SOURCES 
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/*.cpp"
)

file(GLOB_RECURSE HEADERS 
    "${CMAKE_CURRENT_SOURCE_DIR}/Headers/*.hpp"
)

# Create executable
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# Include directories
target_include_directories(${PROJECT_NAME} 
    PRIVATE 
        "${CMAKE_CURRENT_SOURCE_DIR}/Headers"
)

# Link libraries
target_link_libraries(${PROJECT_NAME} 
    PRIVATE 
           libpqxx::pqxx
        OpenSSL::SSL 
        OpenSSL::Crypto
        fmt::fmt
        nlohmann_json::nlohmann_json
        jwt-cpp::jwt-cpp
        Boost::system
           Crow::Crow
)

# Post-build step to copy pqxx.dll to the output directory
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different 
        $<TARGET_FILE:libpqxx::pqxx> 
        ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
)

# Platform-specific settings
if(WIN32)
    target_compile_definitions(${PROJECT_NAME} 
        PRIVATE 
            _WIN32_WINNT=0x0601
            WIN32_LEAN_AND_MEAN
    )
endif()

# Install rules
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

# Print configuration info
message(STATUS "CMAKE_TOOLCHAIN_FILE: ${CMAKE_TOOLCHAIN_FILE}")
message(STATUS "CMAKE_PREFIX_PATH: ${CMAKE_PREFIX_PATH}")
message(STATUS "Project will be built in: ${CMAKE_BINARY_DIR}")