#include <storm/storage/expressions/ExpressionManager.h>
#include <string>
#include "DFTASFChecker.h"

namespace storm::dft {
namespace modelchecker {

/*
 * Variable[VarIndex] is the maximum of the others
 */
class IsMaximum : public SmtConstraint {
   public:
    IsMaximum(uint64_t varIndex, std::vector<uint64_t> const &varIndices) : varIndex(varIndex), varIndices(varIndices) {}

    virtual ~IsMaximum() {}

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
        sstr << ")";  // end of the or
        sstr << ")";  // end outer and.
        return sstr.str();
    }

    storm::expressions::Expression toExpression(std::vector<std::string> const &varNames,
                                                std::shared_ptr<storm::expressions::ExpressionManager> manager) const override {
        std::vector<storm::expressions::Expression> outerAnd;
        std::vector<storm::expressions::Expression> innerOr;
        for (auto const &ovi : varIndices) {
            outerAnd.push_back((manager->getVariableExpression(varNames.at(varIndex)) >= manager->getVariableExpression(varNames.at(ovi))));
            innerOr.push_back((manager->getVariableExpression(varNames.at(varIndex)) == manager->getVariableExpression(varNames.at(ovi))));
        }
        outerAnd.push_back(disjunction(innerOr));
        return conjunction(outerAnd);
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
    IsMinimum(uint64_t varIndex, std::vector<uint64_t> const &varIndices) : varIndex(varIndex), varIndices(varIndices) {}

    virtual ~IsMinimum() {}

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
        sstr << ")";  // end of the or
        sstr << ")";  // end outer and.
        return sstr.str();
    }

    storm::expressions::Expression toExpression(std::vector<std::string> const &varNames,
                                                std::shared_ptr<storm::expressions::ExpressionManager> manager) const override {
        std::vector<storm::expressions::Expression> outerAnd;
        std::vector<storm::expressions::Expression> innerOr;
        for (auto const &ovi : varIndices) {
            outerAnd.push_back((manager->getVariableExpression(varNames.at(varIndex)) <= manager->getVariableExpression(varNames.at(ovi))));
            innerOr.push_back((manager->getVariableExpression(varNames.at(varIndex)) == manager->getVariableExpression(varNames.at(ovi))));
        }
        outerAnd.push_back(disjunction(innerOr));
        return conjunction(outerAnd);
    }

   private:
    uint64_t varIndex;
    std::vector<uint64_t> varIndices;
};

class BetweenValues : public SmtConstraint {
   public:
    BetweenValues(uint64_t varIndex, uint64_t lower, uint64_t upper) : varIndex(varIndex), upperBound(upper), lowerBound(lower) {}

    virtual ~BetweenValues() {}

    std::string toSmtlib2(std::vector<std::string> const &varNames) const override {
        std::stringstream sstr;
        sstr << "(and ";
        sstr << "(>= " << varNames.at(varIndex) << " " << lowerBound << ")";
        sstr << "(<= " << varNames.at(varIndex) << " " << upperBound << ")";
        sstr << ")";
        return sstr.str();
    }

    storm::expressions::Expression toExpression(std::vector<std::string> const &varNames,
                                                std::shared_ptr<storm::expressions::ExpressionManager> manager) const override {
        return (manager->getVariableExpression(varNames.at(varIndex)) >= lowerBound) && (manager->getVariableExpression(varNames.at(varIndex)) <= upperBound);
    }

   private:
    uint64_t varIndex;
    uint64_t upperBound;
    uint64_t lowerBound;
};

class And : public SmtConstraint {
   public:
    And(std::vector<std::shared_ptr<SmtConstraint>> const &constraints) : constraints(constraints) {}

    virtual ~And() {}

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

    storm::expressions::Expression toExpression(std::vector<std::string> const &varNames,
                                                std::shared_ptr<storm::expressions::ExpressionManager> manager) const override {
        if (constraints.empty()) {
            return manager->boolean(true);
        } else {
            std::vector<storm::expressions::Expression> conjuncts;
            for (auto const &c : constraints) {
                conjuncts.push_back(c->toExpression(varNames, manager));
            }
            return conjunction(conjuncts);
        }
    }

