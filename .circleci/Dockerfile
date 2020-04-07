
FROM ubuntu:disco

RUN apt-get update && \
    apt-get install -y g++ curl nasm python3 python3-pip cmake ninja-build lcov && \
    pip3 install pytest pytest-cov

#This layer will go away when GCC 9+ becomes default
RUN apt-get install -y gcc-9 g++-9
