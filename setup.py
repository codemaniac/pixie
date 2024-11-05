from setuptools import setup, Extension

module = Extension(
    "pypixie",
    sources=[
        "src/pypixie.cpp",
        "src/board.cpp",
        "src/eval.cpp",
        "src/fen.cpp",
        "src/move.cpp",
        "src/perft.cpp",
        "src/position.cpp",
        "src/search.cpp",
        "src/threadpool.cpp",
        "src/tt.cpp",
        "src/utils.cpp"
    ],
    extra_compile_args=[
        "-std=c++20",
        "-Wall",
        "-Wextra",
        "-pedantic",
        "-O3",
        "-flto"
    ]
)

setup(
    name="pypixie",
    version="0.2.2",
    description="A free UCI chess engine",
    license_files = ('LICENSE',),
    ext_modules=[module],
)
