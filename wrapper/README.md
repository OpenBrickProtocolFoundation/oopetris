# OOPetris wrapper

## What is this

These are wrappers of some OOPetris functionaility in other languages.
They are all in submodules alias in a seperate git repo.

They currently wrap this:

- OOPetris Recordings

Planned:

- OOPetris AI support

## Languages

Current:

- Node.js (C++ binding)
- C (C++ binding)
- Haskell (C binding)
- Python (C++ binding)
- Java (C++ binding)

Planned:

- Lua
- Rust

## OS Comaptibility table

| Language | Linux | Windows | macOS |
| :------: | :---: | :-----: | :---: |
|    JS    |   ✅   |    ✅    |   ✅   |
|    C     |   ✅   |    ✅    |   ✅   |
| Haskell  |   ✅   |    ✅    | ❌[^1] |
|  Python  |   ✅   |    ✅    | ⚠️[^2] |
|   Java   |   ✅   |    ✅    |   ✅   |

[^1]: A needed dependency doesn't yet compile on macOS
[^2]: Arm64 has some errors, that are related to the toolchain (pybind11)

## Other

For more information on how to get / build those, refer to the subfolders of the languages
