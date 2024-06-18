# Nintendo Wii build

## Prerequisites

- a C++23 compatible cross compiler, we only support The [devkitpro](https://devkitpro.org/) one
- meson
- some portlibs from devkitpro, namely sdl2, sdl2_ttf, sdl2_mix.


## Compiling


Cross Compiling is only possible on linux-like systems.

We use meson and cross-compilation files, everything is wrapped into shell script, so you just need to run:


```bash
./platforms/build-wii.sh 

```
After that you can find the runnable executable (also supports emulators)
in `build-wii/oopetris.dol`

Typically this would be moved into a folder on an external SD card of the WII, than you can load it with Homebrew.


The Folder structure would be:

`/apps/OOPetris/`
- `boot.dol`
- `icon.png`
- `meta.xml`

At the moment we don't package that like this, but in the future we might.

The `meta.xml` file is being generated, the `boot.dol` file has to be renamed and the `icon.png` can be taken from the `assets/icon/` folder
