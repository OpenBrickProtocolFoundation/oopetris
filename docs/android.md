# Android build


You need:

- meson
- Java JDK >= 17



## Compiling


Cross Compiling is only possible on linux-like systems.

We use meson and cross-compilation files with the android [NDK](https://developer.android.com/ndk). This is a two step compilation process.
We support all 4 architectures, that the NDK support, namely: `armeabi-v7a, arm64-v8a, x86, x86_64`. You can build the apk and shared libraries for all of them or just for the one your android has (Hint: Emulators use x86_64 mostly, modern android phones arm64-v8a).

To build all just don't specify any architecture in the build steps.  

The first step runs a shell script, that downloads the used Android NDK and builds the `.so` (shared library) that we use with and android Java App, it so you just need to run:

```bash
./platforms/build-android.sh <arch> # one of the 4 mentioned ones, or nothing, to build all 
```


After that we need to make aan apk that consists of the shared library and the Native Android Java Code. To build this you need a java JDK >= 17.

To built it, run this command:



```bash
cd platforms/android/
./gradlew assembleDebug --no-daemon -PANDROID_ABI=<arch>  # one of the 4 mentioned ones, or nothing, to build all 
```

After that you can find the apk
in `platforms/android/app/build/outputs/apk/debug/app-<arch>-debug.apk`
