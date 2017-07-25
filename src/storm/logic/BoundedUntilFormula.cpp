#include "storm/logic/BoundedUntilFormula.h"

#include "storm/utility/constants.h"
#include "storm/utility/macros.h"
#include "storm/exceptions/InvalidArgumentException.h"

#include "storm/logic/FormulaVisitor.h"

#include "storm/exceptions/InvalidPropertyException.h"
#include "storm/exceptions/InvalidOperationException.h"

namespace storm {
    namespace logic {
        BoundedUntilFormula::BoundedUntilFormula(std::shared_ptr<Formula const> const& leftSubformula, std::shared_ptr<Formula const> const& rightSubformula, boost::optional<TimeBound> const& lowerBound, boost::optional<TimeBound> const& upperBound, TimeBoundReference const& timeBoundReference) : BinaryPathFormula(leftSubformula, rightSubformula), timeBoundReference({timeBoundReference}), lowerBound({lowerBound}), upperBound({upperBound}) {
            STORM_LOG_THROW(lowerBound || upperBound, storm::exceptions::InvalidArgumentException, "Bounded until formula requires at least one bound.");
        }

        BoundedUntilFormula::BoundedUntilFormula(std::shared_ptr<Formula const> const& leftSubformula, std::shared_ptr<Formula const> const& rightSubformula,std::vector<boost::optional<TimeBound>> const& lowerBounds, std::vector<boost::optional<TimeBound>> const& upperBounds, std::vector<TimeBoundReference> const& timeBoundReferences) : BinaryPathFormula(leftSubformula, rightSubformula), timeBoundReference(timeBoundReferences), lowerBound(lowerBounds), upperBound(upperBounds) {
            assert(timeBoundReferences.size() == upperBound.size());
            assert(timeBoundReferences.size() == lowerBound.size());
            STORM_LOG_THROW(this->getDimension() != 0, storm::exceptions::InvalidArgumentException, "Bounded until formula requires at least one dimension.");
            for(unsigned i = 0; i < timeBoundReferences.size(); ++i) {
                STORM_LOG_THROW(hasLowerBound(i) || hasUpperBound(i), storm::exceptions::InvalidArgumentException, "Bounded until formula requires at least one bound in each dimension.");
            }
        }


        bool BoundedUntilFormula::isBoundedUntilFormula() const {
            return true;
        }
        
        bool BoundedUntilFormula::isProbabilityPathFormula() const {
            return true;
        }
        
        boost::any BoundedUntilFormula::accept(FormulaVisitor const& visitor, boost::any const& data) const {
            return visitor.visit(*this, data);
        }
        
        void BoundedUntilFormula::gatherReferencedRewardModels(std::set<std::string>& referencedRewardModels) const {
            for (unsigned i = 0; i < this->getDimension(); ++i) {
                if (this->getTimeBoundReference(i).isRewardBound()) {
                    referencedRewardModels.insert(this->getTimeBoundReference().getRewardName());
                }
            }

            this->getLeftSubformula().gatherReferencedRewardModels(referencedRewardModels);
            this->getRightSubformula().gatherReferencedRewardModels(referencedRewardModels);
        }
        
        
        TimeBoundReference const& BoundedUntilFormula::getTimeBoundReference(unsigned i) const {
            assert(i < timeBoundReference.size());
            return timeBoundReference.at(i);
        }
        
        bool BoundedUntilFormula::isMultiDimensional() const {
            assert(timeBoundReference.size() != 0);
            return timeBoundReference.size() > 1;
        }

        unsigned BoundedUntilFormula::getDimension() const {
            return timeBoundReference.size();
        }
        
        bool BoundedUntilFormula::isLowerBoundStrict(unsigned i) const {
            assert(i < lowerBound.size());
            return lowerBound.at(i).get().isStrict();
        }
        
        bool BoundedUntilFormula::hasLowerBound() const {
            for(auto const& lb : lowerBound) {
                if (static_cast<bool>(lb)) {
                    return true;
                }
            }
            return false;
        }

        bool BoundedUntilFormula::hasLowerBound(unsigned i) const {
            return static_cast<bool>(lowerBound.at(i));
        }
        
        bool BoundedUntilFormula::hasIntegerLowerBound(unsigned i) const {
            return lowerBound.at(i).get().getBound().hasIntegerType();
        }

        bool BoundedUntilFormula::isUpperBoundStrict(unsigned i) const {
            return upperBound.at(i).get().isStrict();
        }

        bool BoundedUntilFormula::hasUpperBound() const {
            for(auto const& ub : upperBound) {
                if (static_cast<bool>(ub)) {
                    return true;
                }
            }
            return false;
        }

        bool BoundedUntilFormula::hasUpperBound(unsigned i) const {
            return static_cast<bool>(upperBound.at(i));
        }

        bool BoundedUntilFormula::hasIntegerUpperBound(unsigned i) const {
            return upperBound.at(i).get().getBound().hasIntegerType();
        }

        storm::expressions::Expression const& BoundedUntilFormula::getLowerBound(unsigned i) const {
            return lowerBound.at(i).get().getBound();
        }
        
        storm::expressions::Expression const& BoundedUntilFormula::getUpperBound(unsigned i) const {
            return upperBound.at(i).get().getBound();
        }
        
