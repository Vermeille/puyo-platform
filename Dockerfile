FROM ubuntu:15.10

ENV CC gcc
ENV CXX  g++

WORKDIR /root

RUN apt-get update &&  apt-get install -y \
         cmake \
         gcc \
         g++ \
         libboost-all-dev \
         libgoogle-glog-dev \
         libgflags-dev \
         make \
         libmicrohttpd-dev

RUN ln -s /usr/bin/aclocal-1.15 /usr/bin/aclocal-1.14
RUN ln -s /usr/bin/automake-1.15 /usr/bin/automake-1.14

ADD . /root

EXPOSE 8888

RUN mkdir build && cd build && cmake .. && make

ENTRYPOINT ["./build/puyo"]
