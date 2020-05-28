#pragma once

#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <memory>

#include "storm-dft/modelchecker/dft/SFTBDDChecker.h"
#include "storm-dft/storage/SylvanBddManager.h"
#include "storm/logic/AtomicLabelFormula.h"
#include "storm/logic/BinaryBooleanStateFormula.h"
#include "storm/logic/BoundedUntilFormula.h"
#include "storm/logic/ProbabilityOperatorFormula.h"
#include "storm/logic/StateFormula.h"
#include "storm/logic/UnaryBooleanStateFormula.h"

namespace storm {
namespace adapters {

class SFTBDDPropertyFormulaAdapter {
    using ValueType = double;
    using Bdd = sylvan::Bdd;
    using FormulaCPointer = std::shared_ptr<storm::logic::Formula const>;
    using StateFormulaCPointer =
        std::shared_ptr<storm::logic::StateFormula const>;
    using UnaryStateFormulaCPointer =
        std::shared_ptr<storm::logic::UnaryBooleanStateFormula const>;
    using BinaryStateFormulaCPointer =
        std::shared_ptr<storm::logic::BinaryBooleanStateFormula const>;
    using AtomicLabelFormulaCPointer =
        std::shared_ptr<storm::logic::AtomicLabelFormula const>;
    using FormulaVector = std::vector<FormulaCPointer>;

   public:
    SFTBDDPropertyFormulaAdapter(
        std::shared_ptr<storm::storage::DFT<ValueType>> dft)
        : sylvanBddManager{std::make_shared<
              storm::storage::SylvanBddManager>()},
          checker{dft},
          dft{dft} {}

    SFTBDDPropertyFormulaAdapter(
        std::shared_ptr<storm::storage::SylvanBddManager> sylvanBddManager,
        std::shared_ptr<storm::storage::DFT<ValueType>> dft)
        : sylvanBddManager{sylvanBddManager}, checker{dft}, dft{dft} {}

    /**
     * Calculate the properties specified by the formulas
     * \param formuals
     * The Properties to check for.
     */
    std::vector<ValueType> check(FormulaVector const &formulas,
                                 size_t const chunksize = 0) {
        checkForm(formulas);
        auto const bdds{formulasToBdd(formulas)};

        std::map<uint64_t, Bdd> BDDToBdd{};
        for (auto const &bdd : bdds) {
            BDDToBdd[bdd.GetBDD()] = bdd;
        }

        std::map<uint64_t, std::vector<double>> bddToReversedTimepoints{};
        for (size_t i{0}; i < bdds.size(); ++i) {
            auto const reversedIndex{bdds.size() - i - 1};
            auto const &bdd{bdds[reversedIndex]};
            auto const &formula{formulas[reversedIndex]};
            auto const timebound{getTimebound(formula)};

            bddToReversedTimepoints[bdd.GetBDD()].push_back(timebound);
        }

        std::map<uint64_t, std::vector<double>> bddToReversedProbabilities{};
        for (auto const &pair : bddToReversedTimepoints) {
            auto const bdd{BDDToBdd.at(pair.first)};
            bddToReversedProbabilities[pair.first] =
                checker.getProbabilitiesAtTimepoints(bdd, pair.second,
                                                     chunksize);
        }

        std::vector<ValueType> rval{};
        rval.reserve(bdds.size());

        for (size_t i{0}; i < bdds.size(); ++i) {
            auto const &bdd{bdds[i]};
            auto &tmpVec{bddToReversedProbabilities.at(bdd.GetBDD())};
            rval.push_back(tmpVec.back());
            tmpVec.pop_back();
        }

        return rval;
    }

    /**
     * \return
     * The bdds representing the StatesFormulas of the given formulas
     *
     * \param formulas
     * The Properties to extract the StateFormulas of.
     */
    std::vector<Bdd> formulasToBdd(FormulaVector const &formulas) {
        calculateRelevantEventBdds(formulas);
        std::vector<Bdd> rval{};
        rval.reserve(formulas.size());
        for (auto const &formula : formulas) {
            enableNot = checkBoundsSame(formula);
            rval.push_back(StateFormulaToBdd(toStateFormula(formula)));
        }
        return rval;
    }

