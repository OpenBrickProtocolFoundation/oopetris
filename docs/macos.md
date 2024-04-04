# MacOS build

## Prerequisites

- a C++23 compatible compiler, we only support Clang with `libc++`
- meson
- some development packages from homebrew, these are optional, since meson can compile them from source

## Compiling

You just have to call these commands:


```bash
meson setup build

meson compile -C build

./build/oopetris # to run the executable
```
