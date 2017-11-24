#include "DFTASFChecker.h"
#include <string>
#include "storm/utility/file.h"
#include "storm/exceptions/NotImplementedException.h"
#include "storm/exceptions/NotSupportedException.h"

namespace storm {
    
    namespace modelchecker {
        
        /*
         * Variable[VarIndex] is the maximum of the others
         */
        class IsMaximum : public DFTConstraint {
        public:
            IsMaximum(uint64_t varIndex, std::vector<uint64_t> const& varIndices) : varIndex(varIndex), varIndices(varIndices) {
            }
            
            virtual ~IsMaximum() {
            }
            
            std::string toSmtlib2(std::vector<std::string> const& varNames) const override {
                std::stringstream sstr;
                sstr << "(and ";
                // assert it is largereq than all values.
                for (auto const& ovi : varIndices) {
                    sstr << "(>= " << varNames.at(varIndex) << " " << varNames.at(ovi) << ") ";
                }
                // assert it is one of the values.
                sstr << "(or ";
                for (auto const& ovi : varIndices) {
                    sstr << "(= " << varNames.at(varIndex) << " " << varNames.at(ovi) << ") ";
                }
                sstr << ")"; // end of the or
                sstr << ")"; // end outer and.
                return sstr.str();
            }

        private:
            uint64_t varIndex;
            std::vector<uint64_t> varIndices;
        };


        /*
         * First is the minimum of the others
         */
        class IsMinimum : public DFTConstraint {
        public:
            IsMinimum(uint64_t varIndex, std::vector<uint64_t> const& varIndices) : varIndex(varIndex), varIndices(varIndices) {
            }

            virtual ~IsMinimum() {
            }

            std::string toSmtlib2(std::vector<std::string> const& varNames) const override {
                std::stringstream sstr;
                sstr << "(and ";
                // assert it is smallereq than all values.
                for (auto const& ovi : varIndices) {
                    sstr << "(<= " << varNames.at(varIndex) << " " << varNames.at(ovi) << ") ";
                }
                // assert it is one of the values.
                sstr << "(or ";
                for (auto const& ovi : varIndices) {
                    sstr << "(= " << varNames.at(varIndex) << " " << varNames.at(ovi) << ") ";
                }
                sstr << ")"; // end of the or
                sstr << ")"; // end outer and.
                return sstr.str();
            }

        private:
            uint64_t varIndex;
            std::vector<uint64_t> varIndices;
        };
                

        class BetweenValues : public DFTConstraint {
        public:
            BetweenValues(uint64_t varIndex, uint64_t lower, uint64_t upper) : varIndex(varIndex), upperBound(upper) , lowerBound(lower) {
            }
            virtual ~BetweenValues() {
            }

            std::string toSmtlib2(std::vector<std::string> const& varNames) const override {
                std::stringstream sstr;
                sstr << "(and ";
                sstr << "(>= " << varNames.at(varIndex) << " " << lowerBound << ")";
                sstr << "(<= " << varNames.at(varIndex) << " " << upperBound << ")";
                sstr << ")";
                return sstr.str();
            }

        private:
            uint64_t varIndex;
            uint64_t upperBound;
            uint64_t lowerBound;
        };


        class And : public DFTConstraint {
        public:
            And(std::vector<std::shared_ptr<DFTConstraint>> const& constraints) : constraints(constraints) {}
            virtual ~And() {
            }

            std::string toSmtlib2(std::vector<std::string> const& varNames) const override {
                std::stringstream sstr;
                sstr << "(and";
                for(auto const& c : constraints) {
                    sstr << " " << c->toSmtlib2(varNames);
                }
                sstr << ")";
                return sstr.str();
            }

        private:
            std::vector<std::shared_ptr<DFTConstraint>> constraints;

        };


        class Implies : public DFTConstraint {
        public:
            Implies(std::shared_ptr<DFTConstraint> l, std::shared_ptr<DFTConstraint> r) : lhs(l), rhs(r) {
            }

