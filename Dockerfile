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
    xmake \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /Ivy

RUN git config --global --add safe.directory '*'

CMD ["./scripts/run.sh"]