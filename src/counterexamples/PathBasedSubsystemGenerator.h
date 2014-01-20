/*
 * PathBasedSubsystemGenerator.h
 *
 *  Created on: 11.10.2013
 *      Author: Manuel Sascha Weiand
 */

#ifndef STORM_COUNTEREXAMPLES_PATHBASEDSUBSYSTEMGENERATOR_H_
#define STORM_COUNTEREXAMPLES_PATHBASEDSUBSYSTEMGENERATOR_H_

#include "src/models/Dtmc.h"
#include "src/models/AbstractModel.h"
#include "src/modelchecker/prctl/SparseDtmcPrctlModelChecker.h"
#include "src/solver/GmmxxLinearEquationSolver.h"
#include "src/storage/BitVector.h"
#include "src/storage/SparseMatrix.h"

#include "log4cplus/logger.h"
#include "log4cplus/loggingmacros.h"

extern log4cplus::Logger logger;

namespace storm {
    namespace counterexamples {

    template <class T>
    class PathBasedSubsystemGenerator {

private:

	template <class CompareType>
	class CompareStates {
	public:
		bool operator()(const std::pair<uint_fast64_t, CompareType>& s1, const std::pair<uint_fast64_t, CompareType>& s2) {
			return s1.second < s2.second;
		}
	};

public:

	/*!
	 *
	 */
	static void computeShortestDistances(storm::storage::SparseMatrix<T> const& transMat, storm::storage::BitVector& subSysStates, storm::storage::BitVector& terminalStates, storm::storage::BitVector& allowedStates, std::vector<std::pair<uint_fast64_t, T>>& distances) {

		std::multiset<std::pair<uint_fast64_t, T>, CompareStates<T> > activeSet;
		//std::priority_queue<std::pair<uint_fast64_t, T*>, std::vector<std::pair<uint_fast64_t, T*>>, CompareStates<T> > queue;

		// resize and init distances
		const std::pair<uint_fast64_t, T> initDistances(0, (T) -1);
		distances.resize(transMat.getColumnCount(), initDistances);

		//since gcc 4.7.2 does not implement std::set::emplace(), insert is used.
		std::pair<uint_fast64_t, T> state;

		// First store all transitions from initial states
		// Also save all found initial states in array of discovered states.
		for(auto init : subSysStates) {
			//use init state only if it is allowed
			if(allowedStates.get(init)) {

				if(terminalStates.get(init)) {
						// it's an init -> target search
						// save target state as discovered and get it's outgoing transitions

						distances[init].first = init;
						distances[init].second = (T) 1;
				}

				for(auto const& trans : transMat.getRow(init)) {
					//save transition only if it's no 'virtual transition of prob 0 and it doesn't go from init state to init state.
					if(trans.second != (T) 0 && !subSysStates.get(trans.first)) {
						//new state?
						if(distances[trans.first].second == (T) -1) {
							distances[trans.first].first = init;
							distances[trans.first].second = trans.second;

							activeSet.insert(std::pair<uint_fast64_t, T>(trans.first, distances[trans.first].second));
						}
						else if(distances[trans.first].second < trans.second){
							//This state has already been discovered
							//And the distance can be improved by using this transition.

							//find state in set, remove it, reenter it with new and correct values.
							auto range = activeSet.equal_range(std::pair<uint_fast64_t, T>(trans.first, distances[trans.first].second));
							for(;range.first != range.second; range.first++) {
								if(trans.first == range.first->first) {
									activeSet.erase(range.first);
									break;
								}
							}

							distances[trans.first].first = init;
							distances[trans.first].second = trans.second;

							activeSet.insert(std::pair<uint_fast64_t, T>(trans.first, trans.second));
						}
					}
				}
			}
		}

		LOG4CPLUS_DEBUG(logger, "Initialized.");

		//Now find the shortest distances to all states
		while(!activeSet.empty()) {

			// copy here since using a reference leads to segfault
			std::pair<uint_fast64_t, T> activeState = *(--activeSet.end());
			activeSet.erase(--activeSet.end());

			// If this is an initial state, do not consider its outgoing transitions, since all relevant ones have already been considered
			// Same goes for forbidden states since they may not be used on a path, except as last node.
			if(!subSysStates.get(activeState.first) && allowedStates.get(activeState.first)) {
				// Look at all neighbors
				for(auto const& trans : transMat.getRow(activeState.first)) {
					// Only consider the transition if it's not virtual
					if(trans.second != (T) 0) {

						T distance = activeState.second * trans.second;

						//not discovered or initial terminal state
						if(distances[trans.first].second == (T)-1) {
							//New state discovered -> save it
							distances[trans.first].first = activeState.first;
							distances[trans.first].second = distance;

							// push newly discovered state into activeSet
							activeSet.insert(std::pair<uint_fast64_t, T>(trans.first, distance));
						}
						else if(distances[trans.first].second < distance ){
							//This state has already been discovered
							//And the distance can be improved by using this transition.

							//find state in set, remove it, reenter it with new and correct values.

							auto range = activeSet.equal_range(std::pair<uint_fast64_t, T>(trans.first, distances[trans.first].second));
							for(;range.first != range.second; range.first++) {
								if(trans.first == range.first->first) {
									activeSet.erase(range.first);
									break;
								}
							}

							distances[trans.first].first = activeState.first;
							distances[trans.first].second = distance;
							activeSet.insert(std::pair<uint_fast64_t, T>(trans.first, distance));
						}
					}
				}
			}
		}

		LOG4CPLUS_DEBUG(logger, "Discovery done.");
	}

