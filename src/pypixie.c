#include "include/chess.h"
#include "include/fen.h"
#include "include/perft.h"
#include <Python.h>

static PyObject* pypixie_perft(PyObject* self, PyObject* args) {
    const char* fen;
    int         depth;
    if (!PyArg_ParseTuple(args, "si", &fen, &depth))
    {
        return NULL;
    }
    Position* pos    = fen_to_position(fen);
    uint64_t  result = perft(pos, depth);
    return PyLong_FromLong(result);
}

static PyMethodDef PyPixieMethods[] = {{"perft", pypixie_perft, METH_VARARGS, "Perft"},
                                       {NULL, NULL, 0, NULL}};

static struct PyModuleDef pypixie = {PyModuleDef_HEAD_INIT, "pypixie", NULL, -1, PyPixieMethods};

PyMODINIT_FUNC PyInit_pypixie(void) {
    chess_initialize();
    return PyModule_Create(&pypixie);
}
