# Nintendo 3DS build

## Prerequisites

- a C++23 compatible cross compiler, we only support The [devkitpro](https://devkitpro.org/) one
- meson


## Compiling


Cross Compiling is only possible on linux-like systems.

SDL2 officially supports 3ds, so there are no portlibs, so on the initial run of the bash script, SDL2 will be built, and installed into the devkitpro portlibs root folder, this might need sudo (but installing the other toolchains from devkitpro required that too). 

We use meson and cross-compilation files, everything is wrapped into shell script, so you just need to run:


```bash
./platforms/build-3ds.sh 

```
After that you can find the runnable executable (also supports emulators)
in `build-3ds/oopetris.3dsx`
