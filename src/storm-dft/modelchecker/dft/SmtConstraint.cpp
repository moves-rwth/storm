#include "DFTASFChecker.h"
#include <string>

namespace storm {

    namespace modelchecker {

        /*
         * Variable[VarIndex] is the maximum of the others
         */
        class IsMaximum : public SmtConstraint {
        public:
            IsMaximum(uint64_t varIndex, std::vector<uint64_t> const &varIndices) : varIndex(varIndex),
                                                                                    varIndices(varIndices) {
            }

            virtual ~IsMaximum() {
            }

            std::string toSmtlib2(std::vector<std::string> const &varNames) const override {
                std::stringstream sstr;
                sstr << "(and ";
                // assert it is largereq than all values.
                for (auto const &ovi : varIndices) {
                    sstr << "(>= " << varNames.at(varIndex) << " " << varNames.at(ovi) << ") ";
                }
                // assert it is one of the values.
                sstr << "(or ";
                for (auto const &ovi : varIndices) {
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
        class IsMinimum : public SmtConstraint {
        public:
            IsMinimum(uint64_t varIndex, std::vector<uint64_t> const &varIndices) : varIndex(varIndex),
                                                                                    varIndices(varIndices) {
            }

            virtual ~IsMinimum() {
            }

            std::string toSmtlib2(std::vector<std::string> const &varNames) const override {
                std::stringstream sstr;
                sstr << "(and ";
                // assert it is smallereq than all values.
                for (auto const &ovi : varIndices) {
                    sstr << "(<= " << varNames.at(varIndex) << " " << varNames.at(ovi) << ") ";
                }
                // assert it is one of the values.
                sstr << "(or ";
                for (auto const &ovi : varIndices) {
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


        class BetweenValues : public SmtConstraint {
        public:
            BetweenValues(uint64_t varIndex, uint64_t lower, uint64_t upper) : varIndex(varIndex), upperBound(upper),
                                                                               lowerBound(lower) {
            }

            virtual ~BetweenValues() {
            }

            std::string toSmtlib2(std::vector<std::string> const &varNames) const override {
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


        class And : public SmtConstraint {
        public:
            And(std::vector<std::shared_ptr<SmtConstraint>> const &constraints) : constraints(constraints) {
            }

            virtual ~And() {
            }

            std::string toSmtlib2(std::vector<std::string> const &varNames) const override {
                std::stringstream sstr;
                if (constraints.empty()) {
                    sstr << "true";
                } else {
                    sstr << "(and";
                    for (auto const &c : constraints) {
                        sstr << " " << c->toSmtlib2(varNames);
                    }
                    sstr << ")";
                }
                return sstr.str();
            }

        private:
            std::vector<std::shared_ptr<SmtConstraint>> constraints;

        };


        class Or : public SmtConstraint {
        public:
            Or(std::vector<std::shared_ptr<SmtConstraint>> const &constraints) : constraints(constraints) {
            }

            virtual ~Or() {
            }

            std::string toSmtlib2(std::vector<std::string> const &varNames) const override {
                std::stringstream sstr;
                if (constraints.empty()) {
                    sstr << "false";
                } else {
                    sstr << "(or";
                    for (auto const &c : constraints) {
                        sstr << " " << c->toSmtlib2(varNames);
                    }
                    sstr << ")";
                }
                return sstr.str();
            }

        private:
            std::vector<std::shared_ptr<SmtConstraint>> constraints;

        };


        class Implies : public SmtConstraint {
        public:
            Implies(std::shared_ptr<SmtConstraint> l, std::shared_ptr<SmtConstraint> r) : lhs(l), rhs(r) {
            }

            std::string toSmtlib2(std::vector<std::string> const &varNames) const override {
                std::stringstream sstr;
                sstr << "(=> " << lhs->toSmtlib2(varNames) << " " << rhs->toSmtlib2(varNames) << ")";
                return sstr.str();
            }

        private:
            std::shared_ptr<SmtConstraint> lhs;
            std::shared_ptr<SmtConstraint> rhs;
        };


        class Iff : public SmtConstraint {
        public:
            Iff(std::shared_ptr<SmtConstraint> l, std::shared_ptr<SmtConstraint> r) : lhs(l), rhs(r) {
            }

            std::string toSmtlib2(std::vector<std::string> const &varNames) const override {
                std::stringstream sstr;
                sstr << "(= " << lhs->toSmtlib2(varNames) << " " << rhs->toSmtlib2(varNames) << ")";
                return sstr.str();
            }

        private:
            std::shared_ptr<SmtConstraint> lhs;
            std::shared_ptr<SmtConstraint> rhs;
        };


        class IsTrue : public SmtConstraint {
        public:
            IsTrue(bool val) : value(val) {
            }

            virtual ~IsTrue() {
            }

            std::string toSmtlib2(std::vector<std::string> const &varNames) const override {
                std::stringstream sstr;
                sstr << (value ? "true" : "false");
                return sstr.str();
            }

        private:
            bool value;
        };


        class IsBoolValue : public SmtConstraint {
        public:
            IsBoolValue(uint64_t varIndex, bool val) : varIndex(varIndex), value(val) {
            }

            virtual ~IsBoolValue() {
            }

            std::string toSmtlib2(std::vector<std::string> const &varNames) const override {
                std::stringstream sstr;
                assert(varIndex < varNames.size());
                if (value) {
                    sstr << varNames.at(varIndex);
                } else {
                    sstr << "(not " << varNames.at(varIndex) << ")";
                }
                return sstr.str();
            }

        private:
            uint64_t varIndex;
            bool value;
        };


        class IsConstantValue : public SmtConstraint {
        public:
            IsConstantValue(uint64_t varIndex, uint64_t val) : varIndex(varIndex), value(val) {
            }

            virtual ~IsConstantValue() {
            }

            std::string toSmtlib2(std::vector<std::string> const &varNames) const override {
                std::stringstream sstr;
                assert(varIndex < varNames.size());
                sstr << "(= " << varNames.at(varIndex) << " " << value << ")";
                return sstr.str();
            }

        private:
            uint64_t varIndex;
            uint64_t value;
        };


        class IsLessConstant : public SmtConstraint {
        public:
            IsLessConstant(uint64_t varIndex, uint64_t val) : varIndex(varIndex), value(val) {
            }

            virtual ~IsLessConstant() {
            }

            std::string toSmtlib2(std::vector<std::string> const &varNames) const override {
                std::stringstream sstr;
                assert(varIndex < varNames.size());
                sstr << "(< " << varNames.at(varIndex) << " " << value << ")";
                return sstr.str();
            }

        private:
            uint64_t varIndex;
            uint64_t value;
        };

        class IsLessEqualConstant : public SmtConstraint {
        public:
            IsLessEqualConstant(uint64_t varIndex, uint64_t val) : varIndex(varIndex), value(val) {
            }

            virtual ~IsLessEqualConstant() {
            }

            std::string toSmtlib2(std::vector<std::string> const &varNames) const override {
                std::stringstream sstr;
                assert(varIndex < varNames.size());
                sstr << "(<= " << varNames.at(varIndex) << " " << value << ")";
                return sstr.str();
            }

        private:
            uint64_t varIndex;
            uint64_t value;
        };


        class IsEqual : public SmtConstraint {
        public:
            IsEqual(uint64_t varIndex1, uint64_t varIndex2) : var1Index(varIndex1), var2Index(varIndex2) {
            }

            virtual ~IsEqual() {
            }

            std::string toSmtlib2(std::vector<std::string> const &varNames) const override {
                return "(= " + varNames.at(var1Index) + " " + varNames.at(var2Index) + ")";
            }

        private:
            uint64_t var1Index;
            uint64_t var2Index;
        };


        class IsLess : public SmtConstraint {
        public:
            IsLess(uint64_t varIndex1, uint64_t varIndex2) : var1Index(varIndex1), var2Index(varIndex2) {
            }

            virtual ~IsLess() {
            }

            std::string toSmtlib2(std::vector<std::string> const &varNames) const override {
                return "(< " + varNames.at(var1Index) + " " + varNames.at(var2Index) + ")";
            }

        private:
            uint64_t var1Index;
            uint64_t var2Index;
        };


        class PairwiseDifferent : public SmtConstraint {
        public:
            PairwiseDifferent(std::vector<uint64_t> const &indices) : varIndices(indices) {
            }

            virtual ~PairwiseDifferent() {
            }

            std::string toSmtlib2(std::vector<std::string> const &varNames) const override {
                std::stringstream sstr;
                sstr << "(distinct";
                //                for(uint64_t i = 0; i < varIndices.size(); ++i) {
                //                    for(uint64_t j = i + 1; j < varIndices.size(); ++j) {
                //                        sstr << "()";
                //                    }
                //                }
                for (auto const &varIndex : varIndices) {
                    sstr << " " << varNames.at(varIndex);
                }
                sstr << ")";
                return sstr.str();
            }

        private:
            std::vector<uint64_t> varIndices;
        };


        class Sorted : public SmtConstraint {
        public:
            Sorted(std::vector<uint64_t> varIndices) : varIndices(varIndices) {
            }

            virtual ~Sorted() {
            }

            std::string toSmtlib2(std::vector<std::string> const &varNames) const override {
                std::stringstream sstr;
                sstr << "(and ";
                for (uint64_t i = 1; i < varIndices.size(); ++i) {
                    sstr << "(<= " << varNames.at(varIndices.at(i - 1)) << " " << varNames.at(varIndices.at(i)) << ")";
                }
                sstr << ") ";
                return sstr.str();
            }

        private:
            std::vector<uint64_t> varIndices;
        };


        class IfThenElse : public SmtConstraint {
        public:
            IfThenElse(std::shared_ptr<SmtConstraint> ifC, std::shared_ptr<SmtConstraint> thenC,
                       std::shared_ptr<SmtConstraint> elseC) : ifConstraint(ifC), thenConstraint(thenC),
                                                               elseConstraint(elseC) {
            }

            std::string toSmtlib2(std::vector<std::string> const &varNames) const override {
                std::stringstream sstr;
                sstr << "(ite " << ifConstraint->toSmtlib2(varNames) << " " << thenConstraint->toSmtlib2(varNames)
                     << " " << elseConstraint->toSmtlib2(varNames) << ")";
                return sstr.str();
            }

        private:
            std::shared_ptr<SmtConstraint> ifConstraint;
            std::shared_ptr<SmtConstraint> thenConstraint;
            std::shared_ptr<SmtConstraint> elseConstraint;
        };
    }
}

