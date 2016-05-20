#include "common.h"

#include "storage/matrix.h"

PYBIND11_PLUGIN(storage) {
    py::module m("storage");
    define_sparse_matrix(m);
    return m.ptr();
}