        template <>
        double BoundedUntilFormula::getLowerBound(unsigned i) const {
            checkNoVariablesInBound(this->getLowerBound());
            double bound = this->getLowerBound(i).evaluateAsDouble();
            STORM_LOG_THROW(bound >= 0, storm::exceptions::InvalidPropertyException, "Time-bound must not evaluate to negative number.");
            return bound;
        }
        
        template <>
        double BoundedUntilFormula::getUpperBound(unsigned i) const {
            checkNoVariablesInBound(this->getUpperBound());
            double bound = this->getUpperBound(i).evaluateAsDouble();
            STORM_LOG_THROW(bound >= 0, storm::exceptions::InvalidPropertyException, "Time-bound must not evaluate to negative number.");
            return bound;
        }
        
        template <>
        storm::RationalNumber BoundedUntilFormula::getLowerBound(unsigned i) const {
            checkNoVariablesInBound(this->getLowerBound(i));
            storm::RationalNumber bound = this->getLowerBound(i).evaluateAsRational();
            STORM_LOG_THROW(bound >= storm::utility::zero<storm::RationalNumber>(), storm::exceptions::InvalidPropertyException, "Time-bound must not evaluate to negative number.");
            return bound;
        }
        
        template <>
        storm::RationalNumber BoundedUntilFormula::getUpperBound(unsigned i) const {
            checkNoVariablesInBound(this->getUpperBound(i));
            storm::RationalNumber bound = this->getUpperBound(i).evaluateAsRational();
            STORM_LOG_THROW(bound >= storm::utility::zero<storm::RationalNumber>(), storm::exceptions::InvalidPropertyException, "Time-bound must not evaluate to negative number.");
            return bound;
        }
        
        template <>
        uint64_t BoundedUntilFormula::getLowerBound(unsigned i) const {
            checkNoVariablesInBound(this->getLowerBound(i));
            int_fast64_t bound = this->getLowerBound(i).evaluateAsInt();
            STORM_LOG_THROW(bound >= 0, storm::exceptions::InvalidPropertyException, "Time-bound must not evaluate to negative number.");
            return static_cast<uint64_t>(bound);
        }
        
        template <>
        uint64_t BoundedUntilFormula::getUpperBound(unsigned i) const {
            checkNoVariablesInBound(this->getUpperBound(i));
            int_fast64_t bound = this->getUpperBound(i).evaluateAsInt();
            STORM_LOG_THROW(bound >= 0, storm::exceptions::InvalidPropertyException, "Time-bound must not evaluate to negative number.");
            return static_cast<uint64_t>(bound);
        }
        
        template <>
        double BoundedUntilFormula::getNonStrictUpperBound(unsigned i) const {
            double bound = getUpperBound<double>(i);
            STORM_LOG_THROW(!isUpperBoundStrict(i) || bound > 0, storm::exceptions::InvalidPropertyException, "Cannot retrieve non-strict bound from strict zero-bound.");
            return bound;
        }

        template <>
        uint64_t BoundedUntilFormula::getNonStrictUpperBound(unsigned i) const {
            int_fast64_t bound = getUpperBound<uint64_t>(i);
            if (isUpperBoundStrict(i)) {
                STORM_LOG_THROW(bound > 0, storm::exceptions::InvalidPropertyException, "Cannot retrieve non-strict bound from strict zero-bound.");
                return bound - 1;
            } else {
                return bound;
            }
        }
        
        void BoundedUntilFormula::checkNoVariablesInBound(storm::expressions::Expression const& bound) {
            STORM_LOG_THROW(!bound.containsVariables(), storm::exceptions::InvalidOperationException, "Cannot evaluate time-bound '" << bound << "' as it contains undefined constants.");
        }
        
        std::shared_ptr<BoundedUntilFormula const> BoundedUntilFormula::restrictToDimension(unsigned i) const {
            return std::make_shared<BoundedUntilFormula const>(this->getLeftSubformula().asSharedPointer(), this->getRightSubformula().asSharedPointer(), lowerBound.at(i), upperBound.at(i), this->getTimeBoundReference(i));
        }

        std::ostream& BoundedUntilFormula::writeToStream(std::ostream& out) const {
            this->getLeftSubformula().writeToStream(out);
            
            out << " U";
            if (this->isMultiDimensional()) {
                out << "[";
            }
            for(unsigned i = 0; i < this->getDimension(); ++i) {
                if (i > 0) {
                    out << ",";
                }
                if (this->getTimeBoundReference(i).isRewardBound()) {
                    out << "{\"" << this->getTimeBoundReference(i).getRewardName() << "\"}";
                }
                if (this->hasLowerBound(i)) {
                    if (this->hasUpperBound(i)) {
                        if (this->isLowerBoundStrict(i)) {
                            out << "(";
                        } else {
                            out << "[";
                        }
                        out << this->getLowerBound(i);
                        out << ", ";
                        out << this->getUpperBound(i);
                        if (this->isUpperBoundStrict(i)) {
                            out << ")";
                        } else {
                            out << "]";
                        }
                    } else {
                        if (this->isLowerBoundStrict(i)) {
                            out << ">";
                        } else {
                            out << ">=";
                        }
                        out << getLowerBound(i);
                    }
                } else {
                    if (this->isUpperBoundStrict(i)) {
                        out << "<";
                    } else {
                        out << "<=";
                    }
                    out << this->getUpperBound(i);
                }
                out << " ";
            }
            if (this->isMultiDimensional()) {
                out << "]";
            }

            
            this->getRightSubformula().writeToStream(out);
            return out;
        }
    }
}
