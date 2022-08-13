mkdir -p /build
rsync -a /thingybrowser /build/
cd /build/thingybrowser/
cmake -B /build/thingybrowser/build -S . -DCMAKE_BUILD_TYPE=Release
cmake --build /build/thingybrowser/build --target thingybrowser -- -j4
cp /build/thingybrowser/build/thingybrowser /out/