	/*!
	 *
	 */
	static void doDijkstraSearch(storm::storage::SparseMatrix<T> const& transMat, storm::storage::BitVector& subSysStates, storm::storage::BitVector& terminalStates, storm::storage::BitVector& allowedStates, std::vector<std::pair<uint_fast64_t, T>>& itDistances, std::vector<std::pair<uint_fast64_t, T>>& distances) {
		std::multiset<std::pair<uint_fast64_t, T>, CompareStates<T> > activeSet;

		// resize and init distances
		const std::pair<uint_fast64_t, T> initDistances(0, (T) -1);
		distances.resize(transMat.getColumnCount(), initDistances);

		//since gcc 4.7.2 does not implement std::set::emplace(), insert is used.
		std::pair<uint_fast64_t, T> state;

		// First store all transitions from initial states
		// Also save all found initial states in array of discovered states.
		for(auto init : subSysStates) {
			//use init state only if it is allowed
			if(allowedStates.get(init)) {

				if(terminalStates.get(init)) {
						// it's a subsys -> subsys search
						// ignore terminal state completely
						// (since any target state that is only reached by a path going through this state should not be reached)
						continue;
				}

				for(auto const& trans : transMat.getRow(init)) {
					//save transition only if it's no 'virtual transition of prob 0 and it doesn't go from init state to init state.
					if(trans.second != (T) 0 && !subSysStates.get(trans.first)) {
						//new state?
						if(distances[trans.first].second == (T) -1) {
							//for initialization of subsys -> subsys search use prob (init -> subsys state -> found state) instead of prob(subsys state -> found state)
							distances[trans.first].first = init;
							distances[trans.first].second = trans.second * (itDistances[init].second == -1 ? 1 : itDistances[init].second);

							activeSet.insert(std::pair<uint_fast64_t, T>(trans.first, distances[trans.first].second));
						}
						else if(distances[trans.first].second < trans.second * itDistances[init].second){
							//This state has already been discovered
							//And the distance can be improved by using this transition.

							//find state in set, remove it, reenter it with new and correct values.
							auto range = activeSet.equal_range(std::pair<uint_fast64_t, T>(trans.first, distances[trans.first].second));
							for(;range.first != range.second; range.first++) {
								if(trans.first == range.first->first) {
									activeSet.erase(range.first);
									break;
								}
							}

							//for initialization of subsys -> subsys search use prob (init -> subsys state -> found state) instead of prob(subsys state -> found state)
							distances[trans.first].first = init;
							distances[trans.first].second = trans.second * (itDistances[init].second == -1 ? 1 : itDistances[init].second);

							activeSet.insert(std::pair<uint_fast64_t, T>(trans.first, trans.second));
						}
					}
				}
			}
		}

		LOG4CPLUS_DEBUG(logger, "Initialized.");

		//Now find the shortest distances to all states
		while(!activeSet.empty()) {

			// copy here since using a reference leads to segfault
			std::pair<uint_fast64_t, T> activeState = *(--activeSet.end());
			activeSet.erase(--activeSet.end());

			// Always stop at first target/terminal state
			//if(terminalStates.get(activeState.first) || subSysStates.get(activeState.first)) break;

			// If this is an initial state, do not consider its outgoing transitions, since all relevant ones have already been considered
			// Same goes for forbidden states since they may not be used on a path, except as last node.
			if(!subSysStates.get(activeState.first) && allowedStates.get(activeState.first)) {
				// Look at all neighbors
				for(auto const& trans : transMat.getRow(activeState.first)) {
					// Only consider the transition if it's not virtual
					if(trans.second != (T) 0) {

						T distance = activeState.second * trans.second;

						//not discovered or initial terminal state
						if(distances[trans.first].second == (T)-1) {
							//New state discovered -> save it
							distances[trans.first].first = activeState.first;
							distances[trans.first].second = distance;

							// push newly discovered state into activeSet
							activeSet.insert(std::pair<uint_fast64_t, T>(trans.first, distance));
						}
						else if(distances[trans.first].second < distance ){
							//This state has already been discovered
							//And the distance can be improved by using this transition.

							//find state in set, remove it, reenter it with new and correct values.

							auto range = activeSet.equal_range(std::pair<uint_fast64_t, T>(trans.first, distances[trans.first].second));
							for(;range.first != range.second; range.first++) {
								if(trans.first == range.first->first) {
									activeSet.erase(range.first);
									break;
								}
							}

							distances[trans.first].first = activeState.first;
							distances[trans.first].second = distance;
							activeSet.insert(std::pair<uint_fast64_t, T>(trans.first, distance));
						}
					}
				}
			}
		}

		LOG4CPLUS_DEBUG(logger, "Discovery done.");
	}

