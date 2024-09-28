# pixie

A free UCI chess engine

[![License](https://img.shields.io/github/license/codemaniac/pixie)](LICENSE)
[![Contributor Covenant](https://img.shields.io/badge/Contributor%20Covenant-2.1-4baaaa.svg)](CODE_OF_CONDUCT.md)

# Compiling pixie

## On Linux 64-bit

```
premake5 gmake2
make config=release_linux64
```

## On MacOS 64-bit

```
premake5 gmake2
make config=release_macos64
```

## On Windows 64-bit

```
 make -f .\windows.make
```

# Debugging

## Checking for memory leaks on MacOS

```
export MallocStackLogging=1
leaks -atExit --list -- ./bin/Debug/pixie > leaks.txt
```

# Running Tests

## On Unix-like systems

To run tests, run the following

```
python setup.py develop
python tests/test_perft.py
python tests/test_mate.py TestMate.test_mate_in_2
```

# Acknowledgements

- https://www.chessprogramming.org for concepts
- https://github.com/bluefeversoft/vice for concepts and code snippets
- https://github.com/maksimKorzh/bbc for concepts and code snippets
- https://github.com/AndyGrant/Ethereal for perft standard.epd file
- https://github.com/TerjeKir/EngineTests for mate .epd files
