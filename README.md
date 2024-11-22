# pixie

A free UCI chess engine

[![License](https://img.shields.io/github/license/codemaniac/pixie)](LICENSE)
[![Contributor Covenant](https://img.shields.io/badge/Contributor%20Covenant-2.1-4baaaa.svg)](CODE_OF_CONDUCT.md)
[![emoji-log](https://cdn.rawgit.com/ahmadawais/stuff/ca97874/emoji-log/flat-round.svg)](https://github.com/ahmadawais/Emoji-Log/)

# Compiling pixie

## On MacOS 64-bit

```
premake5 gmake2
make config=release_macos64
```

## On Linux 64-bit

```
premake5 gmake2
make config=release_linux64
```

## On Windows 64-bit

Requires MinGW 64-bit to be installed and added to `path` using [msys2](https://www.msys2.org)

```
premake5 gmake2
make config=release_windows64
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
python test/test_perft.py
python test/test_mate.py TestMate.test_mate_in_2
```

# Acknowledgements

- https://www.chessprogramming.org for concepts
- https://github.com/bluefeversoft/vice for concepts and code snippets
- https://github.com/maksimKorzh/bbc for concepts and code snippets
- https://github.com/AndyGrant/Ethereal for perft standard.epd file
- https://github.com/TerjeKir/EngineTests for mate .epd files
- https://github.com/kz04px/libchess for perft test cases
- https://github.com/kz04px/rawr for perft test cases
