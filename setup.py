from setuptools import setup, Extension

module = Extension(
    "pypixie",
    sources=[
        "src/pypixie.c",
        "src/bitscan.c",
        "src/chess.c",
        "src/fen.c",
        "src/hashkey.c",
        "src/perft.c",
        "src/utils.c",
        "src/lib/logc/log.c"
    ]
)

setup(
    name="pypixie",
    version="0.1.0",
    description="Chess engine",
    ext_modules=[module],
)
