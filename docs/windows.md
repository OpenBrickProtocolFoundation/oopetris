# Windows build


You need:

- a C++23 compatible compiler, we only support MSVC
- meson or CMake


## Compiling with CMake

For that you additionally need vcpkg, since we use it as dependency system.

After that you can use your normal cmake commands:

You have to replace `<vcpkg-root>` by your actual vcpkg root (where you installed it to).

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE="<vcpkg-root>/vcpkg/scripts/buildsystems/vcpkg.cmake" .

cmake --build build -j --config Release

./build/oopetris # to run the executable
```


## Compiling with Meson


You just have to call these commands:


```bash
meson setup build

meson compile -C build


./build/oopetris.exe # to run the executable
```