   private:
    std::vector<std::shared_ptr<SmtConstraint>> constraints;
};

class Or : public SmtConstraint {
   public:
    Or(std::vector<std::shared_ptr<SmtConstraint>> const &constraints) : constraints(constraints) {}

    virtual ~Or() {}

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

    storm::expressions::Expression toExpression(std::vector<std::string> const &varNames,
                                                std::shared_ptr<storm::expressions::ExpressionManager> manager) const override {
        if (constraints.empty()) {
            return manager->boolean(false);
        } else {
            std::vector<storm::expressions::Expression> disjuncts;
            for (auto const &c : constraints) {
                disjuncts.push_back(c->toExpression(varNames, manager));
            }
            return disjunction(disjuncts);
        }
    }

   private:
    std::vector<std::shared_ptr<SmtConstraint>> constraints;
};

class Implies : public SmtConstraint {
   public:
    Implies(std::shared_ptr<SmtConstraint> l, std::shared_ptr<SmtConstraint> r) : lhs(l), rhs(r) {}

    std::string toSmtlib2(std::vector<std::string> const &varNames) const override {
        std::stringstream sstr;
        sstr << "(=> " << lhs->toSmtlib2(varNames) << " " << rhs->toSmtlib2(varNames) << ")";
        return sstr.str();
    }

    storm::expressions::Expression toExpression(std::vector<std::string> const &varNames,
                                                std::shared_ptr<storm::expressions::ExpressionManager> manager) const override {
        return implies(lhs->toExpression(varNames, manager), rhs->toExpression(varNames, manager));
    }

   private:
    std::shared_ptr<SmtConstraint> lhs;
    std::shared_ptr<SmtConstraint> rhs;
};

class Iff : public SmtConstraint {
   public:
    Iff(std::shared_ptr<SmtConstraint> l, std::shared_ptr<SmtConstraint> r) : lhs(l), rhs(r) {}

    std::string toSmtlib2(std::vector<std::string> const &varNames) const override {
        std::stringstream sstr;
        sstr << "(= " << lhs->toSmtlib2(varNames) << " " << rhs->toSmtlib2(varNames) << ")";
        return sstr.str();
    }

    storm::expressions::Expression toExpression(std::vector<std::string> const &varNames,
                                                std::shared_ptr<storm::expressions::ExpressionManager> manager) const override {
        return iff(lhs->toExpression(varNames, manager), rhs->toExpression(varNames, manager));
    }

   private:
    std::shared_ptr<SmtConstraint> lhs;
    std::shared_ptr<SmtConstraint> rhs;
};

class IsTrue : public SmtConstraint {
   public:
    IsTrue(bool val) : value(val) {}

    virtual ~IsTrue() {}

    std::string toSmtlib2(std::vector<std::string> const &varNames) const override {
        std::stringstream sstr;
        sstr << (value ? "true" : "false");
        return sstr.str();
    }

    storm::expressions::Expression toExpression(std::vector<std::string> const &varNames,
                                                std::shared_ptr<storm::expressions::ExpressionManager> manager) const override {
        return manager->boolean(value);
    }

   private:
    bool value;
};

class IsBoolValue : public SmtConstraint {
   public:
    IsBoolValue(uint64_t varIndex, bool val) : varIndex(varIndex), value(val) {}

    virtual ~IsBoolValue() {}

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

    storm::expressions::Expression toExpression(std::vector<std::string> const &varNames,
                                                std::shared_ptr<storm::expressions::ExpressionManager> manager) const override {
        if (value) {
            return manager->getVariableExpression(varNames.at(varIndex));
        } else {
            return !(manager->getVariableExpression(varNames.at(varIndex)));
        }
    }

   private:
    uint64_t varIndex;
    bool value;
};

class IsConstantValue : public SmtConstraint {
   public:
    IsConstantValue(uint64_t varIndex, uint64_t val) : varIndex(varIndex), value(val) {}

