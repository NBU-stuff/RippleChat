FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive
ENV VCPKG_ROOT=/vcpkg

# Install build requirements with all possible PostgreSQL dependencies
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
    bison \
    flex \
    autoconf \
    automake \
    libtool \
    perl \
    && rm -rf /var/lib/apt/lists/*

# Setup vcpkg
WORKDIR /vcpkg
RUN git clone https://github.com/Microsoft/vcpkg.git . && \
    ./bootstrap-vcpkg.sh

# Create working directory
WORKDIR /app

# Copy only vcpkg.json
COPY vcpkg.json .

# Instead of running the full installation, we'll just try to build libpqxx
# and capture all possible output
CMD echo "Starting debug process..." && \
    echo "Environment:" && \
    env && \
    echo "Installing libpqxx..." && \
    ${VCPKG_ROOT}/vcpkg install libpqxx --x-manifest-root=/app --triplet=x64-linux --debug && \
    echo "Checking logs:" && \
    find /vcpkg/buildtrees/libpqxx -name "*.log" -type f -exec echo "=== {} ===" \; -exec cat {} \; && \
    echo "Done"