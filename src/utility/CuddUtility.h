/*
 * CuddUtility.h
 *
 *  Created on: 26.01.2013
 *      Author: Christian Dehnert
 */

#ifndef STORM_UTILITY_CUDDUTILITY_H_
#define STORM_UTILITY_CUDDUTILITY_H_

#include "cuddObj.hh"

namespace storm {

namespace utility {

class CuddUtility {
public:
	~CuddUtility() {
		for (auto element : allDecisionDiagramVariables) {
			delete element;
		}
	}

	ADD* getNewAddVariable();
	ADD* getAddVariable(int index) const;

	ADD* getConstantEncoding(uint_fast64_t constant, std::vector<ADD*>& variables) const;

	ADD* addValueForEncodingOfConstant(ADD* add, uint_fast64_t constant, std::vector<ADD*>& variables, double value) const;

	ADD* getConstant(double value) const;

	void dumpDotToFile(ADD* add, std::string filename) const;

	Cudd const& getManager() const;

	friend CuddUtility* cuddUtilityInstance();

private:
	CuddUtility() : manager(0, 0), allDecisionDiagramVariables() {

	}

	Cudd manager;
	std::vector<ADD*> allDecisionDiagramVariables;

	static CuddUtility* instance;
};

CuddUtility* cuddUtilityInstance();

} // namespace utility

} // namespace storm

#endif /* STORM_UTILITY_CUDDUTILITY_H_ */
