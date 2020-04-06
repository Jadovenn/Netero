FROM alpine:3.11

RUN echo http://nl.alpinelinux.org/alpine/edge/main >> /etc/apk/repositories
RUN echo http://nl.alpinelinux.org/alpine/edge/community >> /etc/apk/repositories
RUN echo http://nl.alpinelinux.org/alpine/edge/testing >> /etc/apk/repositories
RUN apk add --update && \
    apk add --no-cache git openssh-client tar gzip ca-certificates gcc g++ nasm make cmake lcov curl
