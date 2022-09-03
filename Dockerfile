FROM ubuntu:20.04
ARG DEBIAN_FRONTEND=noninteractive
LABEL Description="Diophantus build environment"

# Install packages
RUN apt update -y
RUN apt install -y \
    build-essential cmake clang git \
    libstdc++-10-dev libgmp-dev libboost-log1.71-dev

# Copy source files
COPY etc /usr/src/diophantus/etc
COPY src /usr/src/diophantus/src
COPY CMakeLists.txt /usr/src/diophantus/CMakeLists.txt
COPY examples /usr/src/diophantus/examples

# Set up environment
WORKDIR /usr/src/diophantus/build
ENV CXX=clang++

# Build & test
RUN cmake ..
RUN make
RUN make test
