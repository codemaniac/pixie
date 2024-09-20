# pixie
A free UCI chess engine

# Compiling pixie
## On MacOS 64-bit

```
premake5 gmake2
make config=release_macos64
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
