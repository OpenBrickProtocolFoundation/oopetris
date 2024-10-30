# Linux build

## Prerequisites

- a C++23 compatible compiler, we support Clang with `libc++` and `libstdc++` and GCC.
- meson
- some development packages, these are optional, since meson can compile them from source

## Compiling

You just have to call these commands:

```bash
meson setup build

meson compile -C build

./build/oopetris # to run the executable
```

## Compiling for flatpak

You optionally can compile a flatpak image.

For that you need some additional binaries, namely the flatpak-builder, than just run:

> [!NOTE]
> that is not entirely correct

```bash
flatpak-build io.github.openbrickprotocolfoundation.oopetris.yml
```
