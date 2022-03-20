rm -rf /root/.conan
rm -rf /build
mkdir -p /build
cp -r /thingybrowser/ /build/
rm /build/thingybrowser/libs/wxWidgets -rf
cd /build/thingybrowser/
git submodule update --init --recursive
cmake -DCMAKE_TOOLCHAIN_FILE=mingw-w64-x86_64.cmake -B /build/thingybrowser/build -S . -DCMAKE_BUILD_TYPE=Release -DWIN32=1 -DMINGW=1
cmake --build /build/thingybrowser/build
cp /build/thingybrowser/build/thingybrowser /out/thingybrowser