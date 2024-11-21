#include "include/fen.h"
#include "include/perft.h"
#include "include/position.h"
#include "include/search.h"
#include "include/threadpool.h"
#include "include/utils.h"
#include <Python.h>
#include <cstdint>
#include <memory>

static std::unique_ptr<TranspositionTable> table;
static std::unique_ptr<ThreadPool>         pool;

static PyObject* pypixie_perft(PyObject* self, PyObject* args) {
    const char* fen;
    int         depth;
    if (!PyArg_ParseTuple(args, "si", &fen, &depth))
    {
        return NULL;
    }

    Position position;
    fen_to_position(fen, &position);

    uint64_t result = perft_multithreaded(position, depth, *pool.get(), false);

    return PyLong_FromLong(result);
}

static PyObject* pypixie_perft_captures(PyObject* self, PyObject* args) {
    const char* fen;
    int         depth;
    if (!PyArg_ParseTuple(args, "si", &fen, &depth))
    {
        return NULL;
    }

    Position position;
    fen_to_position(fen, &position);

    uint64_t result = perft_multithreaded(position, depth, *pool.get(), true);

    return PyLong_FromLong(result);
}

static PyObject* pypixie_search(PyObject* self, PyObject* args) {
    const char* fen;
    int         depth;
    if (!PyArg_ParseTuple(args, "si", &fen, &depth))
    {
        return NULL;
    }

    Position position = Position();
    fen_to_position(fen, &position);

    SearchInfo info;
    info.depth         = (uint8_t) depth;
    info.timeset       = false;
    info.starttime     = utils_get_current_time_in_milliseconds();
    info.stoptime      = 0;
    info.stopped       = false;
    info.use_iterative = false;
    info.use_uci       = false;

    table->clear();

    std::pair<int32_t, uint64_t> result = search(position, table.get(), pool.get(), &info);

    return PyLong_FromLong(result.first);
}

static PyMethodDef PyPixieMethods[] = {
  {"perft", pypixie_perft, METH_VARARGS, "Perft"},
  {"perft_captures", pypixie_perft_captures, METH_VARARGS, "Perft Captures"},
  {"search", pypixie_search, METH_VARARGS, "Search"},
  {NULL, NULL, 0, NULL}};

static struct PyModuleDef pypixie = {PyModuleDef_HEAD_INIT, "pypixie", NULL, -1, PyPixieMethods};

PyMODINIT_FUNC PyInit_pypixie(void) {
    position_init();
    pool  = std::make_unique<ThreadPool>(4);
    table = std::make_unique<TranspositionTable>(64);
    return PyModule_Create(&pypixie);
}
