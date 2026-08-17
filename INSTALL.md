# Installing and consuming multobjopt

`multobjopt` installs as a standard CMake config package. Applications are
expected to consume an installed library through the imported target
`multobjopt::multobjopt`.

## Requirements

- CMake 3.20 or newer
- A compiler with C++20 support
- No third-party runtime libraries

## Install from source

Configure a release build without the repository tests and examples:

```sh
cmake -S . -B build-install \
  -DCMAKE_BUILD_TYPE=Release \
  -DMULTOBJOPT_BUILD_TESTS=OFF \
  -DMULTOBJOPT_BUILD_EXAMPLES=OFF
cmake --build build-install --parallel
cmake --install build-install --prefix /path/to/multobjopt-prefix
```

For a multi-configuration generator, select the configuration while building
and installing:

```sh
cmake --build build-install --config Release
cmake --install build-install --config Release \
  --prefix /path/to/multobjopt-prefix
```

Set `BUILD_SHARED_LIBS=ON` while configuring to build a shared library. The
default is the platform's normal static-library setting.

The installed prefix contains the public headers, library, and CMake package
files. Installation does not modify global compiler or linker settings.

## Consume an installed library with CMake

A minimal consumer `CMakeLists.txt` is:

```cmake
cmake_minimum_required(VERSION 3.20)

project(my_design LANGUAGES CXX)

find_package(multobjopt CONFIG REQUIRED)

add_executable(my_design main.cpp)
target_link_libraries(my_design PRIVATE multobjopt::multobjopt)
target_compile_features(my_design PRIVATE cxx_std_20)
```

When `multobjopt` is installed in a custom prefix, point CMake at that prefix:

```sh
cmake -S . -B build \
  -DCMAKE_PREFIX_PATH=/path/to/multobjopt-prefix
cmake --build build --parallel
```

Alternatively, set `multobjopt_DIR` to the directory containing
`multobjoptConfig.cmake`, normally:

```sh
cmake -S . -B build \
  -Dmultobjopt_DIR=/path/to/multobjopt-prefix/lib/cmake/multobjopt
```

The library directory may be `lib64` or another platform-specific location.
Using `CMAKE_PREFIX_PATH` is therefore usually more portable.

## Read the installed library version

The installed package includes its generated version header:

```cpp
#include <multobjopt/version.hpp>

#include <iostream>

int main() {
    std::cout << multobjopt::version::library_version << '\n';
}
```

`library_version` contains the Git description recorded when the library was
built, including a `-dirty` suffix when applicable. Use `semantic_version` for
compatibility checks that should ignore Git metadata. Source archives without
a Git commit, including vendored copies without their own `.git` entry, use the
CMake project version for both values.

## Build the examples against the installed package

The examples directory is also a standalone CMake project. With an installed
library, build all examples without rebuilding `multobjopt` itself:

```sh
cmake -S examples -B build-examples \
  -DCMAKE_PREFIX_PATH=/path/to/multobjopt-prefix \
  -DCMAKE_BUILD_TYPE=Release
cmake --build build-examples --parallel
```

For example, run the discrete mechatronic design demonstration from a
single-configuration build with:

```sh
./build-examples/multobjopt_example_04_mechatronic_discrete_auto
```

See [README.md](README.md#examples) for the purpose of each example.

## Verify an installation

This repository includes an external consumer test that installs the current
build to a temporary build-local prefix, locates it with `find_package`, and
links a separate executable:

```sh
cmake -S . -B build -DMULTOBJOPT_BUILD_TESTS=ON
cmake --build build --parallel
ctest --test-dir build --output-on-failure -R 25_test_install_package
```

There is no generated uninstall target. To remove a custom installation,
remove only the prefix selected explicitly for `cmake --install`, or use the
install manifest to identify installed files.
