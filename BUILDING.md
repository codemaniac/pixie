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
| GNU/Linux        | x64          |
| Windows          | x64          |
| macOS            | x64          |

## Building Pixie on supported platforms

### Optional Prerequisites

* [Premake](https://premake.github.io) to auto generate build scripts
* [ASan](https://github.com/google/sanitizers)
* [UBSan] (https://github.com/google/sanitizers)

### Autogenerating Makefile

Although `Makefile` is included in the repository, in case there is a need to regenerate it, run:

```
premake5 gmake2
```

### Build Configurations

The following configurations exist. To access the full list of available configurations, run `make help`.

| Operating System    | debug         | test       | stage         | release   |
| ------------------- | ------------- | ---------- | ------------- | --------- |
| GNU/Linux (linux64) | Available     | Available  |               | Available |
| Windows (windows64) | Available     | Available  |               | Available |
| macOS (macos64)     | Available[^1] | Available  | Available[^1] | Available |

[^1]: Requires [ASan](https://github.com/google/sanitizers) and [UBSan] (https://github.com/google/sanitizers)

Below are the descriptions for each configuration:

| Configuration | Description                                                      |
|---------------| -----------------------------------------------------------------|
| debug         | To be used during development only                               |
| test          | Runs only the tests and not the engine                           |
| stage         | To be used for checking memory leaks and undefined behavior only |
| release       | To be used for building the engine for real-life use             |

### Windows

#### Windows Prerequisites

* Requires MinGW 64-bit to be installed and added to `path` using [msys2](https://www.msys2.org)

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
./bin/Test/pixie
```
