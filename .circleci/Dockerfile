
FROM alpine:latest

RUN apk add --no-cache g++
RUN apk add --no-cache curl
RUN apk add --no-cache nasm
RUN apk add --no-cache python3
RUN apk add --no-cache perl
RUN apk add --no-cache perl-io-gzip
RUN apk add --no-cache perl-json
RUN apk add --no-cache bash
RUN apk add --no-cache cmake
RUN apk add --no-cache make
RUN apk add --no-cache git
RUN apk add --no-cache gtest-dev

RUN git clone https://github.com/linux-test-project/lcov.git && make install -C lcov

RUN pip3 install pytest pytest-cov