    virtual ~IsConstantValue() {}

    std::string toSmtlib2(std::vector<std::string> const &varNames) const override {
        std::stringstream sstr;
        assert(varIndex < varNames.size());
        sstr << "(= " << varNames.at(varIndex) << " " << value << ")";
        return sstr.str();
    }

    storm::expressions::Expression toExpression(std::vector<std::string> const &varNames,
                                                std::shared_ptr<storm::expressions::ExpressionManager> manager) const override {
        return manager->getVariableExpression(varNames.at(varIndex)) == manager->integer(value);
    }

   private:
    uint64_t varIndex;
    uint64_t value;
};

class IsNotConstantValue : public SmtConstraint {
   public:
    IsNotConstantValue(uint64_t varIndex, uint64_t val) : varIndex(varIndex), value(val) {}

    virtual ~IsNotConstantValue() {}

    std::string toSmtlib2(std::vector<std::string> const &varNames) const override {
        std::stringstream sstr;
        assert(varIndex < varNames.size());
        sstr << "(distinct " << varNames.at(varIndex) << " " << value << ")";
        return sstr.str();
    }

    storm::expressions::Expression toExpression(std::vector<std::string> const &varNames,
                                                std::shared_ptr<storm::expressions::ExpressionManager> manager) const override {
        return manager->getVariableExpression(varNames.at(varIndex)) != manager->integer(value);
    }

   private:
    uint64_t varIndex;
    uint64_t value;
};

class IsLessConstant : public SmtConstraint {
   public:
    IsLessConstant(uint64_t varIndex, uint64_t val) : varIndex(varIndex), value(val) {}

    virtual ~IsLessConstant() {}

    std::string toSmtlib2(std::vector<std::string> const &varNames) const override {
        std::stringstream sstr;
        assert(varIndex < varNames.size());
        sstr << "(< " << varNames.at(varIndex) << " " << value << ")";
        return sstr.str();
    }

    storm::expressions::Expression toExpression(std::vector<std::string> const &varNames,
                                                std::shared_ptr<storm::expressions::ExpressionManager> manager) const override {
        return manager->getVariableExpression(varNames.at(varIndex)) < value;
    }

   private:
    uint64_t varIndex;
    uint64_t value;
};

class IsGreaterConstant : public SmtConstraint {
   public:
    IsGreaterConstant(uint64_t varIndex, uint64_t val) : varIndex(varIndex), value(val) {}

    virtual ~IsGreaterConstant() {}

    std::string toSmtlib2(std::vector<std::string> const &varNames) const override {
        std::stringstream sstr;
        assert(varIndex < varNames.size());
        sstr << "(< " << value << " " << varNames.at(varIndex) << ")";
        return sstr.str();
    }

    storm::expressions::Expression toExpression(std::vector<std::string> const &varNames,
                                                std::shared_ptr<storm::expressions::ExpressionManager> manager) const override {
        return manager->getVariableExpression(varNames.at(varIndex)) > value;
    }

   private:
    uint64_t varIndex;
    uint64_t value;
};

class IsLessEqualConstant : public SmtConstraint {
   public:
    IsLessEqualConstant(uint64_t varIndex, uint64_t val) : varIndex(varIndex), value(val) {}

    virtual ~IsLessEqualConstant() {}

    std::string toSmtlib2(std::vector<std::string> const &varNames) const override {
        std::stringstream sstr;
        assert(varIndex < varNames.size());
        sstr << "(<= " << varNames.at(varIndex) << " " << value << ")";
        return sstr.str();
    }

    storm::expressions::Expression toExpression(std::vector<std::string> const &varNames,
                                                std::shared_ptr<storm::expressions::ExpressionManager> manager) const override {
        return manager->getVariableExpression(varNames.at(varIndex)) <= value;
    }

   private:
    uint64_t varIndex;
    uint64_t value;
};

class IsGreaterEqualConstant : public SmtConstraint {
   public:
    IsGreaterEqualConstant(uint64_t varIndex, uint64_t val) : varIndex(varIndex), value(val) {}

