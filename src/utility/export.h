/** 
 * @file:   export.h
 * @author: Sebastian Junges
 *
 * @since October 7, 2014
 */

#ifndef STORM_UTILITY_EXPORT_H_
#define STORM_UTILITY_EXPORT_H_

#include <iostream>

#include "src/storage/parameters.h"
#include "src/settings/modules/ParametricSettings.h"
#include "src/modelchecker/reachability/CollectConstraints.h"

namespace storm {
namespace utility {

template<typename ValueType>	
void exportParametricMcResult(const ValueType& mcresult, storm::modelchecker::reachability::CollectConstraints<storm::RationalFunction> const& constraintCollector) {
	std::string path = storm::settings::parametricSettings().exportResultPath();
	std::ofstream filestream;
	filestream.open(path);
	// todo add checks.
	filestream << "!Parameters: ";
	std::set<storm::Variable> vars = mcresult.gatherVariables();
	std::copy(vars.begin(), vars.end(), std::ostream_iterator<storm::Variable>(filestream, " "));
	filestream << std::endl;
	filestream << "!Result: " << mcresult << std::endl;
	filestream << "!Well-formed Constraints: " << std::endl;
	std::copy(constraintCollector.wellformedConstraints().begin(), constraintCollector.wellformedConstraints().end(), std::ostream_iterator<carl::Constraint<ValueType>>(filestream, "\n"));
	filestream << "!Graph-preserving Constraints: " << std::endl;
	std::copy(constraintCollector.graphPreservingConstraints().begin(), constraintCollector.graphPreservingConstraints().end(), std::ostream_iterator<carl::Constraint<ValueType>>(filestream, "\n"));
	filestream.close();
}

void exportStringStreamToFile(std::string const& str, std::string filepath) {
	// todo add checks.
	std::ofstream filestream;
	filestream.open(filepath);
	filestream << str;
	filestream.close();
}

}
}



#endif