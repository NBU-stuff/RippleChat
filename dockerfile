# Use Ubuntu 22.04 as base image
FROM ubuntu:22.04

# Avoid prompts from apt
ENV DEBIAN_FRONTEND=noninteractive
ENV TZ=UTC

# Install build dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    git \
    curl \
    zip \
    unzip \
    pkg-config \
    libssl-dev \
    libpq-dev \
    libpqxx-dev \
    nlohmann-json3-dev \
    libfmt-dev \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/*

# Install and setup vcpkg
WORKDIR /opt
RUN git clone https://github.com/microsoft/vcpkg && \
    ./vcpkg/bootstrap-vcpkg.sh && \
    ./vcpkg/vcpkg integrate install

# Install required packages via vcpkg
RUN /opt/vcpkg/vcpkg install crow libpqxx fmt nlohmann-json openssl

# Setup working directory
WORKDIR /app

# Copy CMake files first
COPY CMakeLists.txt .
COPY RippleChat/CMakeLists.txt ./RippleChat/

# Copy source files
COPY RippleChat/Source ./RippleChat/Source
COPY RippleChat/Headers ./RippleChat/Headers

# Build the project
RUN cmake -B build -S . \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_TOOLCHAIN_FILE=/opt/vcpkg/scripts/buildsystems/vcpkg.cmake && \
    cmake --build build --config Release

# Set the entry point
CMD ["./build/bin/RippleChatApp"]