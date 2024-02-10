# Linux build


You need:

- a C++23 compatible compiler, we support Clang and GCC.
- meson or CMake
- some development packages, these are optional, since either meson or CMake can compile them from source


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


./build/oopetris # to run the executable
```


## Compiling for flatpak

You optionally can compile a flatpak image.

For that you need some additional binaries, namely the flatpak-builder, than just run:

<!-- WIP: that is not entirely correct -->
```bash
flatpak-build com.github.mgerhold.OOPetris.yml
```