    virtual ~IsGreaterEqualConstant() {}

    std::string toSmtlib2(std::vector<std::string> const &varNames) const override {
        std::stringstream sstr;
        assert(varIndex < varNames.size());
        sstr << "(<= " << value << " " << varNames.at(varIndex) << ")";
        return sstr.str();
    }

    storm::expressions::Expression toExpression(std::vector<std::string> const &varNames,
                                                std::shared_ptr<storm::expressions::ExpressionManager> manager) const override {
        return manager->getVariableExpression(varNames.at(varIndex)) >= value;
    }

   private:
    uint64_t varIndex;
    uint64_t value;
};

class IsEqual : public SmtConstraint {
   public:
    IsEqual(uint64_t varIndex1, uint64_t varIndex2) : var1Index(varIndex1), var2Index(varIndex2) {}

    virtual ~IsEqual() {}

    std::string toSmtlib2(std::vector<std::string> const &varNames) const override {
        return "(= " + varNames.at(var1Index) + " " + varNames.at(var2Index) + ")";
    }

    storm::expressions::Expression toExpression(std::vector<std::string> const &varNames,
                                                std::shared_ptr<storm::expressions::ExpressionManager> manager) const override {
        return manager->getVariableExpression(varNames.at(var1Index)) == manager->getVariableExpression(varNames.at(var2Index));
    }

   private:
    uint64_t var1Index;
    uint64_t var2Index;
};

class IsUnequal : public SmtConstraint {
   public:
    IsUnequal(uint64_t varIndex1, uint64_t varIndex2) : var1Index(varIndex1), var2Index(varIndex2) {}

    virtual ~IsUnequal() {}

    std::string toSmtlib2(std::vector<std::string> const &varNames) const override {
        return "(distinct " + varNames.at(var1Index) + " " + varNames.at(var2Index) + ")";
    }

    storm::expressions::Expression toExpression(std::vector<std::string> const &varNames,
                                                std::shared_ptr<storm::expressions::ExpressionManager> manager) const override {
        return manager->getVariableExpression(varNames.at(var1Index)) != manager->getVariableExpression(varNames.at(var2Index));
    }

   private:
    uint64_t var1Index;
    uint64_t var2Index;
};

class IsLess : public SmtConstraint {
   public:
    IsLess(uint64_t varIndex1, uint64_t varIndex2) : var1Index(varIndex1), var2Index(varIndex2) {}

    virtual ~IsLess() {}

    std::string toSmtlib2(std::vector<std::string> const &varNames) const override {
        return "(< " + varNames.at(var1Index) + " " + varNames.at(var2Index) + ")";
    }

    storm::expressions::Expression toExpression(std::vector<std::string> const &varNames,
                                                std::shared_ptr<storm::expressions::ExpressionManager> manager) const override {
        return manager->getVariableExpression(varNames.at(var1Index)) < manager->getVariableExpression(varNames.at(var2Index));
    }

   private:
    uint64_t var1Index;
    uint64_t var2Index;
};

class IsLessEqual : public SmtConstraint {
   public:
    IsLessEqual(uint64_t varIndex1, uint64_t varIndex2) : var1Index(varIndex1), var2Index(varIndex2) {}

    virtual ~IsLessEqual() {}

    std::string toSmtlib2(std::vector<std::string> const &varNames) const override {
        return "(<= " + varNames.at(var1Index) + " " + varNames.at(var2Index) + ")";
    }

    storm::expressions::Expression toExpression(std::vector<std::string> const &varNames,
                                                std::shared_ptr<storm::expressions::ExpressionManager> manager) const override {
        return manager->getVariableExpression(varNames.at(var1Index)) <= manager->getVariableExpression(varNames.at(var2Index));
    }

   private:
    uint64_t var1Index;
    uint64_t var2Index;
};

class IsGreaterEqual : public SmtConstraint {
   public:
    IsGreaterEqual(uint64_t varIndex1, uint64_t varIndex2) : var1Index(varIndex1), var2Index(varIndex2) {}

