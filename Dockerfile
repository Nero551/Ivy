FROM ubuntu:24.04
# Base tools
RUN apt-get update && apt-get install -y \
    build-essential \
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
    && rm -rf /var/lib/apt/lists/*

RUN curl -L \
    https://github.com/Kitware/CMake/releases/download/v4.4.2/cmake-4.4.2-linux-x86_64.sh \
    -o /tmp/cmake.sh \
    && chmod +x /tmp/cmake.sh \
    && /tmp/cmake.sh --skip-license --prefix=/usr/local \
    && rm /tmp/cmake.sh

WORKDIR /Ivy

RUN git config --global --add safe.directory '*'

CMD ["./scripts/run.sh"]