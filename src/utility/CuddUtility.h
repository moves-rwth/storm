/*
 * CuddUtility.h
 *
 *  Created on: 26.01.2013
 *      Author: Christian Dehnert
 */

#ifndef STORM_UTILITY_CUDDUTILITY_H_
#define STORM_UTILITY_CUDDUTILITY_H_

#include "cuddObj.hh"

#include <cstdint>

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

	ADD* getOne() const;
	ADD* getZero() const;

	ADD* getConstantEncoding(uint_fast64_t constant, std::vector<ADD*> const& variables) const;

	void setValueAtIndex(ADD* add, uint_fast64_t index, std::vector<ADD*> const& variables, double value) const;
	void setValueAtIndices(ADD* add, uint_fast64_t rowIndex, uint_fast64_t columnIndex, std::vector<ADD*> const& rowVariables, std::vector<ADD*> const& columnVariables, double value) const;

	ADD* getConstant(double value) const;

	ADD* permuteVariables(ADD* add, std::vector<ADD*> fromVariables, std::vector<ADD*> toVariables, uint_fast64_t totalNumberOfVariables) const;

	void dumpDotToFile(ADD* add, std::string filename) const;

	Cudd const& getManager() const;

	friend CuddUtility* cuddUtilityInstance();

private:
	CuddUtility() : manager(), allDecisionDiagramVariables() {

	}

	Cudd manager;
	std::vector<ADD*> allDecisionDiagramVariables;

	static CuddUtility* instance;
};

CuddUtility* cuddUtilityInstance();

} // namespace utility

} // namespace storm

#endif /* STORM_UTILITY_CUDDUTILITY_H_ */
