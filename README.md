# widberg-defs

An alternative implementation of defs.h from the Hex-Rays decompiler sdk.

### Overview

This project is an alternative implementation of defs.h from the Hex-Rays decompiler sdk, which defines common functions, types, attributes, etc... used in the decompiler output. The Hex-Rays identifiers are used for compatibility, but the implementations are original work. This header is intended to be used with the https://github.com/widberg/llvm-project-widberg-extensions project and relies on GCC/Clang builtins in some cases; however, you are free to modify it to suit your needs if you are using a different compiler. We try to use compiler builtins and the freestanding C standard library where possible as opposed to implementing things ourselves. The header should work with both C and C++. I wrote most of this on a plane using my phone, so there are probably some bugs. Please open an issue/pull request if you find any.

### Usage

You can download the header and add it to your project sources directly

`wget https://raw.githubusercontent.com/widberg/widberg-defs/master/include/defs.h`

or use CMake to add it as a dependency

```cmake
add_subdirectory(widberg-defs)
target_link_libraries(my_target widberg-defs)
```