            std::string toSmtlib2(std::vector<std::string> const& varNames) const override {
                std::stringstream sstr;
                sstr << "(=> " << lhs->toSmtlib2(varNames) << " " << rhs->toSmtlib2(varNames) << ")";
                return sstr.str();
            }

        private:
            std::shared_ptr<DFTConstraint> lhs;
            std::shared_ptr<DFTConstraint> rhs;
        };


        class Iff : public DFTConstraint {
        public:
            Iff(std::shared_ptr<DFTConstraint> l, std::shared_ptr<DFTConstraint> r) : lhs(l), rhs(r) {
            }

            std::string toSmtlib2(std::vector<std::string> const& varNames) const override {
                std::stringstream sstr;
                sstr << "(= " << lhs->toSmtlib2(varNames) << " " << rhs->toSmtlib2(varNames) << ")";
                return sstr.str();
            }

        private:
            std::shared_ptr<DFTConstraint> lhs;
            std::shared_ptr<DFTConstraint> rhs;
        };


        class IsConstantValue : public DFTConstraint {
        public:
            IsConstantValue(uint64_t varIndex, uint64_t val) : varIndex(varIndex), value(val) {
            }

            virtual ~IsConstantValue() {
            }

            std::string toSmtlib2(std::vector<std::string> const& varNames) const override {
                std::stringstream sstr;
                assert(varIndex < varNames.size());
                sstr << "(= " << varNames.at(varIndex) << " " << value << ")";
                return sstr.str();
            }

        private:
            uint64_t varIndex;
            uint64_t value;
        };


        class IsLessConstant : public DFTConstraint {
        public:
            IsLessConstant(uint64_t varIndex, uint64_t val) :varIndex(varIndex), value(val) {
            }

            virtual ~IsLessConstant() {
            }

            std::string toSmtlib2(std::vector<std::string> const& varNames) const override {
                std::stringstream sstr;
                assert(varIndex < varNames.size());
                sstr << "(< " << varNames.at(varIndex) << " " << value << ")";
                return sstr.str();
            }

        private:
            uint64_t varIndex;
            uint64_t value;
        };


        class IsEqual : public DFTConstraint {
        public:
            IsEqual(uint64_t varIndex1, uint64_t varIndex2) :var1Index(varIndex1), var2Index(varIndex2) {
            }

            virtual ~IsEqual() {
            }

            std::string toSmtlib2(std::vector<std::string> const& varNames) const override {
                return "(= " + varNames.at(var1Index) + " " +  varNames.at(var2Index) + ")";
            }

        private:
            uint64_t var1Index;
            uint64_t var2Index;
        };


        class IsLess : public DFTConstraint {
        public:
            IsLess(uint64_t varIndex1, uint64_t varIndex2) :var1Index(varIndex1), var2Index(varIndex2) {
            }

            virtual ~IsLess() {
            }

            std::string toSmtlib2(std::vector<std::string> const& varNames) const override {
                return "(< " + varNames.at(var1Index) + " " +  varNames.at(var2Index) + ")";
            }

        private:
            uint64_t var1Index;
            uint64_t var2Index;
        };


        class PairwiseDifferent : public DFTConstraint {
        public:
            PairwiseDifferent(std::vector<uint64_t> const& indices) : varIndices(indices) {
            }
            virtual ~PairwiseDifferent() {
            }

            std::string toSmtlib2(std::vector<std::string> const& varNames) const override {
                std::stringstream sstr;
                sstr << "(distinct";
                //                for(uint64_t i = 0; i < varIndices.size(); ++i) {
                //                    for(uint64_t j = i + 1; j < varIndices.size(); ++j) {
                //                        sstr << "()";
                //                    }
                //                }
                for (auto const& varIndex : varIndices) {
                    sstr << " " << varNames.at(varIndex);
                }
                sstr << ")";
                return sstr.str();
            }

        private:
            std::vector<uint64_t> varIndices;
        };


        class Sorted : public DFTConstraint {
        public:
            Sorted(std::vector<uint64_t> varIndices) : varIndices(varIndices) {
            }

            virtual ~Sorted() {
            }

