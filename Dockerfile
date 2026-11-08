FROM ubuntu:24.04

ENV DEBIAN_FRONTEND=noninteractive
ENV XMAKE_ROOT=y

RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential \
    ca-certificates \
    clang \
    clang-tools \
    ccache \
    ninja-build \
    git \
    curl \
    zip \
    unzip \
    tar \
    pkg-config \
    autoconf \
    autoconf-archive \
    automake \
    libtool \
    cmake \
    python3 \
    libssl-dev \
    libffi-dev \
    libx11-dev \
    libxext-dev \
    libxrandr-dev \
    libxinerama-dev \
    libxcursor-dev \
    libxi-dev \
    libxcb1-dev \
    libxkbcommon-dev \
    libwayland-dev \
    libgl1-mesa-dev \
    libvulkan-dev \
    && rm -rf /var/lib/apt/lists/*

RUN curl -fsSL https://xmake.io/shget.text | bash
ENV PATH="/root/.local/bin:${PATH}"

WORKDIR /Ivy

COPY . .

RUN --mount=type=cache,target=/root/.xmake \
    xmake f -m release --toolchain=clang -y

ENTRYPOINT ["/Ivy/build/linux/x86_64/release/Ivy"]
