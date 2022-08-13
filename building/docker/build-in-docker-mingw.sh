mkdir -p /build
rsync -a /thingybrowser /build/
cd /build/thingybrowser/
cmake -DCMAKE_TOOLCHAIN_FILE=building/cmake/mingw-w64-x86_64.cmake -B /build/thingybrowser/build -S . -DCMAKE_BUILD_TYPE=Release -DMINGW=1
cmake --build /build/thingybrowser/build --target thingybrowser -- -j4
cp /build/thingybrowser/build/thingybrowser.exe /out/