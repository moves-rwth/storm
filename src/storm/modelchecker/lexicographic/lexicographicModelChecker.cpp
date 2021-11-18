//
// Created by steffi on 05.11.21.
//
#include "storm/modelchecker/lexicographic/lexicographicModelChecker.h"
#include "storm/models/sparse/Mdp.h"
#include "storm/logic/Formula.h"
//#include "storm/modelchecker/CheckTask.h"
#include "storm/logic/ExtractMaximalStateFormulasVisitor.h"
#include "storm/automata/APSet.h"
//#include "storm/modelchecker/helper/ltl/SparseLTLHelper.h"
//#include "storm/exceptions/NotSupportedException.h"
//#include "storm/exceptions/ExpressionEvaluationException.h"
#include "storm/automata/DeterministicAutomaton.h"
#include "storm/transformer/DAProductBuilder.h"
#include "storm/modelchecker/lexicographic/spotHelper/spotProduct.h"

namespace storm {
    namespace modelchecker {
        namespace lexicographic {

        template<typename SparseModelType, typename ValueType, bool Nondeterministic>
        std::pair<int, int> lexicographicModelChecker<SparseModelType, ValueType, Nondeterministic>::getCompleteProductModel(const SparseModelType& model, CheckFormulaCallback const& formulaChecker) {
            storm::logic::ExtractMaximalStateFormulasVisitor::ApToFormulaMap extracted;
            // Get the big product automton for all subformulae
            std::shared_ptr<storm::automata::DeterministicAutomaton> productAutomaton = spothelper::ltl2daSpotProduct<SparseModelType, ValueType>(this->formula, formulaChecker, model, extracted);

            // Compute Satisfaction sets for the APs (which represent the state-subformulae
            auto apSets = computeApSets(extracted, formulaChecker);

            std::map<std::string, storm::storage::BitVector> apSatSets = computeApSets(extracted, formulaChecker);
            const storm::automata::APSet& apSet = productAutomaton->getAPSet();
            std::vector<storm::storage::BitVector> statesForAP;
            for (const std::string& ap : apSet.getAPs()) {
                auto it = apSatSets.find(ap);
                STORM_LOG_THROW(it != apSatSets.end(), storm::exceptions::InvalidOperationException,
                                "Deterministic automaton has AP " << ap << ", does not appear in formula");

                statesForAP.push_back(std::move(it->second));
            }

            storm::storage::BitVector statesOfInterest;

            if (this->hasRelevantStates()) {
                statesOfInterest = this->getRelevantStates();
            } else {
                // Product from all model states
                statesOfInterest = storm::storage::BitVector(this->_transitionMatrix.getRowGroupCount(), true);
            }

            transformer::DAProductBuilder productBuilder(*productAutomaton, statesForAP);


            auto product = productBuilder.build<productModelType>(model.getTransitionMatrix(), statesOfInterest);

            return std::make_pair(0,0);
        }

        template<typename SparseModelType, typename ValueType, bool Nondeterministic>
        std::pair<int, int> lexicographicModelChecker<SparseModelType, ValueType, Nondeterministic>::solve(int productModel, int acceptanceCondition) {
            return std::pair<int, int>(0,0);
        }

        template<typename SparseModelType, typename ValueType, bool Nondeterministic>
        int lexicographicModelChecker<SparseModelType, ValueType, Nondeterministic>::reachability(int bcc, int bccLexArray, int productModel) {
            return 0;
        }

        template<typename SparseModelType, typename ValueType, bool Nondeterministic>
        std::map<std::string, storm::storage::BitVector> lexicographicModelChecker<SparseModelType, ValueType, Nondeterministic>::computeApSets(std::map<std::string, std::shared_ptr<storm::logic::Formula const>> const& extracted, CheckFormulaCallback const& formulaChecker) {
            std::map<std::string, storm::storage::BitVector> apSets;
            for (auto& p: extracted) {
                STORM_LOG_DEBUG(" Computing satisfaction set for atomic proposition \"" << p.first << "\" <=> " << *p.second << "...");
                apSets[p.first] = formulaChecker(*p.second);
            }
            return apSets;
        }

        std::string exec(const char* cmd) {
            std::array<char, 128> buffer{};
            std::string result;
            std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
            if (!pipe) {
                throw std::runtime_error("popen() failed!");
            }
            while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
                result += buffer.data();
            }
            return result;
        }



        template class lexicographic::lexicographicModelChecker<storm::models::sparse::Mdp<double>, double, true>;
        template class lexicographic::lexicographicModelChecker<storm::models::sparse::Mdp<double>, double, false>;
        template class lexicographic::lexicographicModelChecker<storm::models::sparse::Mdp<storm::RationalNumber>, storm::RationalNumber, true>;
        template class lexicographic::lexicographicModelChecker<storm::models::sparse::Mdp<storm::RationalNumber>, storm::RationalNumber, false>;
        }
    }
}