    // TODO: Move formulahandling into its own module
    /**
     * Check if the formulas are of the form 'P=? [F op phi]'
     * where op is in {<=, <, =} and phi is a state formula
     */
    static void checkForm(FormulaVector const &formulas) {
        for (auto const &formula : formulas) {
            if (formula->isProbabilityOperatorFormula()) {
                auto const probabilityOperator{std::static_pointer_cast<
                    storm::logic::ProbabilityOperatorFormula const>(formula)};
                auto const subFormula{
                    probabilityOperator->getSubformula().asSharedPointer()};
                if (subFormula->isBoundedUntilFormula()) {
                    auto const boundedUntil{std::static_pointer_cast<
                        storm::logic::BoundedUntilFormula const>(subFormula)};

                    auto const leftSide{
                        boundedUntil->getLeftSubformula().asSharedPointer()};
                    if (!leftSide->isTrueFormula()) {
                        STORM_LOG_THROW(
                            false, storm::exceptions::NotSupportedException,
                            "Left side is not a TrueFormula.");
                    }

                    auto const rightSide{
                        boundedUntil->getRightSubformula().asSharedPointer()};
                    if (!rightSide->isStateFormula()) {
                        STORM_LOG_THROW(
                            false, storm::exceptions::NotSupportedException,
                            "Right side is not a StateFormula.");
                    }

                    if (!boundedUntil->hasUpperBound()) {
                        STORM_LOG_THROW(
                            false, storm::exceptions::NotSupportedException,
                            "UpperBound must be set.");
                    } else if (boundedUntil->hasUpperBound() &&
                               boundedUntil->hasLowerBound()) {
                        // Check if '[F = x phi]' was used.
                        if (boundedUntil->getUpperBound().evaluateAsDouble() !=
                            boundedUntil->getLowerBound().evaluateAsDouble()) {
                            STORM_LOG_THROW(
                                false, storm::exceptions::NotSupportedException,
                                "upperBound is set wrongly. "
                                "Only lowerBound == upperBound is Supported.");
                        }
                    }
                } else {
                    STORM_LOG_THROW(false,
                                    storm::exceptions::NotSupportedException,
                                    "SubFormula is not a BoundedUntilFormula.");
                }
            } else {
                STORM_LOG_THROW(
                    false, storm::exceptions::NotSupportedException,
                    "Only ProbabilityOperatorFormulas are supported.");
            }
        }
    }

    /**
     * \return
     * The upper timebound of the given formula
     */
    static double getTimebound(FormulaCPointer const &formula) {
        auto const probabilityOperator{std::static_pointer_cast<
            storm::logic::ProbabilityOperatorFormula const>(formula)};

        auto const subFormula{
            probabilityOperator->getSubformula().asSharedPointer()};

        auto const boundedUntil{
            std::static_pointer_cast<storm::logic::BoundedUntilFormula const>(
                subFormula)};

        return boundedUntil->getUpperBound().evaluateAsDouble();
    }

   private:
    std::shared_ptr<storm::storage::SylvanBddManager> sylvanBddManager;
    storm::modelchecker::SFTBDDChecker checker;
    std::shared_ptr<storm::storage::DFT<ValueType>> dft;

    std::map<std::string, Bdd> relevantEventBdds;

    /**
     * \return
     * true iff the formula if of the form 'P=? [F = x phi]'
     */
    static bool checkBoundsSame(FormulaCPointer const &formula) {
        auto const probabilityOperator{std::static_pointer_cast<
            storm::logic::ProbabilityOperatorFormula const>(formula)};
        auto const boundedUntil{
            std::static_pointer_cast<storm::logic::BoundedUntilFormula const>(
                probabilityOperator->getSubformula().asSharedPointer())};

        if (!boundedUntil->hasLowerBound() || !boundedUntil->hasUpperBound()) {
            return false;
        } else if (boundedUntil->getUpperBound().evaluateAsDouble() ==
                   boundedUntil->getLowerBound().evaluateAsDouble()) {
            return true;
        }

        return false;
    }

    /**
     * \return
     * The nested StateFormula of the given formula
     */
    static StateFormulaCPointer toStateFormula(FormulaCPointer const &formula) {
        auto const probabilityOperator{std::static_pointer_cast<
            storm::logic::ProbabilityOperatorFormula const>(formula)};

        auto const subFormula{
            probabilityOperator->getSubformula().asSharedPointer()};

        auto const boundedUntil{
            std::static_pointer_cast<storm::logic::BoundedUntilFormula const>(
                subFormula)};

        auto const rightSide{
            boundedUntil->getRightSubformula().asSharedPointer()};

        auto const stateFormula{
            std::static_pointer_cast<storm::logic::StateFormula const>(
                rightSide)};

        return stateFormula;
    }

    /**
     * \return
     * The Name of the event referenced by the given AtomicLabelFormula
     */
    std::string getAtomicLabelString(
        AtomicLabelFormulaCPointer const &formula) const {
        auto const label{formula->getLabel()};
        if (label == "failed") {
            return dft->getTopLevelGate()->name();
        } else if (boost::ends_with(label, "_failed")) {
            auto const name{label.substr(0, label.size() - 7)};
            return name;
        }

        STORM_LOG_THROW(false, storm::exceptions::NotSupportedException,
                        "Illegal AtomicLabelFormula: " << formula->toString());
        return "__ERROR__";
    }

