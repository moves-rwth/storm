#include "common.h"

#include "storage/model.h"
#include "storage/matrix.h"

PYBIND11_PLUGIN(storage) {
    py::module m("storage");
    define_model(m);
    define_sparse_matrix(m);
    return m.ptr();
}
