#include "storm/storage/geometry/NativePolytope.h"

#ifdef STasdf

#include "storm/utility/macros.h"
#include "storm/exceptions/InvalidArgumentException.h"
#include "storm/exceptions/UnexpectedException.h"
#include "storm/utility/solver.h"

namespace storm {
    namespace storage {
        namespace geometry {
            
            template <typename ValueType>
            NativePolytope<ValueType>::NativePolytope(std::vector<Halfspace<ValueType>> const& halfspaces) {
                if(halfspaces.empty()){
                    // The polytope is universal
                    emptyStatus = EmptyStatus::Nonempty;
                } else {
                    uint_fast64_t maxCol = halfspaces.front().normalVector().size();
                    uint_fast64_t maxRow = halfspaces.size();
                    A = EigenMatrix(maxRow, maxCol);
                    b = EigenVector(maxRow);
                    for ( uint_fast64_t row = 0; row < maxRow; ++row ){
                        assert(halfspaces[row].normal().rows() == maxCol);
                        b(row) = halfspaces[row].offset();
                        A(row) = storm::adapters::EigenAdapter<ValueType>::toEigenVector(halfspaces[row].normal());
                    }
                    emptyStatus = EmptyStatus::Unknown;
                }
            }
            
            template <typename ValueType>
            NativePolytope<ValueType>::NativePolytope(std::vector<Point> const& points) {
                if(points.empty()){
                    emptyStatus = EmptyStatus::Empty;
                } else {
                    std::vector<EigenVector> eigenPoints;
                    eigenPoints.reserve(points.size());
                    for(auto const& p : points){
                        eigenPoints.emplace_back(storm::adapters::EigenAdapter<ValueType>::toEigenVector(p));
                    }

                    // todo: quickhull(eigenPoints)

                    emptyStatus = EmptyStatus::Nonempty;
                }
            }
            
            template <typename ValueType>
            std::shared_ptr<Polytope<ValueType>> NativePolytope<ValueType>::create(boost::optional<std::vector<Halfspace<ValueType>>> const& halfspaces,
                                                                             boost::optional<std::vector<Point>> const& points) {
                if(halfspaces) {
                    STORM_LOG_WARN_COND(!points, "Creating a NativePolytope where halfspaces AND points are given. The points will be ignored.");
                    return std::make_shared<NativePolytope<ValueType>>(*halfspaces);
                } else if(points) {
                    return std::make_shared<NativePolytope<ValueType>>(*points);
                }
                STORM_LOG_THROW(false, storm::exceptions::UnexpectedException, "Creating a NativePolytope but no representation was given.");
                return nullptr;
            }
            
            template <typename ValueType>
            NativePolytope<ValueType>::NativePolytope(NativePolytope<ValueType> const& other) : emptyStatus(other.emptyStatus), A(other.A), b(other.b) {
                // Intentionally left empty
            }
            
            template <typename ValueType>
            NativePolytope<ValueType>::NativePolytope(NativePolytope<ValueType>&& other) : emptyStatus(std::move(other.emptyStatus)), A(std::move(other.A)), b(std::move(other.b)) {
                // Intentionally left empty
            }
            
            template <typename ValueType>
            NativePolytope<ValueType>::NativePolytope(EmptyStatus const& emptyStatus, EigenMatrix const& halfspaceMatrix, EigenVector const& halfspaceVector) : emptyStatus(emptyStatus), A(halfspaceMatrix), b(halfspaceVector) {
                // Intentionally left empty
            }
            
            template <typename ValueType>
            NativePolytope<ValueType>::NativePolytope(EmptyStatus&& emptyStatus, EigenMatrix&& halfspaceMatrix, EigenVector&& halfspaceVector) : emptyStatus(emptyStatus), A(halfspaceMatrix), b(halfspaceVector) {
                // Intentionally left empty
            }
            
            template <typename ValueType>
            NativePolytope<ValueType>::~NativePolytope() {
                // Intentionally left empty
            }
            
            template <typename ValueType>
            std::vector<typename Polytope<ValueType>::Point> NativePolytope<ValueType>::getVertices() const {
                std::vector<hypro::Point<ValueType>> eigenVertices = getEigenVertices();
                std::vector<Point> result;
                result.reserve(eigenVertices.size());
                for(auto const& p : eigenVertices) {
                    result.push_back(storm::adapters::EigenAdapter<ValueType>::toStdVector(p));
                }
                return result;
            }
            
            template <typename ValueType>
            std::vector<Halfspace<ValueType>> NativePolytope<ValueType>::getHalfspaces() const {
                std::vector<Halfspace<ValueType>> result;
                result.reserve(A.rows());

                for(uint_fast64_t row=0; row < A.rows(); ++row){
                    result.emplace_back(storm::adapters::EigenAdapter<ValueType>::toStdVector(A.row(row)), b.row(row));
                }
            }

