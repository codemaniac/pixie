# Building Pixie

Depending on the platform, the build process may differ.

## Table of contents

* [Supported platforms](#supported-platforms)
* [Building Pixie on supported platforms](#building-pixie-on-supported-platforms)
    * [Optional Prerequisites](#optional-prerequisites)
    * [Autogenerating Makefile](#autogenerating-makefile)
    * [Build Configurations](#build-configurations)
    * [Windows](#windows)
    * [Linux](#linux)
    * [MacOS](#macos)
* [Running Tests](#running-tests)

## Supported platforms

This is the list of platforms currently supported.

| Operating System | Architecture |
| ---------------- | ------------ |
| Windows          | x64          |
| GNU/Linux        | x64          |
| MacOS            | x64          |

## Building Pixie on supported platforms

### Optional Prerequisites

* [Premake](https://premake.github.io) to auto generate build scripts
* [ASan](https://github.com/google/sanitizers)
* [UBSan](https://github.com/google/sanitizers)

### Autogenerating Makefile

Although `Makefile` is included in the repository, in case there is a need to regenerate it, run:

```
premake5 gmake2
```

Alternatively, to create a build with ASan and UBSan, generate the `Makefile` using the `--enable-san` switch. This will enable ASan and UBSan on all of the [build configurations](#build-configurations). This is to be used only to perform memory error checks and undefined behavior checks.

```
premake5 gmake2 --enable-san
```

### Build Configurations

The following configurations exist. To access the full list of available configurations, run `make help`.

| Configuration | Description                                                      |
|---------------| -----------------------------------------------------------------|
| debug         | To be used during development only                               |
| test          | Runs only the tests and not the engine                           |
| release       | To be used for building the engine for real-life use             |

### Windows

#### Windows Prerequisites

* Requires `mingw-w64-ucrt-x86_64-gcc` to be installed and added to `path` using [msys2](https://www.msys2.org)

#### Building on Windows

To build Pixie on Windows, run:

```
make config=release_windows64
```

### Linux

#### Linux Prerequisites

* `gcc`
* `build-essential` to be able to run `make`

#### Building on Linux

To build Pixie on Linux, run:

```
make config=release_linux64
```

### MacOS

#### MacOS Prerequisites

* `Apple clang version 15.0.0` or higher

#### Building on MacOS

To build Pixie on MacOS, run:

```
make config=release_macos64
```

## Running Tests

First, create the test build for Pixie. Below is an example of creating the test build on MacOS. For Windows or Linux, change the config value to `test_windows64` or `test_linux64` accordingly.

```
make config=test_macos64
```

Once the test build is created, run:

```
./bin/Test/pixie
```
