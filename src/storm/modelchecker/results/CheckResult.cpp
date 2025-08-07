#include "storm/modelchecker/results/CheckResult.h"

#include "storm-config.h"
#include "storm/adapters/RationalFunctionAdapter.h"

#include "storm/modelchecker/results/ExplicitParetoCurveCheckResult.h"
#include "storm/modelchecker/results/ExplicitQualitativeCheckResult.h"
#include "storm/modelchecker/results/ExplicitQuantitativeCheckResult.h"
#include "storm/modelchecker/results/HybridQuantitativeCheckResult.h"
#include "storm/modelchecker/results/LexicographicCheckResult.h"
#include "storm/modelchecker/results/SymbolicParetoCurveCheckResult.h"
#include "storm/modelchecker/results/SymbolicQualitativeCheckResult.h"
#include "storm/modelchecker/results/SymbolicQuantitativeCheckResult.h"

#include "storm/exceptions/InvalidOperationException.h"
#include "storm/utility/macros.h"

namespace storm {
namespace modelchecker {
bool CheckResult::isExplicit() const {
    return false;
}

bool CheckResult::isSymbolic() const {
    return false;
}

bool CheckResult::isHybrid() const {
    return false;
}

bool CheckResult::isQuantitative() const {
    return false;
}

bool CheckResult::isQualitative() const {
    return false;
}

bool CheckResult::isParetoCurveCheckResult() const {
    return false;
}

bool CheckResult::isLexicographicCheckResult() const {
    return false;
}

bool CheckResult::isResultForAllStates() const {
    return false;
}

std::ostream& operator<<(std::ostream& out, CheckResult const& checkResult) {
    checkResult.writeToStream(out);
    return out;
}

bool CheckResult::isExplicitQualitativeCheckResult() const {
    return false;
}

bool CheckResult::isExplicitQuantitativeCheckResult() const {
    return false;
}

bool CheckResult::isExplicitParetoCurveCheckResult() const {
    return false;
}

bool CheckResult::isSymbolicQualitativeCheckResult() const {
    return false;
}

bool CheckResult::isSymbolicQuantitativeCheckResult() const {
    return false;
}

bool CheckResult::isSymbolicParetoCurveCheckResult() const {
    return false;
}

bool CheckResult::isHybridQuantitativeCheckResult() const {
    return false;
}

ExplicitQualitativeCheckResult& CheckResult::asExplicitQualitativeCheckResult() {
    return dynamic_cast<ExplicitQualitativeCheckResult&>(*this);
}

ExplicitQualitativeCheckResult const& CheckResult::asExplicitQualitativeCheckResult() const {
    return dynamic_cast<ExplicitQualitativeCheckResult const&>(*this);
}

template<typename ValueType>
ExplicitQuantitativeCheckResult<ValueType>& CheckResult::asExplicitQuantitativeCheckResult() {
    return dynamic_cast<ExplicitQuantitativeCheckResult<ValueType>&>(*this);
}

template<typename ValueType>
ExplicitQuantitativeCheckResult<ValueType> const& CheckResult::asExplicitQuantitativeCheckResult() const {
    return dynamic_cast<ExplicitQuantitativeCheckResult<ValueType> const&>(*this);
}

template<typename ValueType>
ExplicitParetoCurveCheckResult<ValueType>& CheckResult::asExplicitParetoCurveCheckResult() {
    return dynamic_cast<ExplicitParetoCurveCheckResult<ValueType>&>(*this);
}

template<typename ValueType>
ExplicitParetoCurveCheckResult<ValueType> const& CheckResult::asExplicitParetoCurveCheckResult() const {
    return dynamic_cast<ExplicitParetoCurveCheckResult<ValueType> const&>(*this);
}

template<typename ValueType>
LexicographicCheckResult<ValueType>& CheckResult::asLexicographicCheckResult() {
    return dynamic_cast<LexicographicCheckResult<ValueType>&>(*this);
}

template<typename ValueType>
LexicographicCheckResult<ValueType> const& CheckResult::asLexicographicCheckResult() const {
    return dynamic_cast<LexicographicCheckResult<ValueType> const&>(*this);
}

QualitativeCheckResult& CheckResult::asQualitativeCheckResult() {
    return dynamic_cast<QualitativeCheckResult&>(*this);
}

QualitativeCheckResult const& CheckResult::asQualitativeCheckResult() const {
    return dynamic_cast<QualitativeCheckResult const&>(*this);
}

template<typename ValueType>
QuantitativeCheckResult<ValueType>& CheckResult::asQuantitativeCheckResult() {
    return static_cast<QuantitativeCheckResult<ValueType>&>(*this);
}

template<typename ValueType>
QuantitativeCheckResult<ValueType> const& CheckResult::asQuantitativeCheckResult() const {
    return static_cast<QuantitativeCheckResult<ValueType> const&>(*this);
}

template<storm::dd::DdType Type>
SymbolicQualitativeCheckResult<Type>& CheckResult::asSymbolicQualitativeCheckResult() {
    return dynamic_cast<SymbolicQualitativeCheckResult<Type>&>(*this);
}

template<storm::dd::DdType Type>
SymbolicQualitativeCheckResult<Type> const& CheckResult::asSymbolicQualitativeCheckResult() const {
    return dynamic_cast<SymbolicQualitativeCheckResult<Type> const&>(*this);
}

template<storm::dd::DdType Type, typename ValueType>
SymbolicQuantitativeCheckResult<Type, ValueType>& CheckResult::asSymbolicQuantitativeCheckResult() {
    return dynamic_cast<SymbolicQuantitativeCheckResult<Type, ValueType>&>(*this);
}

template<storm::dd::DdType Type, typename ValueType>
SymbolicQuantitativeCheckResult<Type, ValueType> const& CheckResult::asSymbolicQuantitativeCheckResult() const {
    return dynamic_cast<SymbolicQuantitativeCheckResult<Type, ValueType> const&>(*this);
}

template<storm::dd::DdType Type, typename ValueType>
HybridQuantitativeCheckResult<Type, ValueType>& CheckResult::asHybridQuantitativeCheckResult() {
    return dynamic_cast<HybridQuantitativeCheckResult<Type, ValueType>&>(*this);
}

template<storm::dd::DdType Type, typename ValueType>
HybridQuantitativeCheckResult<Type, ValueType> const& CheckResult::asHybridQuantitativeCheckResult() const {
    return dynamic_cast<HybridQuantitativeCheckResult<Type, ValueType> const&>(*this);
}

template<storm::dd::DdType Type, typename ValueType>
SymbolicParetoCurveCheckResult<Type, ValueType>& CheckResult::asSymbolicParetoCurveCheckResult() {
    return dynamic_cast<SymbolicParetoCurveCheckResult<Type, ValueType>&>(*this);
}

template<storm::dd::DdType Type, typename ValueType>
SymbolicParetoCurveCheckResult<Type, ValueType> const& CheckResult::asSymbolicParetoCurveCheckResult() const {
    return dynamic_cast<SymbolicParetoCurveCheckResult<Type, ValueType> const&>(*this);
}

bool CheckResult::hasScheduler() const {
    return false;
}

// Explicitly instantiate the template functions.
template QuantitativeCheckResult<double>& CheckResult::asQuantitativeCheckResult();
template QuantitativeCheckResult<double> const& CheckResult::asQuantitativeCheckResult() const;

template ExplicitQuantitativeCheckResult<double>& CheckResult::asExplicitQuantitativeCheckResult();
template ExplicitQuantitativeCheckResult<double> const& CheckResult::asExplicitQuantitativeCheckResult() const;
template ExplicitParetoCurveCheckResult<double>& CheckResult::asExplicitParetoCurveCheckResult();
template ExplicitParetoCurveCheckResult<double> const& CheckResult::asExplicitParetoCurveCheckResult() const;
template LexicographicCheckResult<double>& CheckResult::asLexicographicCheckResult();
template LexicographicCheckResult<double> const& CheckResult::asLexicographicCheckResult() const;

template SymbolicQualitativeCheckResult<storm::dd::DdType::CUDD>& CheckResult::asSymbolicQualitativeCheckResult();
template SymbolicQualitativeCheckResult<storm::dd::DdType::CUDD> const& CheckResult::asSymbolicQualitativeCheckResult() const;
template SymbolicQuantitativeCheckResult<storm::dd::DdType::CUDD, double>& CheckResult::asSymbolicQuantitativeCheckResult();
template SymbolicQuantitativeCheckResult<storm::dd::DdType::CUDD, double> const& CheckResult::asSymbolicQuantitativeCheckResult() const;
template SymbolicParetoCurveCheckResult<storm::dd::DdType::CUDD, double>& CheckResult::asSymbolicParetoCurveCheckResult();
template SymbolicParetoCurveCheckResult<storm::dd::DdType::CUDD, double> const& CheckResult::asSymbolicParetoCurveCheckResult() const;
template HybridQuantitativeCheckResult<storm::dd::DdType::CUDD, double>& CheckResult::asHybridQuantitativeCheckResult();
template HybridQuantitativeCheckResult<storm::dd::DdType::CUDD, double> const& CheckResult::asHybridQuantitativeCheckResult() const;

template SymbolicQualitativeCheckResult<storm::dd::DdType::Sylvan>& CheckResult::asSymbolicQualitativeCheckResult();
template SymbolicQualitativeCheckResult<storm::dd::DdType::Sylvan> const& CheckResult::asSymbolicQualitativeCheckResult() const;
template SymbolicQuantitativeCheckResult<storm::dd::DdType::Sylvan, double>& CheckResult::asSymbolicQuantitativeCheckResult();
template SymbolicQuantitativeCheckResult<storm::dd::DdType::Sylvan, double> const& CheckResult::asSymbolicQuantitativeCheckResult() const;
template SymbolicQuantitativeCheckResult<storm::dd::DdType::Sylvan, storm::RationalNumber>& CheckResult::asSymbolicQuantitativeCheckResult();
template SymbolicQuantitativeCheckResult<storm::dd::DdType::Sylvan, storm::RationalNumber> const& CheckResult::asSymbolicQuantitativeCheckResult() const;
template SymbolicQuantitativeCheckResult<storm::dd::DdType::Sylvan, storm::RationalFunction>& CheckResult::asSymbolicQuantitativeCheckResult();
template SymbolicQuantitativeCheckResult<storm::dd::DdType::Sylvan, storm::RationalFunction> const& CheckResult::asSymbolicQuantitativeCheckResult() const;
template SymbolicParetoCurveCheckResult<storm::dd::DdType::Sylvan, double>& CheckResult::asSymbolicParetoCurveCheckResult();
template SymbolicParetoCurveCheckResult<storm::dd::DdType::Sylvan, double> const& CheckResult::asSymbolicParetoCurveCheckResult() const;
template HybridQuantitativeCheckResult<storm::dd::DdType::Sylvan, double>& CheckResult::asHybridQuantitativeCheckResult();
template HybridQuantitativeCheckResult<storm::dd::DdType::Sylvan, double> const& CheckResult::asHybridQuantitativeCheckResult() const;

#ifdef STORM_HAVE_CARL
template QuantitativeCheckResult<storm::RationalNumber>& CheckResult::asQuantitativeCheckResult();
template QuantitativeCheckResult<storm::RationalNumber> const& CheckResult::asQuantitativeCheckResult() const;

template ExplicitQuantitativeCheckResult<storm::RationalNumber>& CheckResult::asExplicitQuantitativeCheckResult();
template ExplicitQuantitativeCheckResult<storm::RationalNumber> const& CheckResult::asExplicitQuantitativeCheckResult() const;

template QuantitativeCheckResult<storm::RationalFunction>& CheckResult::asQuantitativeCheckResult();
template QuantitativeCheckResult<storm::RationalFunction> const& CheckResult::asQuantitativeCheckResult() const;

template ExplicitQuantitativeCheckResult<storm::RationalFunction>& CheckResult::asExplicitQuantitativeCheckResult();
template ExplicitQuantitativeCheckResult<storm::RationalFunction> const& CheckResult::asExplicitQuantitativeCheckResult() const;

template ExplicitParetoCurveCheckResult<storm::RationalNumber>& CheckResult::asExplicitParetoCurveCheckResult();
template ExplicitParetoCurveCheckResult<storm::RationalNumber> const& CheckResult::asExplicitParetoCurveCheckResult() const;

template LexicographicCheckResult<storm::RationalNumber>& CheckResult::asLexicographicCheckResult();
template LexicographicCheckResult<storm::RationalNumber> const& CheckResult::asLexicographicCheckResult() const;

#endif
}  // namespace modelchecker
}  // namespace storm
