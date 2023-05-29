#include "GrowableFlexibleSparseMatrix.h"
#include "storm/storage/FlexibleSparseMatrix.h"


namespace storm{
namespace modelchecker {
namespace blackbox {

template<class ValueType>
void GrowableFlexibleSparseMatrix<ValueType>::addRow() {
    this->data.pushBack((std:vector<row_type>()))
}

template<class ValueType>
void GrowableFlexibleSparseMatrix<ValueType>::addRows(index_type num_rows) {
    for (index_type i = 0; i < num_rows; i++) {
        addRow();
    }
}

template<class ValueType>
void GrowableFlexibleSparseMatrix<ValueType>::addRows(std::vector<row_type> rows) {
    // fastest way to extend list according to 
    // https://stackoverflow.com/questions/313432/c-extend-a-vector-with-another-vector    
    this->data.reserve(this->data.size(), rows.size());
    this->data.insert(this->data.end(), rows.begin(), rows.end());
}

} // blackbox
} // modelchecker
} // storm