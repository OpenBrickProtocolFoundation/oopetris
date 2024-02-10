# Nintendo Switch build


You need:

- a C++23 compatible cross compiler, we only support The [devkitpro](https://devkitpro.org/) one
- meson
- some portlibs from devkitpro, namely sdl2, sld2_ttf, sdl2_mix.



## Compiling


Cross Compiling is only possible on linux-like systems.

We use meson and cross-compilation files, everythind is wrapped into shell script, so you just need to run:


```bash
./platforms/build-switch.sh 

```
After that you can find the runnable executable (also supports emulators)
in `build-switch/oopetris.nro`
