
# Stage 1: vcpkg cache layer
FROM ubuntu:22.04 AS vcpkg-builder

ENV DEBIAN_FRONTEND=noninteractive
ENV VCPKG_ROOT=/vcpkg

# Install minimal build requirements
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
    && rm -rf /var/lib/apt/lists/*

# Setup vcpkg
WORKDIR /vcpkg
RUN git clone https://github.com/Microsoft/vcpkg.git . && \
    ./bootstrap-vcpkg.sh

# Copy only vcpkg.json first to cache dependencies
WORKDIR /app
COPY vcpkg.json .

# Install dependencies with vcpkg
RUN ${VCPKG_ROOT}/vcpkg install \
    --x-manifest-root=/app \
    --triplet=x64-linux \
    --clean-after-build

# Stage 2: Project build layer
FROM vcpkg-builder AS builder

# Install additional build dependencies needed for libpq/PostgreSQL
RUN apt-get update && apt-get install -y \
    autoconf \
    automake \
    libtool \
    perl \
    python3 \
    bison \
    flex \
    libssl-dev \
    && rm -rf /var/lib/apt/lists/*

# Copy project files
COPY CMakeLists.txt .
COPY Headers/ Headers/
COPY Source/ Source/

# Configure and build using cached vcpkg packages
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

# Copy the built executable from the builder stage
COPY --from=builder /app/build/bin/RippleChat /app/RippleChat

WORKDIR /app
EXPOSE 8080

CMD ["./RippleChat"]