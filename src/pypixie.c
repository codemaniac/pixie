#include "include/chess.h"
#include "include/fen.h"
#include "include/perft.h"
#include "include/search.h"
#include "include/transpositiontable.h"
#include "include/utils.h"
#include <Python.h>

static PyObject* pypixie_perft(PyObject* self, PyObject* args) {
    const char* fen;
    int         depth;
    if (!PyArg_ParseTuple(args, "si", &fen, &depth))
    {
        return NULL;
    }

    Position position = position_create();
    fen_to_position(fen, &position);

    uint64_t result = perft(&position, depth);

    return PyLong_FromLong(result);
}

static PyObject* pypixie_search(PyObject* self, PyObject* args) {
    const char* fen;
    int         depth;
    if (!PyArg_ParseTuple(args, "si", &fen, &depth))
    {
        return NULL;
    }

    Position position = position_create();
    fen_to_position(fen, &position);

    TranspositionTable table;
    hashtable_init(&table);

    SearchInfo info;
    info.depth     = (uint8_t) depth;
    info.timeset   = false;
    info.starttime = utils_time_curr_time_ms();
    info.nodes     = 0ULL;

    int32_t eval = search(&position, &table, &info, false, false);

    free(table.contents);
    table.contents = NULL;

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