	/*!
	 *
	 *//*
	template <typename T>
	static void doBackwardsSearch(storm::storage::SparseMatrix<T> const& transMat, storm::storage::BitVector& initStates, storm::storage::BitVector& subSysStates, storm::storage::BitVector& terminalStates, storm::storage::BitVector& allowedStates, std::vector<T>& probabilities, std::vector<uint_fast64_t>& shortestPath, T& probability) {
		std::multiset<std::pair<uint_fast64_t, T>, CompareStates<T> > activeSet;

		// resize and init distances
		const std::pair<uint_fast64_t, T> initDistances(0, (T) -1);
		std::vector<std::pair<uint_fast64_t, T>> distances(transMat.getColumnCount(), initDistances);

		//since the transition matrix only gives a means to iterate over successors and not over predecessors and there is no Transpose for the matrix
		//GraphTransitions is used
		storm::models::GraphTransitions<T> backTrans(transMat, false);

		//First store all allowed predecessors of target states that are not in the subsystem
		for(storm::storage::BitVector::constIndexIterator target = terminalStates.begin(); target != terminalStates.end(); ++target) {

			// if there is a terminal state that is an initial state then prob == 1 and return
			if(initStates.get(*target)){
				distances[*target].first = *target;
				distances[*target].second = (T) 1;
				return;
			}

			//iterate over predecessors
			for(auto iter = backTrans.beginStateSuccessorsIterator(*target); iter != backTrans.endStateSuccessorsIterator(*target); iter++) {
				//only use if allowed and not in subsys and not terminal
				if(allowedStates.get(*iter) && !subSysStates.get(*iter) && !terminalStates.get(*iter)) {
					//new state?
					if(distances[*iter].second == (T) -1) {
						// save as discovered and push into active set
						distances[*iter].first = *target;												//successor
						distances[*iter].second = transMat.getValue(*iter, *target);					//prob of shortest path

						activeSet.insert(std::pair<uint_fast64_t, T>(*iter, probabilities[*iter]));		//prob of reaching some terminal state from pred.
					}
					else {
						// state was already discovered
						// is this the better transition?
						if(distances[*iter].second > transMat.getValue(*iter, *target)) {
							distances[*iter].first = *target;
							distances[*iter].second = transMat.getValue(*iter, *target);
						}
					}
				}
			}
		}

		//Now store all allowed predecessors of subsystem states that are not subsystem states themselves
		for(storm::storage::BitVector::constIndexIterator sysState = subSysStates.begin(); sysState != subSysStates.end(); ++sysState) {
			//iterate over predecessors
			for(auto iter = backTrans.beginStateSuccessorsIterator(*sysState); iter != backTrans.endStateSuccessorsIterator(*sysState); iter++) {
				//only use if allowed and not in subsys and not terminal
				if(allowedStates.get(*iter) && !subSysStates.get(*iter) && !terminalStates.get(*iter)) {
					//new state?
					if(distances[*iter].second == (T) -1) {
						// save as discovered and push into active set
						distances[*iter].first = *sysState;												//successor
						distances[*iter].second = transMat.getValue(*iter, *sysState);					//prob of shortest path

						activeSet.insert(std::pair<uint_fast64_t, T>(*iter, probabilities[*iter]));		//prob of reaching some terminal state from pred.
					}
					else {
						// state was already discovered
						// is this the better transition?
						if(distances[*iter].second > transMat.getValue(*iter, *sysState)) {
							distances[*iter].first = *sysState;
							distances[*iter].second = transMat.getValue(*iter, *sysState);
						}
					}
				}
			}
		}

		LOG4CPLUS_DEBUG(logger, "Initialized.");

		// Do the backwards search
		std::pair<uint_fast64_t, T> state;
		uint_fast64_t activeState;
		while(!activeSet.empty()) {
			// copy here since using a reference leads to segfault
			state = *(--activeSet.end());
			activeState = state.first;
			activeSet.erase(--activeSet.end());

			//stop on the first subsys/init state
			if(initStates.get(activeState) || subSysStates.get(activeState)) break;

			// If this is a subSys or terminal state, do not consider its incoming transitions, since all relevant ones have already been considered
			if(!terminalStates.get(activeState) && !subSysStates.get(activeState)) {
				//iterate over predecessors
				for(auto iter = backTrans.beginStateSuccessorsIterator(activeState); iter != backTrans.endStateSuccessorsIterator(activeState); iter++) {
					//only if transition is not "virtual" and no selfloop
					if(*iter != activeState && transMat.getValue(*iter, activeState) != (T) 0) {
						//new state?
						if(distances[*iter].second == (T) -1) {
							// save as discovered and push into active set
							distances[*iter].first = activeState;
							distances[*iter].second = transMat.getValue(*iter, activeState) * distances[activeState].second;

							activeSet.insert(std::pair<uint_fast64_t, T>(*iter, probabilities[*iter]));
						}
						else {
							// state was already discovered
							// is this the better transition?
							if(distances[*iter].second < transMat.getValue(*iter, activeState) * distances[activeState].second) {
								distances[*iter].first = activeState;
								distances[*iter].second = transMat.getValue(*iter, activeState) * distances[activeState].second;
							}
						}
					}
				}
			}
		}

		//get path probability
		probability = distances[activeState].second;
		if(probability == (T) -1) probability = 1;

		// iterate over the successors until reaching the end of the finite path
		shortestPath.push_back(activeState);
		activeState = distances[activeState].first;
		while(!terminalStates.get(activeState) && !subSysStates.get(activeState)) {
			shortestPath.push_back(activeState);
			activeState = distances[activeState].first;
		}
		shortestPath.push_back(activeState);
	}

	*/

