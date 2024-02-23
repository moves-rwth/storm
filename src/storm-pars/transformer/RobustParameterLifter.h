#pragma once

#include <memory>
#include <set>
#include <unordered_map>
#include <vector>

#include "adapters/RationalFunctionForward.h"
#include "adapters/RationalNumberForward.h"
#include "storm-pars/analysis/Order.h"
#include "storm-pars/storage/ParameterRegion.h"
#include "storm-pars/utility/parametric.h"
#include "storm/solver/OptimizationDirection.h"
#include "storm/storage/BitVector.h"
#include "storm/storage/SparseMatrix.h"

#include "storm-pars/analysis/MonotonicityChecker.h"

namespace storm {
namespace transformer {

/*!
 * This class lifts parameter choices to nondeterminism:
 * For each row in the  given matrix that considerd #par parameters, the resulting matrix will have one row group consisting of 2^#par rows.
 * When specifying a region, each row within the row group is evaluated w.r.t. one vertex of the region.
 * The given vector is handled similarly.
 * However, if a vector entry considers a parameter that does not occur in the corresponding matrix row,
 * the parameter is directly set such that the vector entry is maximized (or minimized, depending on the specified optimization direction).
 *
 * @note The row grouping of the original matrix is ignored.
 */
template<typename ParametricType, typename ConstantType>
class RobustParameterLifter {
   public:
    typedef typename storm::utility::parametric::VariableType<ParametricType>::type VariableType;
    typedef typename storm::utility::parametric::CoefficientType<ParametricType>::type CoefficientType;
    typedef typename storm::analysis::MonotonicityResult<VariableType>::Monotonicity Monotonicity;

    /*!
     * Lifts the parameter choices to nondeterminisim. The computation is performed on the submatrix specified by the selected rows and columns
     * @param pMatrix the parametric matrix
     * @param pVector the parametric vector (the vector size should equal the row count of the matrix)
     * @param selectedRows a Bitvector that specifies which rows of the matrix and the vector are considered.
     * @param selectedColumns a Bitvector that specifies which columns of the matrix are considered.
     */
    RobustParameterLifter(storm::storage::SparseMatrix<ParametricType> const& pMatrix, std::vector<ParametricType> const& pVector,
                          storm::storage::BitVector const& selectedRows, storm::storage::BitVector const& selectedColumns, bool generateRowLabels = false);

    void specifyRegion(storm::storage::ParameterRegion<ParametricType> const& region, storm::solver::OptimizationDirection const& dirForParameters);

    /*!
     * Specifies the region for the parameterlifter, the Bitvector works as a fixed (partial) scheduler, this might not give sound results!
     * @param region the region
     * @param dirForParameters the optimization direction
     * @param selectedRows a Bitvector that specifies which rows of the matrix and the vector are considered.
     */
    void specifyRegion(storm::storage::ParameterRegion<ParametricType> const& region, storm::solver::OptimizationDirection const& dirForParameters,
                       storm::storage::BitVector const& selectedRows);

    /*!
     * Specifies the region for the parameterlifter, the reachability order is used to see if there is local monotonicity, such that a fixed (partial) scheduler
     * can be used
     * @param region the region
     * @param dirForParameters the optimization direction
     * @param reachabilityOrder a (possibly insufficient) reachability order, used for local monotonicity
     */
    void specifyRegion(storm::storage::ParameterRegion<ParametricType> const& region, storm::solver::OptimizationDirection const& dirForParameters,
                       std::shared_ptr<storm::analysis::Order> reachabilityOrder,
                       std::shared_ptr<storm::analysis::LocalMonotonicityResult<VariableType>> localMonotonicityResult);

    // Returns the resulting matrix. Should only be called AFTER specifying a region
    storm::storage::SparseMatrix<Interval> const& getMatrix() const;

    // Returns the resulting vector. Should only be called AFTER specifying a region
    std::vector<Interval> const& getVector() const;

    // Returns whether the curent region is all ill-defined.
    bool isCurrentRegionAllIllDefined() const;

   private:
    /*
     * We minimize the number of function evaluations by only calling evaluate() once for each unique pair of function and valuation.
     * The result of each evaluation is then written to all positions in the matrix (and the vector) where the corresponding (function,valuation) occurred.
     */

    class RobustAbstractValuation {
       public:
        RobustAbstractValuation(RationalFunction transition);
        RobustAbstractValuation(RobustAbstractValuation const& other) = default;
        bool operator==(RobustAbstractValuation const& other) const;

        std::size_t getHashValue() const;

        std::set<VariableType> const& getParameters() const;

        uint64_t getNumTransitions() const;

        storm::RationalFunction const& getTransition() const;

        std::map<VariableType, std::set<CoefficientType>> const& getExtrema() const;

        std::set<CoefficientType> cubicEquationZeroes(RawPolynomial polynomial, VariableType parameter);

       private:
        boost::optional<std::pair<std::pair<uint_fast64_t, uint_fast64_t>, std::pair<CoefficientType, CoefficientType>>> recursiveDecompose(
            RawPolynomial polynomial, VariableType parameter, bool firstIteration = true);

        std::set<VariableType> parameters;

        storm::RationalFunction const transition;

        // Position and value of the extrema of each of the functions
        std::map<VariableType, std::set<CoefficientType>> extrema;
    };

    /*!
     * Collects all occurring pairs of functions and (abstract) valuations.
     * We also store a placeholder for the result of each pair. The result is computed and written into the placeholder whenever a region and optimization
     * direction is specified.
     */
    class FunctionValuationCollector {
       public:
        FunctionValuationCollector() = default;

        /*!
         * Adds the provided function and valuation.
         * Returns a reference to a placeholder in which the evaluation result will be written upon calling evaluateCollectedFunctions)
         */
        Interval& add(RobustAbstractValuation const& valuation);

        bool evaluateCollectedFunctions(storm::storage::ParameterRegion<ParametricType> const& region,
                                        storm::solver::OptimizationDirection const& dirForUnspecifiedParameters);

       private:
        class RobustAbstractValuationHash {
           public:
            std::size_t operator()(RobustAbstractValuation const& valuation) const {
                return valuation.getHashValue();
            }
        };

        // Stores the collected functions with the valuations together with a placeholder for the result.
        std::unordered_map<RobustAbstractValuation, Interval, RobustAbstractValuationHash> collectedValuations;
    };

    FunctionValuationCollector functionValuationCollector;

    storm::storage::SparseMatrix<Interval> matrix;  // The resulting matrix;
    std::vector<std::pair<typename storm::storage::SparseMatrix<Interval>::iterator, Interval&>>
        matrixAssignment;  // Connection of matrix entries with placeholders

    std::vector<RobustAbstractValuation> rowLabels;

    std::vector<uint64_t> oldToNewColumnIndexMapping;  // Mapping from old to new columnIndex
    std::vector<uint64_t> oldToNewRowIndexMapping;     // Mapping from old to new columnIndex
    std::vector<uint64_t> rowGroupToStateNumber;       // Mapping from new to old columnIndex

    bool currentRegionAllIllDefined = false;

    std::vector<Interval> vector;
    std::vector<std::pair<typename std::vector<Interval>::iterator, Interval&>> vectorAssignment;  // Connection of vector entries with placeholders
};

}  // namespace transformer
}  // namespace storm
