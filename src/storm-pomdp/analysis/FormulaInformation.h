#pragma once

#include <set>
#include <string>
#include <boost/optional.hpp>

#include "storm/storage/BitVector.h"
#include "storm/solver/OptimizationDirection.h"

namespace storm {
    
    namespace logic {
        class Formula;
    }
    
    namespace pomdp {
        namespace analysis {
            class FormulaInformation {
            public:
                /// Characterizes a certain set of states
                struct StateSet {
                    storm::storage::BitVector states; // The set of states
                    std::set<uint32_t> observations; // The set of the observations that are assigned to at least one state of the set
                    bool observationClosed; // True iff this state set can be uniquely characterized by the observations
                    bool empty() const;
                };
                
                /// Possible supported formula types
                enum class Type {
                    NonNestedReachabilityProbability, // e.g. 'Pmax=? [F "target"]' or 'Pmin=? [!"sink" U "target"]'
                    NonNestedExpectedRewardFormula, // e.g. 'Rmin=? [F x>0 ]'
                    Unsupported // The formula type is unsupported
                };
                
                FormulaInformation(); // Unsupported
                FormulaInformation(Type const& type, storm::solver::OptimizationDirection const& dir, boost::optional<std::string> const& rewardModelName = boost::none);
                
                Type const& getType() const;
                bool isNonNestedReachabilityProbability() const;
                bool isNonNestedExpectedRewardFormula() const;
                bool isUnsupported() const;
                StateSet const& getTargetStates() const;
                StateSet const& getSinkStates() const; // Shall not be called for reward formulas
                std::string const& getRewardModelName() const; // Shall not be called for probability formulas
                storm::solver::OptimizationDirection const& getOptimizationDirection() const;
                bool minimize() const;
                bool maximize() const;
                
                template <typename PomdpType>
                void updateTargetStates(PomdpType const& pomdp, storm::storage::BitVector&& newTargetStates);
                
                template <typename PomdpType>
                void updateSinkStates(PomdpType const& pomdp, storm::storage::BitVector&& newSinkStates);
                
            private:
                Type type;
                storm::solver::OptimizationDirection optimizationDirection;
                boost::optional<StateSet> targetStates;
                boost::optional<StateSet> sinkStates;
                boost::optional<std::string> rewardModelName;
            };
            
            template <typename PomdpType>
            FormulaInformation getFormulaInformation(PomdpType const& pomdp, storm::logic::Formula const& formula);

            
        }
    }
}