	/*!
	 *
	 */
	static void findShortestPath(storm::storage::SparseMatrix<T> const& transMat, storm::storage::BitVector& subSysStates, storm::storage::BitVector& terminalStates, storm::storage::BitVector& allowedStates, std::vector<std::pair<uint_fast64_t, T>>& itDistances, std::vector<uint_fast64_t>& shortestPath, T& probability) {

		//Do Dijksta to find the shortest path from init states to all states
		std::vector<std::pair<uint_fast64_t, T>> distances;
		doDijkstraSearch(transMat, subSysStates, terminalStates, allowedStates, itDistances, distances);

		// Then get the shortest of them
		extractShortestPath(subSysStates, terminalStates, distances, itDistances, shortestPath, probability,false);
	}

	/*!
	 * Only initialized distances vector!
	 */
	static void extractShortestPath(storm::storage::BitVector& subSysStates, storm::storage::BitVector& terminalStates, std::vector<std::pair<uint_fast64_t, T>>& distances, std::vector<std::pair<uint_fast64_t, T>>& itDistances, std::vector<uint_fast64_t>& shortestPath, T& probability, bool stopAtFirstTarget = true, bool itSearch = false) {
		//Find terminal state of best distance
		uint_fast64_t bestIndex = 0;
		T bestValue = (T) 0;

		for(auto term : terminalStates) {

			//the terminal state might not have been found if it is in a system of forbidden states
			if(distances[term].second != -1 && distances[term].second > bestValue){
				bestIndex = term;
				bestValue = distances[term].second;

				//if set, stop since the first target that is not null was the only one found
				if(stopAtFirstTarget) break;
			}
		}

		if(!itSearch) {
			// it's a subSys->subSys search. So target states are terminals and subsys states
			for(auto term : subSysStates) {

				//the terminal state might not have been found if it is in a system of forbidden states
				if(distances[term].second != -1 && distances[term].second > bestValue){
					bestIndex = term;
					bestValue = distances[term].second;

					//if set, stop since the first target that is not null was the only one found
					if(stopAtFirstTarget) break;
				}
			}
		}

		//safety test: is the best terminal state viable?
		if(distances[bestIndex].second == (T) -1){
			shortestPath.push_back(bestIndex);
			probability = (T) 0;
			LOG4CPLUS_DEBUG(logger, "Terminal state not viable!");
			return;
		}

		// save the probability to reach the state via the shortest path
		probability = distances[bestIndex].second;

		//Reconstruct shortest path. Notice that the last state of the path might be an initState.

		//There might be a chain of terminal states with 1.0 transitions at the end of the path
		while(terminalStates.get(distances[bestIndex].first)) {
			bestIndex = distances[bestIndex].first;
		}

		LOG4CPLUS_DEBUG(logger, "Found best state: " << bestIndex);
		LOG4CPLUS_DEBUG(logger, "Value: " << bestValue);

		shortestPath.push_back(bestIndex);
		bestIndex = distances[bestIndex].first;
		while(!subSysStates.get(bestIndex)) {
			shortestPath.push_back(bestIndex);
			bestIndex = distances[bestIndex].first;
		}
		shortestPath.push_back(bestIndex);

		//At last compensate for the distance between init and source state
		probability = itSearch ? probability : probability / itDistances[bestIndex].second;
	}

private:

