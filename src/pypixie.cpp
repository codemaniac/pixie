#include "include/fen.h"
#include "include/perft.h"
#include "include/position.h"
#include "include/search.h"
#include "include/threadpool.h"
#include "include/utils.h"
#include <Python.h>
#include <cstdint>
#include <memory>

static std::unique_ptr<ThreadPool>         pool;
static std::unique_ptr<TranspositionTable> table;

static PyObject* pypixie_perft(PyObject* self, PyObject* args) {
    const char* fen;
    int         depth;
    if (!PyArg_ParseTuple(args, "si", &fen, &depth))
    {
        return NULL;
    }

    std::unique_ptr<Position> position = std::make_unique<Position>();
    fen_to_position(fen, position);

    uint64_t result = perft_multithreaded(position, depth, pool, false);

    return PyLong_FromLong(result);
}

static PyObject* pypixie_perft_captures(PyObject* self, PyObject* args) {
    const char* fen;
    int         depth;
    if (!PyArg_ParseTuple(args, "si", &fen, &depth))
    {
        return NULL;
    }

    std::unique_ptr<Position> position = std::make_unique<Position>();
    fen_to_position(fen, position);

    uint64_t result = perft_multithreaded(position, depth, pool, true);

    return PyLong_FromLong(result);
}

static PyObject* pypixie_search(PyObject* self, PyObject* args) {
    const char* fen;
    int         depth;
    if (!PyArg_ParseTuple(args, "si", &fen, &depth))
    {
        return NULL;
    }

    std::unique_ptr<Position> position = std::make_unique<Position>();
    fen_to_position(fen, position);

    SearchInfo info;
    info.depth         = (uint8_t) depth;
    info.timeset       = false;
    info.starttime     = utils_get_current_time_in_milliseconds();
    info.stoptime      = 0;
    info.stopped       = false;
    info.nodes         = 0ULL;
    info.use_iterative = false;
    info.use_uci       = false;

    table->clear();

    int32_t eval = search(position, table, &info);

    return PyLong_FromLong(eval);
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