            std::string toSmtlib2(std::vector<std::string> const& varNames) const override {
                std::stringstream sstr;
                sstr << "(and ";
                for(uint64_t i = 1; i < varIndices.size(); ++i) {
                    sstr << "(<= " << varNames.at(varIndices.at(i-1)) << " " << varNames.at(varIndices.at(i)) << ")";
                }
                sstr << ") ";
                return sstr.str();
            }

        private:
            std::vector<uint64_t> varIndices;
        };


        class IfThenElse : public DFTConstraint {
        public:
            IfThenElse(std::shared_ptr<DFTConstraint> ifC, std::shared_ptr<DFTConstraint> thenC, std::shared_ptr<DFTConstraint> elseC) : ifConstraint(ifC), thenConstraint(thenC), elseConstraint(elseC) {
            }

            std::string toSmtlib2(std::vector<std::string> const& varNames) const override {
                std::stringstream sstr;
                sstr << "(ite " << ifConstraint->toSmtlib2(varNames) << " " << thenConstraint->toSmtlib2(varNames) << " " << elseConstraint->toSmtlib2(varNames) << ")";
                return sstr.str();
            }

        private:
            std::shared_ptr<DFTConstraint> ifConstraint;
            std::shared_ptr<DFTConstraint> thenConstraint;
            std::shared_ptr<DFTConstraint> elseConstraint;
        };

                
        DFTASFChecker::DFTASFChecker(storm::storage::DFT<double> const& dft) : dft(dft) {
            // Intentionally left empty.
        }

        uint64_t DFTASFChecker::getClaimVariableIndex(uint64_t spare, uint64_t child) const {
            return claimVariables.at(SpareAndChildPair(spare, child));
        }

