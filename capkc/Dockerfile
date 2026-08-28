# ---------- Stage 1: Build ----------
FROM ubuntu:24.04 AS builder

RUN apt-get update && apt-get install -y \
    g++ \
    make \
    zlib1g-dev \
    libboost-dev \
    libgmp10-dev \
    libgmpxx4ldbl \
    ninja-build \
    libboost-program-options1.83-dev \
    libboost-random1.83-dev \
    cmake \
    python3-dev \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /capkc_build

COPY capkc/ ./capkc/
COPY dnf/ ./dnf/
COPY D4/d4/ ./d4/
COPY D4/wrapper/ ./wrapper/

WORKDIR /capkc

RUN cd /capkc_build/d4 && \
    make clean && make -j && \
    cp /capkc_build/d4/d4 /capkc/ && \
    rm -rf /capkc_build/d4

RUN cd /capkc_build/wrapper && \
    make clean && make -j && \
    cp /capkc_build/wrapper/wrap /capkc/ && \
    rm -rf /capkc_build/wrapper

RUN cd /capkc_build/capkc/dependencies/networkit && \
    rm -rf build && \
    mkdir build && \
    cd build && \
    cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_POSITION_INDEPENDENT_CODE=ON .. && \
    make -j4 && \
    cd /capkc_build/capkc && \
    g++ gen.cpp -o gen && \
    ./gen -DD4_PATH=\\\""/capkc/d4"\\\" && \
    ninja clean && \
    ninja && \
    cp /capkc_build/capkc/build/capkc /capkc/ && \
    cp /capkc_build/capkc/build/sampler /capkc/ && \
    cp /capkc_build/capkc/build/appmc /capkc/ && \
    cp /capkc_build/capkc/build/smp /capkc/ && \
    cp /capkc_build/capkc/build/up /capkc/ && \
    rm -rf /capkc_build/capkc

RUN cd /capkc_build/dnf/dependencies/cadical-rel-2.1.3 && \
    ./configure && \
    make -j && \
    cd /capkc_build/dnf && \
    g++ gen.cpp -o gen && \
    ./gen && \
    ninja && \
    cp /capkc_build/dnf/build/dnf /capkc/ && \
    rm -rf /capkc_build/dnf

RUN rm -rf /capkc_build

# ---------- Stage 2: Runtime ----------
FROM ubuntu:24.04

RUN apt-get update && apt-get install -y \
    g++ \
    make \
    zlib1g \
    libgmp10 \
    libgmpxx4ldbl \
    ninja-build \
    libboost-program-options1.83.0 \
    libboost-random1.83.0 \
    cmake \
    python3 \
    && rm -rf /var/lib/apt/lists/*

COPY --from=builder /capkc/ /capkc