    virtual ~IsGreaterEqual() {}

    std::string toSmtlib2(std::vector<std::string> const &varNames) const override {
        return "(>= " + varNames.at(var1Index) + " " + varNames.at(var2Index) + ")";
    }

    storm::expressions::Expression toExpression(std::vector<std::string> const &varNames,
                                                std::shared_ptr<storm::expressions::ExpressionManager> manager) const override {
        return manager->getVariableExpression(varNames.at(var1Index)) >= manager->getVariableExpression(varNames.at(var2Index));
    }

   private:
    uint64_t var1Index;
    uint64_t var2Index;
};

class PairwiseDifferent : public SmtConstraint {
   public:
    PairwiseDifferent(std::vector<uint64_t> const &indices) : varIndices(indices) {}

    virtual ~PairwiseDifferent() {}

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

    storm::expressions::Expression toExpression(std::vector<std::string> const &varNames,
                                                std::shared_ptr<storm::expressions::ExpressionManager> manager) const override {
        std::vector<storm::expressions::Expression> conjuncts;
        for (uint64_t i = 0; i < varIndices.size(); ++i) {
            for (uint64_t j = i + 1; j < varIndices.size(); ++j) {
                // check all elements pairwise for inequality
                conjuncts.push_back(manager->getVariableExpression(varNames.at(varIndices.at(i))) !=
                                    manager->getVariableExpression(varNames.at(varIndices.at(j))));
            }
        }
        // take the conjunction of all pairwise inequalities
        return conjunction(conjuncts);
    }

   private:
    std::vector<uint64_t> varIndices;
};

class Sorted : public SmtConstraint {
   public:
    Sorted(std::vector<uint64_t> varIndices) : varIndices(varIndices) {}

    virtual ~Sorted() {}

    std::string toSmtlib2(std::vector<std::string> const &varNames) const override {
        std::stringstream sstr;
        sstr << "(and ";
        for (uint64_t i = 1; i < varIndices.size(); ++i) {
            sstr << "(<= " << varNames.at(varIndices.at(i - 1)) << " " << varNames.at(varIndices.at(i)) << ")";
        }
        sstr << ") ";
        return sstr.str();
    }

    storm::expressions::Expression toExpression(std::vector<std::string> const &varNames,
                                                std::shared_ptr<storm::expressions::ExpressionManager> manager) const override {
        std::vector<storm::expressions::Expression> conjuncts;
        for (uint64_t i = 1; i < varIndices.size(); ++i) {
            conjuncts.push_back(manager->getVariableExpression(varNames.at(varIndices.at(i - 1))) <=
                                manager->getVariableExpression(varNames.at(varIndices.at(i))));
        }
        // take the conjunction of all pairwise inequalities
        return conjunction(conjuncts);
    }

   private:
    std::vector<uint64_t> varIndices;
};

class IfThenElse : public SmtConstraint {
   public:
    IfThenElse(std::shared_ptr<SmtConstraint> ifC, std::shared_ptr<SmtConstraint> thenC, std::shared_ptr<SmtConstraint> elseC)
        : ifConstraint(ifC), thenConstraint(thenC), elseConstraint(elseC) {}

    std::string toSmtlib2(std::vector<std::string> const &varNames) const override {
        std::stringstream sstr;
        sstr << "(ite " << ifConstraint->toSmtlib2(varNames) << " " << thenConstraint->toSmtlib2(varNames) << " " << elseConstraint->toSmtlib2(varNames) << ")";
        return sstr.str();
    }

    storm::expressions::Expression toExpression(std::vector<std::string> const &varNames,
                                                std::shared_ptr<storm::expressions::ExpressionManager> manager) const override {
        return ite(ifConstraint->toExpression(varNames, manager), thenConstraint->toExpression(varNames, manager),
                   elseConstraint->toExpression(varNames, manager));
    }

