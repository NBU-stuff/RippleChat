# Stage 1: Build environment
FROM ubuntu:22.04 AS builder

# Prevent apt from asking for user input
ENV DEBIAN_FRONTEND=noninteractive

# Install basic build dependencies including autoconf and other required tools
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    git \
    curl \
    zip \
    unzip \
    tar \
    pkg-config \
    ninja-build \
    bison \
    flex \
    libssl-dev \
    libreadline-dev \
    zlib1g-dev \
    autoconf \
    automake \
    libtool \
    perl \
    linux-headers-generic \
    python3 \
    && rm -rf /var/lib/apt/lists/*

# Set up vcpkg in a separate layer
WORKDIR /vcpkg
RUN git clone https://github.com/Microsoft/vcpkg.git . && \
    ./bootstrap-vcpkg.sh && \
    ./vcpkg integrate install
ENV VCPKG_ROOT=/vcpkg

# Create a manifest file for vcpkg to cache dependencies
WORKDIR /app
COPY vcpkg.json .

# Install dependencies using manifest mode
RUN ${VCPKG_ROOT}/vcpkg install --triplet x64-linux --clean-after-build

# Copy only necessary files
COPY CMakeLists.txt .
COPY Headers/ Headers/
COPY Source/ Source/

# Configure and build
RUN cmake -B build \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_TOOLCHAIN_FILE=/vcpkg/scripts/buildsystems/vcpkg.cmake \
    -DVCPKG_TARGET_TRIPLET=x64-linux \
    -GNinja \
    -DCMAKE_VERBOSE_MAKEFILE=ON

RUN cmake --build build --config Release

# Stage 2: Runtime environment
FROM ubuntu:22.04

# Install runtime dependencies
RUN apt-get update && apt-get install -y \
    libpq5 \
    libssl3 \
    && rm -rf /var/lib/apt/lists/*

# Copy the built executable and required shared libraries
COPY --from=builder /app/build/bin/RippleChat /app/RippleChat

# Set the working directory
WORKDIR /app

# Expose the port your application uses
EXPOSE 8080

# Set the entry point
CMD ["./RippleChat"]