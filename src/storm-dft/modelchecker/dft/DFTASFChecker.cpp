#include "DFTASFChecker.h"
#include <string>
#include "storm/utility/file.h"

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


        class IsEqual : public DFTConstraint {
        public:
            IsEqual(uint64_t varIndex1, uint64_t varIndex2) :var1Index(varIndex1), var2Index(varIndex2) {
            }

            virtual ~IsEqual() {
            }

            std::string toSmtlib2(std::vector<std::string> const& varNames) const override {
                return "(= " + varNames.at(var1Index) + " " +  varNames.at(var2Index) + " )";
            }

        private:
            uint64_t var1Index;
            uint64_t var2Index;
        };


        class IsLEqual : public DFTConstraint {
        public:
            IsLEqual(uint64_t varIndex1, uint64_t varIndex2) :var1Index(varIndex1), var2Index(varIndex2) {
            }

            virtual ~IsLEqual() {
            }

            std::string toSmtlib2(std::vector<std::string> const& varNames) const override {
                return "(<= " + varNames.at(var1Index) + " " +  varNames.at(var2Index) + " )";
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
            // Convert all elements
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

            // BE
            constraints.push_back(std::make_shared<PairwiseDifferent>(beVariables));
            constraints.back()->setDescription("No two BEs fail at the same time");
            for (auto const& beV : beVariables) {
                constraints.push_back(std::make_shared<BetweenValues>(beV, 1, dft.nrBasicElements()));
            }

            // Claim variables
            for (auto const& csvV : claimVariables) {
                constraints.push_back(std::make_shared<BetweenValues>(csvV.second, 0, dft.nrBasicElements() + 1));
            }

            for (size_t i = 0; i < dft.nrElements(); ++i) {
                std::shared_ptr<storm::storage::DFTElement<ValueType> const> element = dft.getElement(i);
                if(element->isSpareGate()) {
                    auto spare = std::static_pointer_cast<storm::storage::DFTSpare<double> const>(element);
                    auto const& spareChild = spare->children().front();
                    constraints.push_back(std::make_shared<IsConstantValue>(getClaimVariableIndex(spare->id(), spareChild->id()), 0));
                    constraints.back()->setDescription("Spare " + spare->name() + " claims first child");

                }
            }

            for (size_t i = 0; i < dft.nrElements(); ++i) {
                std::vector<uint64_t> childVarIndices;
                if (dft.isGate(i)) {
                    std::shared_ptr<storm::storage::DFTGate<ValueType> const> gate = dft.getGate(i);
                    for (auto const& child : gate->children()) {
                        childVarIndices.push_back(timePointVariables.at(child->id()));
                    }
                }

                std::shared_ptr<storm::storage::DFTElement<ValueType> const> element = dft.getElement(i);
                switch (element->type()) {
                    case storm::storage::DFTElementType::AND:
                        constraints.push_back(std::make_shared<IsMaximum>(timePointVariables.at(i), childVarIndices));
                        constraints.back()->setDescription("And gate");
                        break;
                    case storm::storage::DFTElementType::OR:
                        constraints.push_back(std::make_shared<IsMinimum>(timePointVariables.at(i), childVarIndices));
                        constraints.back()->setDescription("Or gate");
                        break;
                    case storm::storage::DFTElementType::PAND:
                        constraints.push_back(std::make_shared<IfThenElse>(std::make_shared<Sorted>(childVarIndices), std::make_shared<IsEqual>(timePointVariables.at(i), timePointVariables.at(childVarIndices.back())), std::make_shared<IsConstantValue>(timePointVariables.at(i), dft.nrBasicElements()+1)));
                        constraints.back()->setDescription("Pand gate");
                    case storm::storage::DFTElementType::SPARE:
                    {
                        auto spare = std::static_pointer_cast<storm::storage::DFTSpare<double> const>(element);
                        auto const& children = spare->children();

                        constraints.push_back(std::make_shared<Iff>(std::make_shared<IsLEqual>(getClaimVariableIndex(spare->id(), children.back()->id()), childVarIndices.back()), std::make_shared<IsEqual>(timePointVariables.at(i), childVarIndices.back())));
                        constraints.back()->setDescription("Last child & claimed -> spare fails");

                        std::shared_ptr<DFTConstraint> elseCase = nullptr;
                        for (uint64_t j = 0; j < children.size(); ++j) {
                            uint64_t currChild = children.size() - 2 - j;
                            std::vector<std::shared_ptr<DFTConstraint>> ifcs;
                            uint64_t xv = childVarIndices.at(currChild); // if v is the child, xv is the time x fails.
                            uint64_t csn = getClaimVariableIndex(spare->id(), childVarIndices.at(currChild+1)); // csn is the moment the spare claims the next child
                            uint64_t xn = childVarIndices.at(currChild+1); // xn is the moment the next child fails
                            if (j == 0) {
                                elseCase = std::make_shared<IsEqual>(timePointVariables.at(i),  xv);
                            }
                            ifcs.push_back(std::make_shared<IsLEqual>(xv, xn));
                            for (auto const& otherSpare : children.at(currChild+1)->parents()) {
                                if (otherSpare->id() == i) { continue; }// not a OTHER spare.
                                ifcs.push_back(std::make_shared<IsConstantValue>(getClaimVariableIndex(otherSpare->id(), children.at(currChild+1)->id()), dft.nrBasicElements()+1));
                            }
                            std::shared_ptr<DFTConstraint> ite = std::make_shared<IfThenElse>(std::make_shared<And>(ifcs), std::make_shared<IsEqual>(csn, xv), elseCase);
                            elseCase = ite;
                            constraints.push_back(std::make_shared<Iff>(std::make_shared<IsLEqual>(getClaimVariableIndex(spare->id(), children.at(currChild)->id()), xv), ite));
                            constraints.back()->setDescription(std::to_string(j+1) + " but last child & claimed");
                        }
                        break;
                    }
                    default:
                        break;
                }
            }
            constraints.push_back(std::make_shared<IsConstantValue>(dft.getTopLevelIndex(), dft.nrBasicElements()+1));
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
                stream << "; " << constraint->description() << std::endl;
                stream << "(assert " << constraint->toSmtlib2(varNames) << ")" << std::endl;
            }
            stream << "(check-sat)" << std::endl;
            storm::utility::closeFile(stream);
        }
    }
}