   private:
    std::shared_ptr<SmtConstraint> ifConstraint;
    std::shared_ptr<SmtConstraint> thenConstraint;
    std::shared_ptr<SmtConstraint> elseConstraint;
};

class TrueCountIsLessConstant : public SmtConstraint {
   public:
    TrueCountIsLessConstant(std::vector<uint64_t> varIndices, uint64_t val) : varIndices(varIndices), value(val) {}

    std::string toSmtlib2(std::vector<std::string> const &varNames) const override {
        std::stringstream sstr;
        sstr << "(< (+ ";
        for (uint64_t i = 0; i < varIndices.size(); ++i) {
            sstr << "(ite " << varNames.at(varIndices.at(i)) << " 1 0 )";
        }
        sstr << ") " << value << " )";
        return sstr.str();
    }

    storm::expressions::Expression toExpression(std::vector<std::string> const &varNames,
                                                std::shared_ptr<storm::expressions::ExpressionManager> manager) const override {
        std::vector<storm::expressions::Expression> boolToInt;
        for (uint64_t i = 0; i < varIndices.size(); ++i) {
            boolToInt.push_back(ite(manager->getVariableExpression(varNames.at(varIndices.at(i))),  // If variable is true
                                    manager->integer(1),                                            // set 1
                                    manager->integer(0)));                                          // else 0
        }
        return sum(boolToInt) < manager->integer(value);
    }

   private:
    std::vector<uint64_t> varIndices;
    uint64_t value;
};

class FalseCountIsEqualConstant : public SmtConstraint {
   public:
    FalseCountIsEqualConstant(std::vector<uint64_t> varIndices, uint64_t val) : varIndices(varIndices), value(val) {}

    std::string toSmtlib2(std::vector<std::string> const &varNames) const override {
        std::stringstream sstr;
        sstr << "(= (+ ";
        for (uint64_t i = 0; i < varIndices.size(); ++i) {
            sstr << "(ite " << varNames.at(varIndices.at(i)) << " 0 1 )";
        }
        sstr << ") " << value << " )";
        return sstr.str();
    }

    storm::expressions::Expression toExpression(std::vector<std::string> const &varNames,
                                                std::shared_ptr<storm::expressions::ExpressionManager> manager) const override {
        std::vector<storm::expressions::Expression> boolToInt;
        for (uint64_t i = 0; i < varIndices.size(); ++i) {
            boolToInt.push_back(ite(manager->getVariableExpression(varNames.at(varIndices.at(i))),  // If variable is true
                                    manager->integer(0),                                            // set 0
                                    manager->integer(1)));                                          // else 1
        }
        return sum(boolToInt) == manager->integer(value);
    }

   private:
    std::vector<uint64_t> varIndices;
    uint64_t value;
};

class TrueCountIsConstantValue : public SmtConstraint {
   public:
    TrueCountIsConstantValue(std::vector<uint64_t> varIndices, uint64_t val) : varIndices(varIndices), value(val) {}

    std::string toSmtlib2(std::vector<std::string> const &varNames) const override {
        std::stringstream sstr;
        sstr << "(= (+ ";
        for (uint64_t i = 0; i < varIndices.size(); ++i) {
            sstr << "(ite " << varNames.at(varIndices.at(i)) << " 1 0 )";
        }
        sstr << ") " << value << " )";
        return sstr.str();
    }

    storm::expressions::Expression toExpression(std::vector<std::string> const &varNames,
                                                std::shared_ptr<storm::expressions::ExpressionManager> manager) const override {
        std::vector<storm::expressions::Expression> boolToInt;
        for (uint64_t i = 0; i < varIndices.size(); ++i) {
            boolToInt.push_back(ite(manager->getVariableExpression(varNames.at(varIndices.at(i))),  // If variable is true
                                    manager->integer(1),                                            // set 1
                                    manager->integer(0)));                                          // else 0
        }
        return sum(boolToInt) == manager->integer(value);
    }

   private:
    std::vector<uint64_t> varIndices;
    uint64_t value;
};

}  // namespace modelchecker
}  // namespace storm::dft
