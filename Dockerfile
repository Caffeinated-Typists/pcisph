FROM ubuntu:22.04

# Prevent interactive prompts during installation
ENV DEBIAN_FRONTEND=noninteractive

# Install dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    git \
    libgl1-mesa-dev \
    libglu1-mesa-dev \
    libglfw3-dev \
    libglew-dev \
    libglm-dev \
    libtbb-dev \
    xorg-dev \
    && rm -rf /var/lib/apt/lists/*

# Create working directory
WORKDIR /app

# Copy project files
COPY . .

# Build project
RUN mkdir -p build && \
    cd build && \
    cmake .. && \
    make -j$(nproc)

# Set working directory to where executable is
WORKDIR /app

# Command to run the application
CMD ["/app/pcisph.out"]
