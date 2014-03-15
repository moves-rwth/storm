#include "MarkovAutomatonParser.h"
#include "AtomicPropositionLabelingParser.h"
#include "SparseStateRewardParser.h"

namespace storm {
    namespace parser {
        
        storm::models::MarkovAutomaton<double> MarkovAutomatonParser::parseMarkovAutomaton(std::string const& transitionsFilename, std::string const& labelingFilename, std::string const& stateRewardFilename, std::string const& transitionRewardFilename) {
            storm::parser::MarkovAutomatonSparseTransitionParser::ResultType transitionResult(storm::parser::MarkovAutomatonSparseTransitionParser::parseMarkovAutomatonTransitions(transitionsFilename));
            storm::models::AtomicPropositionsLabeling resultLabeling(storm::parser::AtomicPropositionLabelingParser(transitionResult.transitionMatrix.getColumnCount(), labelingFilename));

            boost::optional<std::vector<double>> stateRewards;
            if (stateRewardFilename != "") {
                stateRewards.reset(storm::parser::SparseStateRewardParser(transitionResult.transitionMatrix.getColumnCount(), stateRewardFilename));
            }
            
            if (transitionRewardFilename != "") {
                LOG4CPLUS_ERROR(logger, "Transition rewards are unsupported for Markov automata.");
                throw storm::exceptions::WrongFormatException() << "Transition rewards are unsupported for Markov automata.";
            }
            
            storm::models::MarkovAutomaton<double> resultingAutomaton(std::move(transitionResult.transitionMatrix), std::move(resultLabeling), std::move(transitionResult.markovianStates), std::move(transitionResult.exitRates), std::move(stateRewards), boost::optional<storm::storage::SparseMatrix<double>>(), boost::optional<std::vector<boost::container::flat_set<uint_fast64_t>>>());
            
            return resultingAutomaton;
        }
        
    } // namespace parser
} // namespace storm