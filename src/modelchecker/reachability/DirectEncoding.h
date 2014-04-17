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
				void encodeAsSmt2(const storm::models::Dtmc<T>& model, storm::storage::BitVector finalStates, T threshold, bool lessequal = true)
				{
					carl::io::WriteTosmt2Stream smt2;
					uint_fast64_t nrStates = model.getNumberOfStates();
					carl::VariablePool& vpool = carl::VariablePool::getInstance();
					std::vector<carl::Variable> stateVars;
					for(uint_fast64_t state = 0; state < nrStates; ++state)
					{
						carl::Variable stateVar = vpool.getFreshVariable("s_" + std::to_string(state));
						stateVars.push_back(stateVar);
						smt2 << carl::io::smt2flag::ASSERT;
						smt2 << carl::io::smt2node::AND;
						smt2 << carl::Constraint<Polynomial>(Polynomial(stateVar), carl::CompareRelation::GE);
						smt2 << carl::Constraint<Polynomial>(Polynomial(stateVar) - Polynomial(1), carl::CompareRelation::LE);
						smt2 << carl::io::smt2node::CLOSENODE;
					}
					smt2 << carl::io::smt2flag::ASSERT;
					smt2 << carl::io::smt2node::AND;
					smt2.setAutomaticLineBreaks(true);
					Polynomial finalStateReachSum;
					for(uint_fast64_t state = 0; state < nrStates; ++state)
					{
						if(finalStates[state])
						{
							smt2 << carl::Constraint<Polynomial>(Polynomial(stateVars[state]) - Polynomial(1), carl::CompareRelation::EQ);	
							finalStateReachSum += stateVars[state];
						}
						else
						{
							Polynomial reachpropPol(0);
							for(auto const& transition : model.getRows(state))
							{
								reachpropPol += stateVars[transition.first] * transition.second;
							}
							smt2 << carl::Constraint<Polynomial>(reachpropPol - stateVars[state], carl::CompareRelation::EQ);
						}
					}
					smt2 << carl::io::smt2node::CLOSENODE;
					smt2 << carl::io::smt2flag::ASSERT;
					
					carl::CompareRelation thresholdRelation = lessequal ? carl::CompareRelation::LE : carl::CompareRelation::GE;
					smt2 <<  carl::Constraint<Polynomial>(finalStateReachSum - threshold, thresholdRelation);
					smt2 << carl::io::smt2flag::CHECKSAT;
					std::cout << smt2;
	
				}
			};
		}
	}
}

#endif