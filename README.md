# http

A minimal C++20 project built with Meson and Ninja.

## Requirements

- A C++20 compiler
- Meson 0.63 or newer
- Ninja

## Build and run

```sh
meson setup build
meson compile -C build
./build/http
```

For a release build:

```sh
meson setup build-release --buildtype=release
meson compile -C build-release
./build-release/http
```

The executable starts in `src/main.cpp`; build targets are defined in `meson.build`.
