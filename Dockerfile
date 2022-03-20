FROM ubuntu:21.10

#ADD . /thingybrowser
#WORKDIR /thingybrowser
ENV DEBIAN_FRONTEND noninteractive
ENV CONAN_REVISIONS_ENABLED 1
#VOLUME ./out:/out

RUN apt-get update && apt-get upgrade -y
RUN apt-get install libnpth-mingw-w64-dev mingw-w64-x86-64-dev g++-mingw-w64-x86-64-posix gcc-mingw-w64-x86-64-posix binutils-mingw-w64-x86-64 build-essential cmake git wget libz-mingw-w64-dev mingw-w64-common mingw-w64-tools -y
RUN wget https://github.com/conan-io/conan/releases/latest/download/conan-ubuntu-64.deb
RUN dpkg -i conan-ubuntu-64.deb

ENTRYPOINT "/thingybrowser/build-in-docker.sh"