            template <typename ValueType>
            bool NativePolytope<ValueType>::isEmpty() const {
                if(emptyStatus == emptyStatus::Unknown) {
                    storm::expressions::ExpressionManager manager;
                    std::unique_ptr<storm::solver::SmtSolver> solver = storm::utility::solver::SmtSolverFactory().create(manager);
                    storm::expressions::Expression constraints = getConstraints(manager, declareVariables(manager, "x"));
                    solver->add(constraints);
                    switch(solver->check()) {
                    case storm::solver::SmtSolver::CheckResult::Sat:
                        emptyStatus = emptyStatus::Nonempty;
                        break;
                    case storm::solver::SmtSolver::CheckResult::Unsat:
                        emptyStatus = emptyStatus::Empty;
                        break;
                    default:
                        STORM_LOG_THROW(false, storm::exceptions::UnexpectedException, "Unexpected result of SMT solver during emptyness-check of Polytope.");
                        break;
                    }
                }
                return emptyStatus == emptyStatus::Empty;
            }
            
            template <typename ValueType>
            bool NativePolytope<ValueType>::isUniversal() const {
                return A.rows()==0;
            }
            
            template <typename ValueType>
            bool NativePolytope<ValueType>::contains(Point const& point) const{
                EigenVector x = storm::adapters::EigenAdapter<ValueType>::toEigenVector(point);
                for(uint_fast64_t row=0; row < A.rows(); ++row){
                    if((A.row(row) * x)(0) > b(row)){
                        return false;
                    }
                }
                return true;
            }

            template <typename ValueType>
            bool NativePolytope<ValueType>::contains(std::shared_ptr<Polytope<ValueType>> const& other) const {
                STORM_LOG_THROW(other->isNativePolytope(), storm::exceptions::InvalidArgumentException, "Invoked operation between a NativePolytope and a different polytope implementation. This is not supported");
                    // Check whether there is one point in other that is not in this
                    storm::expressions::ExpressionManager manager;
                    std::unique_ptr<storm::solver::SmtSolver> solver = storm::utility::solver::SmtSolverFactory().create(manager);
                    storm::expressions::Expression constraintsThis = this->getConstraints(manager, declareVariables(manager, "x"));
                    solver->add(!constraintsThis);
                    storm::expressions::Expression constraintsOther = dynamic_cast<NativePolytope<ValueType> const&>(*other).getConstraints(manager, declareVariables(manager, "y"));
                    solver->add(constraintsOther);

                    switch(solver->check()) {
                    case storm::solver::SmtSolver::CheckResult::Sat:
                        return false;
                    case storm::solver::SmtSolver::CheckResult::Unsat:
                        return true;
                    default:
                        STORM_LOG_THROW(false, storm::exceptions::UnexpectedException, "Unexpected result of SMT solver during containment check of two polytopes.");
                        return false;
                    }
            }
            
            template <typename ValueType>
            std::shared_ptr<Polytope<ValueType>> NativePolytope<ValueType>::intersection(std::shared_ptr<Polytope<ValueType>> const& rhs) const{
                STORM_LOG_THROW(rhs->isNativePolytope(), storm::exceptions::InvalidArgumentException, "Invoked operation between a NativePolytope and a different polytope implementation. This is not supported");
                NativePolytope<ValueType> const& nativeRhs = dynamic_cast<NativePolytope<ValueType> const&>(*rhs);
                EigenMatrix resultA(A.rows() + nativeRhs.A.rows(), A.cols());
                resultA << A,
                           nativeRhs.A;
                EigenVector resultb(resultA.rows());
                resultb << b,
                           nativeRhs.b;
                return std::make_shared<NativePolytope<ValueType>>(std::move(resultA), std::move(resultb));
            }
            
            template <typename ValueType>
            std::shared_ptr<Polytope<ValueType>> NativePolytope<ValueType>::intersection(Halfspace<ValueType> const& halfspace) const{
                EigenMatrix resultA(A.rows() + 1, A.cols());
                resultA << A,
                           storm::adapters::EigenAdapter<ValueType>::toEigenVector(halfspace.normal());
                EigenVector resultb(resultA.rows());
                resultb << b,
                           halfspace.offset();
                return std::make_shared<NativePolytope<ValueType>>(std::move(resultA), std::move(resultb));
            }
            
