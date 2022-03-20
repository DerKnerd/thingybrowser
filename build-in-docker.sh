mkdir -p /build
cp -r /thingybrowser /build/
cd /build/libs/wxWidgets
git reset --hard
git clean -fdx
cd /build
cmake -DCMAKE_TOOLCHAIN_FILE=/thingybrowser/mingw-w64-x86_64.cmake -B build -S . -DCMAKE_BUILD_TYPE=Release -DWIN32=1 -DMINGW=1
cmake --build build
cp build/thingybrowser.exe /out/thingybrowser.exe