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
				std::string encodeAsSmt2(const storm::models::Dtmc<T>& model, std::vector<carl::Variable> parameters, storm::storage::BitVector initialStates, storm::storage::BitVector finalStates, const typename T::CoeffType& threshold, bool lessequal = false)
				{
					carl::io::WriteTosmt2Stream smt2;
					uint_fast64_t nrStates = model.getNumberOfStates();
					carl::VariablePool& vpool = carl::VariablePool::getInstance();
					std::vector<carl::Variable> stateVars;
					for(carl::Variable p : parameters)
					{
						smt2 << ("parameter_bound_" + vpool.getName(p));
						smt2 << carl::io::smt2node::AND;
						smt2 << carl::Constraint<Polynomial>(Polynomial(p), carl::CompareRelation::GT);
						smt2 << carl::Constraint<Polynomial>(Polynomial(p) - Polynomial(1), carl::CompareRelation::LT);
						smt2 << carl::io::smt2node::CLOSENODE;
					}
					for(uint_fast64_t state = 0; state < nrStates-1; ++state)
					{
						
						carl::Variable stateVar = vpool.getFreshVariable("s_" + std::to_string(state));
						stateVars.push_back(stateVar);
						if(!finalStates[state])
						{
							smt2 << ("state_bound_" + std::to_string(state));
							smt2 << carl::io::smt2node::AND;
							smt2 << carl::Constraint<Polynomial>(Polynomial(stateVar), carl::CompareRelation::GT);
							smt2 << carl::Constraint<Polynomial>(Polynomial(stateVar) - Polynomial(1), carl::CompareRelation::LT);
							smt2 << carl::io::smt2node::CLOSENODE;
						}
						
					}
					
					smt2.setAutomaticLineBreaks(true);
					Polynomial initStateReachSum;
					for(uint_fast64_t state = 0; state < nrStates-1; ++state)
					{
						if(initialStates[state])
						{
							initStateReachSum += stateVars[state];
						}
						if(finalStates[state])
						{
							//smt2 << carl::Constraint<Polynomial>(Polynomial(stateVars[state]) - Polynomial(1), carl::CompareRelation::EQ);	
						}
						else
						{
							T reachpropPol(0);
							for(auto const& transition : model.getRows(state))
							{
								if(finalStates[transition.first])
								{
									reachpropPol += transition.second;
								}
								else if(transition.first == nrStates - 1)
								{
									// intentionally empty.
								}
								else
								{
									reachpropPol += transition.second * stateVars[transition.first];
								}
								
							}
							smt2 << ("transition_" + std::to_string(state));
							smt2 << carl::Constraint<T>(reachpropPol - stateVars[state], carl::CompareRelation::EQ);
						}
					}
					//smt2 << carl::Constraint<Polynomial>(Polynomial(stateVars[nrStates-1]), carl::CompareRelation::EQ);
					
					
					smt2 << ("reachability");
					
					carl::CompareRelation thresholdRelation = lessequal ? carl::CompareRelation::LE : carl::CompareRelation::GE;
					smt2 <<  carl::Constraint<Polynomial>(initStateReachSum - threshold, thresholdRelation);
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