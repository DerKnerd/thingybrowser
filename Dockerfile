FROM ubuntu:rolling

ENV DEBIAN_FRONTEND noninteractive
RUN apt-get update && apt-get upgrade -y
RUN apt-get install mingw-w64 mingw-w64-x86-64-dev g++-mingw-w64-x86-64 gcc-mingw-w64-x86-64 binutils-mingw-w64-x86-64 build-essential cmake git wget -y
RUN wget https://github.com/conan-io/conan/releases/latest/download/conan-ubuntu-64.deb
RUN dpkg -i conan-ubuntu-64.deb
RUN git clone https://github.com/DerKnerd/filament-browser.git --recurse-submodules
#WORKDIR /filament-browser
RUN cmake -DCMAKE_TOOLCHAIN_FILE=/filament-browser/mingw-w64-x86_64.cmake -B build -S /filament-browser -DCMAKE_BUILD_TYPE=Release
RUN cmake --build /build/