    /**
     * \return
     * The names of all referenced events by the given formulas
     */
    std::set<std::string> getRelevantElementNames(
        FormulaVector const &formulas) const {
        std::set<std::string> rval{};

        std::vector<std::shared_ptr<storm::logic::AtomicLabelFormula const>>
            atomicLabels;

        for (auto const &formula : formulas) {
            toStateFormula(formula)->gatherAtomicLabelFormulas(atomicLabels);
        }

        for (auto const &atomicLabel : atomicLabels) {
            rval.insert(getAtomicLabelString(atomicLabel));
        }

        return rval;
    }

    /**
     * Set relevantEventBdds with the appropriate bdds
     */
    void calculateRelevantEventBdds(FormulaVector const &formulas) {
        relevantEventBdds =
            checker.getRelevantEventBdds(getRelevantElementNames(formulas));
    }

    /**
     * \return
     * The bdds representing the StatesFormulas of the given formula
     */
    Bdd FormulaToBdd(FormulaCPointer const &formula) const {
        if (formula->isStateFormula()) {
            return StateFormulaToBdd(
                std::static_pointer_cast<storm::logic::StateFormula const>(
                    formula));
        }

        STORM_LOG_THROW(false, storm::exceptions::NotSupportedException,
                        "Illegal Formula: " << formula->toString());
        return sylvanBddManager->getZero();
    }

    /**
     * \return
     * The bdds representing the given Stateformula
     */
    Bdd StateFormulaToBdd(StateFormulaCPointer const &formula) const {
        if (formula->isBinaryBooleanStateFormula()) {
            return binaryStateFormulaToBdd(
                std::static_pointer_cast<
                    storm::logic::BinaryBooleanStateFormula const>(formula));
        } else if (formula->isAtomicLabelFormula()) {
            return atomicLabelFormulaToBdd(
                std::static_pointer_cast<
                    storm::logic::AtomicLabelFormula const>(formula));
        } else if (formula->isUnaryBooleanStateFormula()) {
            return unaryStateFormulaToBdd(
                std::static_pointer_cast<
                    storm::logic::UnaryBooleanStateFormula const>(formula));
        }

        STORM_LOG_THROW(false, storm::exceptions::NotSupportedException,
                        "Illegal StateFormula: " << formula->toString());
        return sylvanBddManager->getZero();
    }

    /**
     * \return
     * The bdds representing the given Stateformula
     */
    Bdd binaryStateFormulaToBdd(
        BinaryStateFormulaCPointer const &formula) const {
        auto const leftBdd{
            FormulaToBdd(formula->getLeftSubformula().asSharedPointer())};
        auto const rightBdd{
            FormulaToBdd(formula->getRightSubformula().asSharedPointer())};

        if (formula->isAnd()) {
            return leftBdd & rightBdd;
        } else if (formula->isOr()) {
            return leftBdd | rightBdd;
        }

        STORM_LOG_THROW(false, storm::exceptions::NotSupportedException,
                        "Illegal BinaryStateFormula: " << formula->toString());
        return sylvanBddManager->getZero();
    }

    bool enableNot{false};

    /**
     * \return
     * The bdds representing the given Stateformula
     *
     * \note Only works if enableNot is true
     * as negation only works with timepoints not timebounds.
     */
    Bdd unaryStateFormulaToBdd(UnaryStateFormulaCPointer const &formula) const {
        if (!enableNot) {
            STORM_LOG_THROW(false, storm::exceptions::NotSupportedException,
                            "Illegal UnaryStateFormula: \""
                                << formula->toString()
                                << "\". Can only use negation with a formula "
                                   "of the form 'P=? [F = x phi]'");
            return sylvanBddManager->getZero();
        }
        auto const subBdd{
            FormulaToBdd(formula->getSubformula().asSharedPointer())};

        if (formula->isNot()) {
            return !subBdd;
        }

        STORM_LOG_THROW(false, storm::exceptions::NotSupportedException,
                        "Illegal UnaryStateFormula: " << formula->toString());
        return sylvanBddManager->getZero();
    }

    /**
     * \return
     * The bdds representing the given Stateformula
     */
    Bdd atomicLabelFormulaToBdd(
        AtomicLabelFormulaCPointer const &formula) const {
        return relevantEventBdds.at(getAtomicLabelString(formula));
    }
};

}  // namespace adapters
}  // namespace storm
