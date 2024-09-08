from setuptools import setup, Extension

module = Extension(
    "pypixie",
    sources=[
        "src/pypixie.c",
        "src/chess.c",
        "src/eval.c",
        "src/fen.c",
        "src/hashkey.c",
        "src/perft.c",
        "src/search.c",
        "src/utils.c",
        "src/lib/logc/log.c"
    ],
    extra_compile_args=[
        "-std=gnu18",
        "-Wall",
        "-Wextra",
        "-pedantic",
        "-finline-functions",
        "-funroll-loops",
        "-DLOG_QUIET",
    ]
)

setup(
    name="pypixie",
    version="0.1.0",
    description="Chess engine",
    ext_modules=[module],
)
