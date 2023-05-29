#ifndef STORM_GROWABLEFLEXIBLESPARSEMATRIX_H
#define STORM_GROWABLEFLEXIBLESPARSEMATRIX_H

#include "storm/storage/FlexibleSparseMatrix.h"

namespace storm{
namespace modelchecker {
namespace blackbox {


/*
A simple extension of the FlexibleSparseMatrix class that allows you to add additional
rows to the matrix.

There are probably better alternatives to this approach, but I did not want to modify
FlexibleSparseMatrix
*/
template<class ValueType>
class GrowableFlexibleSparseMatrix: public storm::storage::FlexibleSparseMatrix<ValueType> {
   public:
    // use typedefs from FlexibleSparseMatrix
    typedef uint64_t index_type;
    typedef std::vector<index_type> row_type;

    /*!
     * default constructor for empty matrix
     */
    GrowableFlexibleSparseMatrix<ValueType>() = default;

    /*!
     * add an empty row to the matrix
     */
    void addRow();

    /*!
     * extend the matrix with multiple empty rows
     * @param num_rows number of empty rows to add
     */
    void addRows(index_type num_rows);

    /*!
     * extend the matrix with the given rows
     * @param rows vector of rows to extend the matrix with
     */
    void addRows(std::vector<row_type> rows);

};

} // blackbox
} // modelchecker
} // storm



#endif  // STORM_GROWABLEFLEXIBLESPARSEMATRIX_H