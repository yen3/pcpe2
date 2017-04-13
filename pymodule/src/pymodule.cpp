#ifdef __USE_PCPE2_PYTHON_MODULE__

#include <iostream>
#include <string>
#include <sstream>

#include "Python.h"

#include "pymodule.h"

void
pcpe2_inner(const char* raw_seq_fn_x,
            const char* raw_seq_fn_y,
            const char* raw_output_fn)
{
  pcpe::Filename input_seq_x_fn(raw_seq_fn_x);
  pcpe::Filename input_seq_y_fn(raw_seq_fn_y);
  pcpe::Filename output_fn(raw_output_fn);

  pcpe::common_peptide_explorer(input_seq_x_fn, input_seq_y_fn, output_fn);
}


///////////////////////////////////////////////////////////////////////////////
// Python moudle setting
///////////////////////////////////////////////////////////////////////////////
#ifdef __cplusplus
extern "C" {
#endif

static char py_pcpe2_doc[] = "Test for pcpe2 python module";

static PyObject *
py_pcpe2(PyObject *self, PyObject *args) {
    char* input_x;
    char* input_y;
    char* output_fn;

    if (!PyArg_ParseTuple(args, "sss:pcpe2", &input_x, &input_y, &output_fn)) {
        return NULL;
    }

    pcpe2_inner(input_x, input_y, output_fn);

    Py_INCREF(Py_None);
    return Py_None;
}

static PyMethodDef _pcpe2methods[] = {
      {"pcpe2", py_pcpe2, METH_VARARGS, py_pcpe2_doc},
      {NULL, NULL, 0, NULL}
};

#if PY_MAJOR_VERSION < 3
#error "The module does not support Python 2"
#else

/* Python 3 module initialization */
static struct PyModuleDef _pcpe2module = {
    PyModuleDef_HEAD_INIT,
    "_pcpe2", /* name of module */
    NULL, /* module documentation, may be NULL */
    -1,
    _pcpe2methods
};

PyMODINIT_FUNC PyInit__pcpe2(void) {
    PyObject *mod;

    mod = PyModule_Create(&_pcpe2module);

    return mod;
}
#endif

#ifdef __cplusplus
} // extern "C"
#endif

#endif /* ifdef(__USE_PCPE2_PYTHON_MODULE__) */
