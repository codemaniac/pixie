#include "include/chess.h"
#include "include/fen.h"
#include "include/perft.h"
#include "include/search.h"
#include "include/utils.h"
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
    position_destroy(pos);
    return PyLong_FromLong(result);
}

static PyObject* pypixie_search(PyObject* self, PyObject* args) {
    const char* fen;
    int         depth;
    if (!PyArg_ParseTuple(args, "si", &fen, &depth))
    {
        return NULL;
    }
    Position*   pos  = fen_to_position(fen);
    SearchInfo* info = (SearchInfo*) malloc(sizeof(SearchInfo));
    info->depth      = (uint8_t) depth;
    info->timeset    = false;
    info->starttime  = utils_time_curr_time_ms();
    info->nodes      = 0ULL;
    Move    best_move;
    int32_t eval = search(pos, info, &best_move);
    position_destroy(pos);
    return PyLong_FromLong(eval);
}

static PyMethodDef PyPixieMethods[] = {{"perft", pypixie_perft, METH_VARARGS, "Perft"},
                                       {"search", pypixie_search, METH_VARARGS, "search"},
                                       {NULL, NULL, 0, NULL}};

static struct PyModuleDef pypixie = {PyModuleDef_HEAD_INIT, "pypixie", NULL, -1, PyPixieMethods};

PyMODINIT_FUNC PyInit_pypixie(void) {
    chess_initialize();
    return PyModule_Create(&pypixie);
}