            template <typename ValueType>
            std::shared_ptr<Polytope<ValueType>> NativePolytope<ValueType>::convexUnion(std::shared_ptr<Polytope<ValueType>> const& rhs) const{
                STORM_LOG_THROW(rhs->isNativePolytope(), storm::exceptions::InvalidArgumentException, "Invoked operation between a NativePolytope and a different polytope implementation. This is not supported");
                if(this->isEmpty()) {
                    return std::make_shared<NativePolytope<ValueType>>(dynamic_cast<NativePolytope<ValueType> const&>(*rhs));
                } else if (rhs->isEmpty()) {
                    return std::make_shared<NativePolytope<ValueType>>(*this);
                }
                if(this->isUniversal() || rhs->isUniversal) {
                    return std::make_shared<NativePolytope<ValueType>>(*this);
                }

                STORM_LOG_WARN("Implementation of convex union of two polytopes only works if the polytopes are bounded.");

                std::vector<EigenVector> rhsVertices = dynamic_cast<NativePolytope<ValueType> const&>(*rhs).getEigenVertices();
                std::vector<EigenVector> resultVertices = this->getEigenVertices();
                resultVertices.insert(resultVertices.end(), std::make_move_iterator(rhsVertices.begin()), std::make_move_iterator(rhsVertices.end()));

                // todo invoke quickhull

                return nullptr;
            }
            
            template <typename ValueType>
            std::shared_ptr<Polytope<ValueType>> NativePolytope<ValueType>::minkowskiSum(std::shared_ptr<Polytope<ValueType>> const& rhs) const{
                STORM_LOG_THROW(rhs->isNativePolytope(), storm::exceptions::InvalidArgumentException, "Invoked operation between a NativePolytope and a different polytope implementation. This is not supported");
                NativePolytope<ValueType> const& nativeRhs = dynamic_cast<NativePolytope<ValueType> const&>(*rhs);

                return std::make_shared<NativePolytope<ValueType>>(internPolytope.minkowskiSum(dynamic_cast<NativePolytope<ValueType> const&>(*rhs).internPolytope));
            }
            
            template <typename ValueType>
            std::shared_ptr<Polytope<ValueType>> NativePolytope<ValueType>::affineTransformation(std::vector<Point> const& matrix, Point const& vector) const{
                STORM_LOG_THROW(!matrix.empty(), storm::exceptions::InvalidArgumentException, "Invoked affine transformation with a matrix without rows.");
                hypro::matrix_t<ValueType> hyproMatrix(matrix.size(), matrix.front().size());
                for(uint_fast64_t row = 0; row < matrix.size(); ++row) {
                    hyproMatrix.row(row) = storm::adapters::toNative(matrix[row]);
                }
                return std::make_shared<NativePolytope<ValueType>>(internPolytope.affineTransformation(std::move(hyproMatrix), storm::adapters::toNative(vector)));
            }
            
            template <typename ValueType>
            std::pair<typename NativePolytope<ValueType>::Point, bool> NativePolytope<ValueType>::optimize(Point const& direction) const {
                hypro::EvaluationResult<ValueType> evalRes = internPolytope.evaluate(storm::adapters::toNative(direction));
                switch (evalRes.errorCode) {
                    case hypro::SOLUTION::FEAS:
                        return std::make_pair(storm::adapters::fromNative(evalRes.optimumValue), true);
                    case hypro::SOLUTION::UNKNOWN:
                        STORM_LOG_THROW(false, storm::exceptions::UnexpectedException, "Unexpected eror code for Polytope evaluation");
                        return std::make_pair(Point(), false);
                    default:
                        //solution is infinity or infeasible
                        return std::make_pair(Point(), false);
                }
            }
            
            template <typename ValueType>
            bool NativePolytope<ValueType>::isNativePolytope() const {
                return true;
            }
            template <typename ValueType>
            std::vector<typename NativePolytope::ValueType>::EigenVector> NativePolytope<ValueType>::getEigenVertices() const {
                // todo: invoke conversion
                return std::vector<EigenVector>();
            }

            template <typename ValueType>
            std::vector<storm::expressions::Variable> NativePolytope<ValueType>::declareVariables(storm::expressions::ExpressionManager& manager, std::string const& namePrefix) const {
                std::vector<storm::expressions::Variable> result;
                result.reserve(A.cols());
                for(uint_fast64_t col=0; col < A.cols(); ++col){
                    result.push_back(manager->declareVariable(namePrefix + std::to_string(col), manager->getRationalType()));
                }
                return result;
            }

            template <typename ValueType>
            storm::expressions::Expression NativePolytope<ValueType>::getConstraints(storm::expressions::ExpressionManager& manager, std::vector<storm::expressions::Variable> const& variables) cons {
                storm::expressions::Expression result = manager.boolean(true);
                for(uint_fast64_t row=0; row < A.rows(); ++row) {
                    storm::expressions::Expression lhs = manager.rational(A(row,0)) * variables[0].getExpression();
                    for(uint_fast64_t col=1; col < A.cols(); ++col) {
                        lhs = lhs + manager.rational(A(row,col)) * variables[col].getExpression();
                    }
                    result = result && (lhs <= manager.rational(b(row)));
                }
                return result;
            }


            template class NativePolytope<double>;
#ifdef STORM_HAVE_CARL
            template class NativePolytope<storm::RationalNumber>;
#endif
        }
    }
}
#endif
