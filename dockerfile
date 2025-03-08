# Stage 1: vcpkg cache layer with separate libpqxx install
FROM ubuntu:22.04 AS vcpkg-builder

ENV DEBIAN_FRONTEND=noninteractive
ENV VCPKG_ROOT=/vcpkg

# Install build requirements
RUN apt-get update && apt-get install -y \
    build-essential \
    autoconf \
    cmake \
    curl \
    zip \
    unzip \
    tar \
    git \
    ninja-build \
    pkg-config \
    postgresql-server-dev-all \
    libpq-dev \
    libssl-dev \
    python3 \
    && rm -rf /var/lib/apt/lists/*

# Setup vcpkg
WORKDIR /vcpkg
RUN git clone https://github.com/Microsoft/vcpkg.git . && \
    ./bootstrap-vcpkg.sh

# First install libpqxx separately (not in manifest mode)
# We're specifying a specific older version that's known to work better
RUN ${VCPKG_ROOT}/vcpkg install libpqxx:x64-linux --overlay-ports=${VCPKG_ROOT}/ports 

# Create app directory and copy the vcpkg.json for other dependencies
WORKDIR /app
COPY vcpkg.json .

# Now install the rest of the dependencies using the manifest
RUN ${VCPKG_ROOT}/vcpkg install \
    --x-manifest-root=/app \
    --triplet=x64-linux \
    --clean-after-build

# Stage 2: Project build layer
FROM vcpkg-builder AS builder

# Install additional build dependencies
RUN apt-get update && apt-get install -y \
    autoconf \
    automake \
    libtool \
    perl \
    bison \
    flex \
    && rm -rf /var/lib/apt/lists/*

# Copy project files
COPY CMakeLists.txt .
COPY Headers/ Headers/
COPY Source/ Source/

# Configure and build with CMake
RUN cmake -B build \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_TOOLCHAIN_FILE=/vcpkg/scripts/buildsystems/vcpkg.cmake \
    -DVCPKG_TARGET_TRIPLET=x64-linux \
    -GNinja

RUN cmake --build build --config Release

# Stage 3: Runtime layer
FROM ubuntu:22.04

RUN apt-get update && apt-get install -y \
    libpq5 \
    libssl3 \
    && rm -rf /var/lib/apt/lists/*

# Copy the built executable
COPY --from=builder /app/build/bin/RippleChat /app/RippleChat

WORKDIR /app
EXPOSE 8080

CMD ["./RippleChat"]