	template <typename Type>
	struct transition {
		uint_fast64_t source;
		Type prob;
		uint_fast64_t target;
	};

	template <class CompareType>
	class CompareTransitions {
	public:
		bool operator()(transition<CompareType>& t1, transition<CompareType>& t2) {
			return t1.prob < t2.prob;
		}
	};

public:

	/*!
	 *
	 */
	static storm::models::Dtmc<T> computeCriticalSubsystem(storm::models::Dtmc<T> & model, storm::property::prctl::AbstractStateFormula<T> const& stateFormula) {

		//-------------------------------------------------------------
		// 1. Strip and handle formulas
		//-------------------------------------------------------------

#ifdef BENCHMARK
		LOG4CPLUS_INFO(logger, "Formula: " << stateFormula.toString());
#endif
		LOG4CPLUS_INFO(logger, "Start finding critical subsystem.");

		// make model checker
		// TODO: Implement and use generic Model Checker factory.
		storm::modelchecker::prctl::SparseDtmcPrctlModelChecker<T> modelCheck {model, new storm::solver::GmmxxLinearEquationSolver<T>()};

		// init bit vector to contain the subsystem
		storm::storage::BitVector subSys(model.getNumberOfStates());

		storm::property::prctl::AbstractPathFormula<T> const* pathFormulaPtr;
		T bound = 0;

		// Strip bound operator
		storm::property::prctl::ProbabilisticBoundOperator<T> const* boundOperator = dynamic_cast<storm::property::prctl::ProbabilisticBoundOperator<T> const*>(&stateFormula);

		if(boundOperator == nullptr){
			LOG4CPLUS_ERROR(logger, "No path bound operator at formula root.");
			return model.getSubDtmc(subSys);
		}
		bound = boundOperator->getBound();

		storm::property::prctl::AbstractPathFormula<T> const& abstractPathFormula = boundOperator->getPathFormula();
		pathFormulaPtr = &abstractPathFormula;

		// get "init" labeled states
		storm::storage::BitVector initStates = model.getLabeledStates("init");

		//get real prob for formula
		logger.getAppender("mainFileAppender")->setThreshold(log4cplus::WARN_LOG_LEVEL);
		std::vector<T> trueProbs = pathFormulaPtr->check(modelCheck, false);
		logger.getAppender("mainFileAppender")->setThreshold(log4cplus::INFO_LOG_LEVEL);

		T trueProb = 0;
		for(auto index : initStates) {
			trueProb += trueProbs[index];
		}
		trueProb /= initStates.getNumberOfSetBits();
		//std::cout << "True Prob: " << trueProb << std::endl;

		// get allowed and target states
		storm::storage::BitVector allowedStates;
		storm::storage::BitVector targetStates;

		storm::property::prctl::Eventually<T> const* eventually = dynamic_cast<storm::property::prctl::Eventually<T> const*>(pathFormulaPtr);
		storm::property::prctl::Globally<T> const* globally = dynamic_cast<storm::property::prctl::Globally<T> const*>(pathFormulaPtr);
		storm::property::prctl::Until<T> const* until = dynamic_cast<storm::property::prctl::Until<T> const*>(pathFormulaPtr);
		if(eventually != nullptr) {
			targetStates = eventually->getChild().check(modelCheck);
			allowedStates = storm::storage::BitVector(targetStates.size(), true);
		}
		else if(globally != nullptr){
			//eventually reaching a state without property visiting only states with property
			allowedStates = globally->getChild().check(modelCheck);
			targetStates = storm::storage::BitVector(allowedStates);
			targetStates.complement();
		}
		else if(until != nullptr) {
			allowedStates = until->getLeft().check(modelCheck);
			targetStates = until->getRight().check(modelCheck);
		}
		else {
			LOG4CPLUS_ERROR(logger, "Strange path formula. Can't decipher.");
			return model.getSubDtmc(subSys);
		}

		//-------------------------------------------------------------
		// 2. Precomputations for heuristics
		//-------------------------------------------------------------

		// estimate the path count using the models state count as well as the probability bound
		uint_fast8_t const minPrec = 10;
		uint_fast64_t const stateCount = model.getNumberOfStates();
		uint_fast64_t const stateEstimate = ((T) stateCount) * bound;

		//since this only has a good effect on big models -> use only if model has at least 10^5 states
		uint_fast64_t precision = stateEstimate > 100000 ? stateEstimate/1000 : minPrec;



		//-------------------------------------------------------------
		// 3. Subsystem generation
		//-------------------------------------------------------------

		// Search from init to target states until the shortest path for each target state is reached.
		std::vector<uint_fast64_t> shortestPath;
		std::vector<T> subSysProbs;
		T pathProb = 0;
		T subSysProb = 0;
		uint_fast64_t pathCount = 0;
		uint_fast64_t mcCount = 0;

		// First test if there are init states that are also target states.
		// If so the init state represents a subsystem with probability mass 1.
		// -> return it
		if((initStates & targetStates).getNumberOfSetBits() != 0) {
			subSys.set(*(initStates & targetStates).begin());

			LOG4CPLUS_INFO(logger, "Critical subsystem found.");
			LOG4CPLUS_INFO(logger, "Paths needed: " << pathCount);
			LOG4CPLUS_INFO(logger, "State count of critical subsystem: " << subSys.getNumberOfSetBits());
			LOG4CPLUS_INFO(logger, "Prob: " << 1);
			LOG4CPLUS_INFO(logger, "Model checks: " << mcCount);

			return model.getSubDtmc(subSys);
		}


		// Then compute the shortest paths from init states to all target states
		std::vector<std::pair<uint_fast64_t, T>> initTargetDistances;
		computeShortestDistances(model.getTransitionMatrix(), initStates, targetStates, allowedStates, initTargetDistances);

		pathProb = 0;
		extractShortestPath(initStates, targetStates, initTargetDistances, initTargetDistances, shortestPath, pathProb, false, true);

		// push states of found path into subsystem
		for(auto index : shortestPath) {
			subSys.set(index, true);
		}
		pathCount++;

		// Get estimate (upper bound) of new sub system probability
		// That is: prob(target) * cost(path) * (mean(prob(inits))/prob(source))
		subSysProb += trueProbs[shortestPath[0]] * pathProb * trueProb / trueProbs[shortestPath.back()];

		//find new nodes until the system becomes critical.
		while(true) {
			shortestPath.clear();
			pathProb = 0;
			findShortestPath(model.getTransitionMatrix(), subSys, targetStates, allowedStates, initTargetDistances, shortestPath, pathProb);
			//doBackwardsSearch(*model->getTransitionMatrix(), *initStates, subSys, targetStates, allowedStates, trueProbs, shortestPath, pathProb);

			pathCount++;

			// merge found states into subsystem
			for(uint_fast32_t i = 0; i < shortestPath.size(); i++) {
				subSys.set(shortestPath[i], true);
			}

			// Get estimate (upper bound) of new sub system probability
			// That is: prob(target) * cost(path) * (mean(prob(inits))/prob(source))
			//subSysProb += (trueProbs[shortestPath.back()] == 0 ? 0 : trueProbs[shortestPath[0]] * pathProb * trueProb / trueProbs[shortestPath.back()]);
			subSysProb += 1*(trueProbs[shortestPath.back()] == 0 ? 1 : trueProbs[shortestPath[0]] * pathProb == 0 ? 1 : pathProb );
			//std::cout << "Est. prob: " << subSysProb << std::endl;
			// Do we want to model check?
			if((pathCount % precision == 0) && subSysProb >= bound) {

				//get probabilities
				logger.getAppender("mainFileAppender")->setThreshold(log4cplus::WARN_LOG_LEVEL);
				subSysProbs = modelCheck.checkUntil(allowedStates & subSys, targetStates & subSys, false);
				logger.getAppender("mainFileAppender")->setThreshold(log4cplus::INFO_LOG_LEVEL);

				//T diff = subSysProb;
				//std::cout << "Est. prob: " << diff << std::endl;

				// reset sub system prob to correct value
				subSysProb = 0;
				for(auto index : initStates) {
					subSysProb += subSysProbs[index];
				}
				subSysProb /= initStates.getNumberOfSetBits();

				mcCount++;
				//diff -= subSysProb;
				//std::cout << "Real prob: " << subSysProb << std::endl;
				//std::cout << "Diff: " << diff << std::endl;
				//std::cout << "Path count: " << pathCount << std::endl;

				//Are we critical?
				if(subSysProb >= bound){
					break;
				}
				else if (stateEstimate > 100000){
					precision = (stateEstimate/1000) - ((stateEstimate/1000) - minPrec)*(subSysProb/bound);
				}
			}
		}

		LOG4CPLUS_INFO(logger, "Critical subsystem found.");
		LOG4CPLUS_INFO(logger, "Paths needed: " << pathCount);
		LOG4CPLUS_INFO(logger, "State count of critical subsystem: " << subSys.getNumberOfSetBits());
		LOG4CPLUS_INFO(logger, "Prob: " << subSysProb);
		LOG4CPLUS_INFO(logger, "Model checks: " << mcCount);

		return model.getSubDtmc(subSys);
	}

    };

    } // namespace counterexamples
} // namespace storm

#endif /* STORM_COUNTEREXAMPLES_PATHBASEDSUBSYSTEMGENERATOR_H_ */
