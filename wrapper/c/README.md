# OOpetris C Wrapper


## What is this?

This wraps oopetris functionality for C





## Platform support

We officially support Linux, Windows and MacOS, if any error occurs regarding those platform, feel free to open an issue

## How to obtain

You have to compile it yourself, we don't provide prebuilt-packages atm. The Ci may produce some artifacts, that are useable, but not optimized for installation or usage.

## How to build it yourself

This uses meson to build the wrapper, you need to install `libboopetris_recordings` to be able to use this.

Than you just can invoke `meson setup build` and `meson compile -C build` to build the library.

You can install this library by using `meson install -C build`. It comes with a pkg-config file, so it is usable for usage with other build systems.
