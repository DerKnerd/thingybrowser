FROM ubuntu:rolling

ENV DEBIAN_FRONTEND noninteractive
ENV CONAN_REVISIONS_ENABLED 1
RUN apt-get update && apt-get upgrade -y
RUN apt-get install mingw-w64-x86-64-dev g++-mingw-w64-x86-64-posix gcc-mingw-w64-x86-64-posix binutils-mingw-w64-x86-64 build-essential cmake git wget -y
RUN wget https://github.com/conan-io/conan/releases/latest/download/conan-ubuntu-64.deb
RUN dpkg -i conan-ubuntu-64.deb
RUN git clone https://github.com/DerKnerd/thingybrowser.git --recurse-submodules
RUN cmake -DCMAKE_TOOLCHAIN_FILE=/thingybrowser/mingw-w64-x86_64.cmake -B build -S /thingybrowser -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTING=OFF -DMINGW=1
RUN cmake --build /build/