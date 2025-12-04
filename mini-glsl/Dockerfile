# Dockerfile for mini-glsl + SPIR-V
FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive

# Basic build + GL + SPIR-V toolchain
RUN apt-get update && apt-get install -y \
    build-essential \
    curl \
    pkg-config \
    libosmesa6-dev \
    libgl1-mesa-dev \
    libglu1-mesa-dev \
    libpng-dev \
    glslang-tools \
    spirv-tools && \
    rm -rf /var/lib/apt/lists/*

WORKDIR /work

# Copy the whole repo in (students' code, Makefile, shaders, spirv, tools, etc.)
COPY . .

# Optional: fetch teapot once in the image so it’s cached
RUN make obj || true

# Build the harness using pkg-config for libpng (to match Ubuntu layout)
RUN make shader \
    CPPFLAGS="$(pkg-config --cflags libpng)" \
    LIBS="$(pkg-config --libs libpng) -lOSMesa -lm"

# Optional: prebuild SPIR-V reference (can be commented out if you prefer)
RUN make spv-asm-all \
    CPPFLAGS="$(pkg-config --cflags libpng)" \
    LIBS="$(pkg-config --libs libpng) -lOSMesa -lm" || true

# Default: drop into a shell so you can run make / tests as you like
CMD ["/bin/bash"]