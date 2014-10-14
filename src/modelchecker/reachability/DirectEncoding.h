/**
 * @file:   DirectEncoding.h
 * @author: Sebastian Junges
 *
 * @since April 8, 2014
 */

#pragma once

#ifdef STORM_HAVE_CARL
#include <carl/io/WriteTosmt2Stream.h>

namespace storm
{
    namespace modelchecker
    {
        namespace reachability
        {
            class DirectEncoding
            {
            public:
                template<typename T>
                std::string encodeAsSmt2(storm::storage::SparseMatrix<T> const& transitionMatrix, std::vector<T> const& oneStepProbabilities, std::vector<carl::Variable> const& parameters, storm::storage::BitVector const& initialStates, storm::storage::BitVector const& finalStates, typename T::CoeffType const& threshold, bool lessequal = false) {
                    
                    carl::io::WriteTosmt2Stream smt2;
                    uint_fast64_t numberOfStates = transitionMatrix.getRowCount();
                    carl::VariablePool& vpool = carl::VariablePool::getInstance();
                    std::vector<carl::Variable> stateVars;
                    for (carl::Variable const& p : parameters) {
                        smt2 << ("parameter_bound_" + vpool.getName(p));
                        smt2 << carl::io::smt2node::AND;
                        smt2 << carl::Constraint<Polynomial::PolyType>(Polynomial::PolyType(p), carl::CompareRelation::GT);
                        smt2 << carl::Constraint<Polynomial::PolyType>(Polynomial::PolyType(p) - Polynomial::PolyType(1), carl::CompareRelation::LT);
                        smt2 << carl::io::smt2node::CLOSENODE;
                    }
                    
                    for (uint_fast64_t state = 0; state < numberOfStates; ++state) {
                        carl::Variable stateVar = vpool.getFreshVariable("s_" + std::to_string(state));
                        stateVars.push_back(stateVar);
                        if (!finalStates[state]) {
                            smt2 << ("state_bound_" + std::to_string(state));
                            smt2 << carl::io::smt2node::AND;
                            smt2 << carl::Constraint<Polynomial::PolyType>(Polynomial::PolyType(stateVar), carl::CompareRelation::GT);
                            smt2 << carl::Constraint<Polynomial::PolyType>(Polynomial::PolyType(stateVar) - Polynomial::PolyType(1), carl::CompareRelation::LT);
                            smt2 << carl::io::smt2node::CLOSENODE;
                        }
                    }
                    
                    smt2.setAutomaticLineBreaks(true);
                    Polynomial::PolyType initStateReachSum;
                    for (uint_fast64_t state = 0; state < numberOfStates; ++state) {
                        T reachpropPol(0);
                        for (auto const& transition : transitionMatrix.getRow(state)) {
                            reachpropPol += transition.getValue() * stateVars[transition.getColumn()];
                        }
                        reachpropPol += oneStepProbabilities[state];
                        smt2 << ("transition_" + std::to_string(state));
                        smt2 << carl::Constraint<T>(reachpropPol - stateVars[state], carl::CompareRelation::EQ);
                    }
                    
                    smt2 << ("reachability");
                    
                    carl::CompareRelation thresholdRelation = lessequal ? carl::CompareRelation::LEQ : carl::CompareRelation::GEQ;
                    smt2 << carl::io::smt2node::OR;
                    for (uint_fast64_t state : initialStates) {
                        smt2 << carl::Constraint<Polynomial::PolyType>(Polynomial::PolyType(stateVars[state]) - threshold, thresholdRelation);
                    }
                    smt2 << carl::io::smt2node::CLOSENODE;
                    
                    smt2 << carl::io::smt2flag::CHECKSAT;
                    smt2 << carl::io::smt2flag::MODEL;
                    smt2 << carl::io::smt2flag::UNSAT_CORE;
                    std::stringstream strm;
                    strm << smt2;
                    return strm.str();
                }
            };
        }
    }
}

#endif