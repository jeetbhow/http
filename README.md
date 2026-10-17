# http

A minimal C++23 project built with Meson and Ninja.

Licensed under the [MIT License](LICENSE).

## Requirements

- A C++23 compiler
- Meson 0.63 or newer
- Ninja

## Build and run

With Make installed, use these shortcuts (build directories are configured automatically):

```sh
make              # Build the project
make run          # Build and run the executable
make release      # Build with release optimizations
make run-release  # Build and run the release executable
make clean        # Clean compiled files from both build directories
```

You can also invoke Meson directly:

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

## Unit tests

GoogleTest is provided through the pinned WrapDB package in
`subprojects/gtest.wrap`. Meson uses an installed GoogleTest when available and
downloads the wrap as a fallback. The first fallback build requires internet
access; no separate GoogleTest installation is needed.

```sh
make test
# Or, after configuring the build directory:
meson test -C build --print-errorlogs
```

Tests live in `tests/` and use GoogleTest's supplied `main()`. Add new test
sources to `tests/meson.build`. To explicitly test the WrapDB fallback, configure
with `meson setup build --reconfigure --force-fallback-for=gtest`.

## Send a test request

With the server running, run this in another terminal (requires Python 3.8+):

```sh
python3 scripts/test_request.py
```

The script sends `GET /` to `127.0.0.1:8080`. Use `--host` and `--port` to
override the destination. The server prints the request and exits after the
client finishes sending; it currently does not send an HTTP response. Restart
the server before sending another request.