        void DFTASFChecker::convert() {
            std::vector<uint64_t> beVariables;
            notFailed = dft.nrBasicElements()+1; // Value indicating the element is not failed

            // Initialize variables
            for (size_t i = 0; i < dft.nrElements(); ++i) {
                std::shared_ptr<storm::storage::DFTElement<ValueType> const> element = dft.getElement(i);
                varNames.push_back("t_" + element->name());
                timePointVariables.emplace(i, varNames.size() - 1);
                switch (element->type()) {
                    case storm::storage::DFTElementType::BE:
                        beVariables.push_back(varNames.size() - 1);
                        break;
                    case storm::storage::DFTElementType::SPARE:
                    {
                        auto spare = std::static_pointer_cast<storm::storage::DFTSpare<double> const>(element);
                        for( auto const& spareChild : spare->children()) {
                            varNames.push_back("c_" + element->name() + "_" + spareChild->name());
                            claimVariables.emplace(SpareAndChildPair(element->id(), spareChild->id()), varNames.size() - 1);
                        }
                        break;
                    }
                    default:
                        break;
                }
            }


            // Generate constraints

            // All BEs have to fail (first part of constraint 12)
            for (auto const& beV : beVariables) {
                constraints.push_back(std::make_shared<BetweenValues>(beV, 1, dft.nrBasicElements()));
            }

            // No two BEs fail at the same time (second part of constraint 12)
            constraints.push_back(std::make_shared<PairwiseDifferent>(beVariables));
            constraints.back()->setDescription("No two BEs fail at the same time");

            // Initialize claim variables in [1, |BE|+1]
            for (auto const& claimVariable : claimVariables) {
                constraints.push_back(std::make_shared<BetweenValues>(claimVariable.second, 0, notFailed));
            }

            // Encoding for gates
            for (size_t i = 0; i < dft.nrElements(); ++i) {
                std::shared_ptr<storm::storage::DFTElement<ValueType> const> element = dft.getElement(i);
                STORM_LOG_ASSERT(i == element->id(), "Id and index should match.");

                // Get indices for gate children
                std::vector<uint64_t> childVarIndices;
                if (element->isGate()) {
                    std::shared_ptr<storm::storage::DFTGate<ValueType> const> gate = dft.getGate(i);
                    for (auto const& child : gate->children()) {
                        childVarIndices.push_back(timePointVariables.at(child->id()));
                    }
                }

                switch (element->type()) {
                    case storm::storage::DFTElementType::BE:
                        // BEs were already considered before
                        break;
                    case storm::storage::DFTElementType::AND:
                        // Constraint for AND gate (constraint 1)
                        constraints.push_back(std::make_shared<IsMaximum>(timePointVariables.at(i), childVarIndices));
                        constraints.back()->setDescription("AND gate");
                        break;
                    case storm::storage::DFTElementType::OR:
                        // Constraint for OR gate (constraint 2)
                        constraints.push_back(std::make_shared<IsMinimum>(timePointVariables.at(i), childVarIndices));
                        constraints.back()->setDescription("OR gate");
                        break;
                    case storm::storage::DFTElementType::PAND:
                    {
                        // Constraint for PAND gate (constraint 3)
                        std::shared_ptr<DFTConstraint> ifC = std::make_shared<Sorted>(childVarIndices);
                        std::shared_ptr<DFTConstraint> thenC = std::make_shared<IsEqual>(timePointVariables.at(i), childVarIndices.back());
                        std::shared_ptr<DFTConstraint> elseC = std::make_shared<IsConstantValue>(timePointVariables.at(i), notFailed);
                        constraints.push_back(std::make_shared<IfThenElse>(ifC, thenC, elseC));
                        constraints.back()->setDescription("PAND gate");
                        break;
                    }
                    case storm::storage::DFTElementType::SEQ:
                        STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "SMT encoding of SEQs is not implemented yet.");
                        break;
                    case storm::storage::DFTElementType::SPARE:
                    {
                        auto spare = std::static_pointer_cast<storm::storage::DFTSpare<double> const>(element);
                        auto const& children = spare->children();
                        uint64_t firstChild = children.front()->id();
                        uint64_t lastChild = children.back()->id();

                        // First child of each spare is claimed in the beginning
                        constraints.push_back(std::make_shared<IsConstantValue>(getClaimVariableIndex(spare->id(), firstChild), 0));
                        constraints.back()->setDescription("SPARE " + spare->name() + " claims first child");

                        // If last child is claimed before failure, then the spare fails when the last child fails (constraint 5)
                        std::shared_ptr<DFTConstraint> leftC = std::make_shared<IsLess>(getClaimVariableIndex(spare->id(), lastChild), childVarIndices.back());
                        constraints.push_back(std::make_shared<Implies>(leftC, std::make_shared<IsEqual>(timePointVariables.at(i), childVarIndices.back())));
                        constraints.back()->setDescription("Last child & claimed -> SPARE fails");

                        // Construct constraint for trying to claim next child
                        STORM_LOG_ASSERT(children.size() >= 2, "Spare has only one child");
                        for (uint64_t currChild = 0; currChild < children.size() - 1; ++currChild) {
                            uint64_t timeCurrChild = childVarIndices.at(currChild); // Moment when current child fails

                            // If i-th child fails after being claimed, then try to claim next child (constraint 6)
                            std::shared_ptr<DFTConstraint> tryClaimC = generateTryToClaimConstraint(spare, currChild + 1, timeCurrChild);
                            constraints.push_back(std::make_shared<Iff>(std::make_shared<IsLess>(getClaimVariableIndex(spare->id(), children.at(currChild)->id()), timeCurrChild), tryClaimC));
                            constraints.back()->setDescription("Try to claim " + std::to_string(currChild+2) + "th child");
                        }
                        break;
                    }
                    case storm::storage::DFTElementType::PDEP:
                        STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "SMT encoding of FDEPs and PDEPs is not implemented yet.");
                        break;
                    default:
                        STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "SMT encoding for type '" << element->type() << "' is not supported.");
                        break;
                }
            }

            // Only one spare can claim a child (constraint 8)
            for (size_t i = 0; i < dft.nrElements(); ++i) {
                std::shared_ptr<storm::storage::DFTElement<ValueType> const> element = dft.getElement(i);
                if (element->isSpareGate()) {
                    auto spare = std::static_pointer_cast<storm::storage::DFTSpare<double> const>(element);
                    for (auto const& child : spare->children()) {
                        // No other spare claims this child
                        std::vector<std::shared_ptr<DFTConstraint>> noOtherClaims;
                        for (auto const& otherSpare : child->parents()) {
                            if (otherSpare->id() == spare->id()) {
                                // original spare
                                continue;
                            }
                            noOtherClaims.push_back(std::make_shared<IsConstantValue>(getClaimVariableIndex(otherSpare->id(), child->id()), notFailed));
                        }
                        if (!noOtherClaims.empty()) {
                            std::shared_ptr<DFTConstraint> leftC = std::make_shared<IsLessConstant>(getClaimVariableIndex(spare->id(), child->id()), notFailed);
                            constraints.push_back(std::make_shared<Implies>(leftC, std::make_shared<And>(noOtherClaims)));
                            constraints.back()->setDescription("Only one spare claims the child " + child->name());
                        }
                    }
                }
            }

            // Toplevel element will not fail (part of constraint 13)
            constraints.push_back(std::make_shared<IsConstantValue>(timePointVariables.at(dft.getTopLevelIndex()), notFailed));
            constraints.back()->setDescription("Toplevel element should not fail");
        }

        std::shared_ptr<DFTConstraint> DFTASFChecker::generateTryToClaimConstraint(std::shared_ptr<storm::storage::DFTSpare<ValueType> const> spare, uint64_t childIndex, uint64_t timepoint) const {
            auto child = spare->children().at(childIndex);
            uint64_t timeChild = timePointVariables.at(child->id()); // Moment when the child fails
            uint64_t claimChild = getClaimVariableIndex(spare->id(), child->id()); // Moment the spare claims the child

            std::vector<std::shared_ptr<DFTConstraint>> noClaimingPossible;
            // Child cannot be claimed.
            noClaimingPossible.push_back(std::make_shared<IsEqual>(claimChild, notFailed));
            if (childIndex + 1 < spare->children().size()) {
                // Consider next child for claiming (second case in constraint 7)
                noClaimingPossible.push_back(generateTryToClaimConstraint(spare, childIndex+1, timepoint));
            } else {
                // Last child: spare fails at same point as this child (third case in constraint 7)
                noClaimingPossible.push_back(std::make_shared<IsEqual>(timePointVariables.at(spare->id()), timepoint));
            }
            std::shared_ptr<DFTConstraint> elseCaseC = std::make_shared<And>(noClaimingPossible);

            // Check if next child is availble (first case in constraint 7)
            std::vector<std::shared_ptr<DFTConstraint>> claimingPossibleC;
            // Next child is not yet failed
            claimingPossibleC.push_back(std::make_shared<IsLess>(timepoint, timeChild));
            // Child is not yet claimed by a different spare
            for (auto const& otherSpare : child->parents()) {
                if (otherSpare->id() == spare->id()) {
                    // not a different spare.
                    continue;
                }
                claimingPossibleC.push_back(std::make_shared<IsConstantValue>(getClaimVariableIndex(otherSpare->id(), child->id()), notFailed));
            }

            // Claim child if available
            std::shared_ptr<DFTConstraint> firstCaseC = std::make_shared<IfThenElse>(std::make_shared<And>(claimingPossibleC), std::make_shared<IsEqual>(claimChild, timepoint), elseCaseC);
            return firstCaseC;
        }


        void DFTASFChecker::toFile(std::string const& filename) {
            std::ofstream stream;
            storm::utility::openFile(filename, stream);
            stream << "; time point variables" << std::endl;
            for (auto const& timeVarEntry : timePointVariables) {
                stream << "(declare-fun " << varNames[timeVarEntry.second] << "()  Int)" << std::endl;
            }
            stream << "; claim variables" << std::endl;
            for (auto const& claimVarEntry : claimVariables) {
                stream << "(declare-fun " << varNames[claimVarEntry.second] << "() Int)" << std::endl;
            }
            for (auto const& constraint : constraints) {
                if (!constraint->description().empty()) {
                    stream << "; " << constraint->description() << std::endl;
                }
                stream << "(assert " << constraint->toSmtlib2(varNames) << ")" << std::endl;
            }
            stream << "(check-sat)" << std::endl;
            storm::utility::closeFile(stream);
        }
    }
}
