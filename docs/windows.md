# Windows build

## Prerequisites

- a C++23 compatible compiler, we support MSVC and MINGW-64
- meson

## Compiling


You just have to call these commands:


```bash
meson setup build

meson compile -C build


./build/oopetris.exe # to run the executable
```

