/*
 * CslParser.cpp
 *
 *  Created on: 08.04.2013
 *      Author: Thomas Heinemann
 */

#include "src/parser/PrctlParser.h"
#include "src/utility/OsDetection.h"
#include "src/utility/ConstTemplates.h"

// If the parser fails due to ill-formed data, this exception is thrown.
#include "src/exceptions/WrongFormatException.h"

// Used for Boost spirit.
#include <boost/typeof/typeof.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix.hpp>

// Include headers for spirit iterators. Needed for diagnostics and input stream iteration.
#include <boost/spirit/include/classic_position_iterator.hpp>
#include <boost/spirit/include/support_multi_pass.hpp>

// Needed for file IO.
#include <fstream>
#include <iomanip>
#include <map>


// Some typedefs and namespace definitions to reduce code size.
typedef std::string::const_iterator BaseIteratorType;
typedef boost::spirit::classic::position_iterator2<BaseIteratorType> PositionIteratorType;
namespace qi = boost::spirit::qi;
namespace phoenix = boost::phoenix;

#include "CslParser.h"


namespace storm {
namespace parser {

template<typename Iterator, typename Skipper>
struct CslParser::CslGrammar : qi::grammar<Iterator, storm::formula::AbstractFormula<double>*(), Skipper > {
	CslGrammar() : CslGrammar::base_type(start) {
		freeIdentifierName = qi::lexeme[+(qi::alpha | qi::char_('_'))];

		//This block defines rules for parsing state formulas
		stateFormula %= orFormula;
		stateFormula.name("state formula");
		orFormula = andFormula[qi::_val = qi::_1] > *(qi::lit("|") > andFormula)[qi::_val =
				phoenix::new_<storm::formula::Or<double>>(qi::_val, qi::_1)];
		orFormula.name("state formula");
		andFormula = notFormula[qi::_val = qi::_1] > *(qi::lit("&") > notFormula)[qi::_val =
				phoenix::new_<storm::formula::And<double>>(qi::_val, qi::_1)];
		andFormula.name("state formula");
		notFormula = atomicStateFormula[qi::_val = qi::_1] | (qi::lit("!") > atomicStateFormula)[qi::_val =
				phoenix::new_<storm::formula::Not<double>>(qi::_1)];
		notFormula.name("state formula");

		//This block defines rules for "atomic" state formulas
		//(Propositions, probabilistic/reward formulas, and state formulas in brackets)
		atomicStateFormula %= probabilisticBoundOperator | steadyStateBoundOperator | atomicProposition | qi::lit("(") >> stateFormula >> qi::lit(")");
		atomicStateFormula.name("state formula");
		atomicProposition = (freeIdentifierName)[qi::_val =
				phoenix::new_<storm::formula::Ap<double>>(qi::_1)];
		atomicProposition.name("state formula");
		probabilisticBoundOperator = (
				(qi::lit("P") >> qi::lit(">") >> qi::double_ > qi::lit("[") > pathFormula > qi::lit("]"))[qi::_val =
						phoenix::new_<storm::formula::ProbabilisticBoundOperator<double> >(storm::formula::PathBoundOperator<double>::GREATER, qi::_1, qi::_2)] |
				(qi::lit("P") >> qi::lit(">=") > qi::double_ > qi::lit("[") > pathFormula > qi::lit("]"))[qi::_val =
						phoenix::new_<storm::formula::ProbabilisticBoundOperator<double> >(storm::formula::PathBoundOperator<double>::GREATER_EQUAL, qi::_1, qi::_2)] |
				(qi::lit("P") >> qi::lit("<") >> qi::double_ > qi::lit("[") > pathFormula > qi::lit("]"))[qi::_val =
								phoenix::new_<storm::formula::ProbabilisticBoundOperator<double> >(storm::formula::PathBoundOperator<double>::LESS, qi::_1, qi::_2)] |
				(qi::lit("P") > qi::lit("<=") > qi::double_ > qi::lit("[") > pathFormula > qi::lit("]"))[qi::_val =
						phoenix::new_<storm::formula::ProbabilisticBoundOperator<double> >(storm::formula::PathBoundOperator<double>::LESS_EQUAL, qi::_1, qi::_2)]
				);
		probabilisticBoundOperator.name("state formula");
		steadyStateBoundOperator = (
				(qi::lit("S") >> qi::lit(">") >> qi::double_ > qi::lit("[") > stateFormula > qi::lit("]"))[qi::_val =
										phoenix::new_<storm::formula::SteadyStateBoundOperator<double> >(storm::formula::PathBoundOperator<double>::GREATER, qi::_1, qi::_2)] |
				(qi::lit("S") >> qi::lit(">=") >> qi::double_ > qi::lit("[") > stateFormula > qi::lit("]"))[qi::_val =
										phoenix::new_<storm::formula::SteadyStateBoundOperator<double> >(storm::formula::PathBoundOperator<double>::GREATER_EQUAL, qi::_1, qi::_2)] |
				(qi::lit("S") >> qi::lit("<") >> qi::double_ > qi::lit("[") > stateFormula > qi::lit("]"))[qi::_val =
										phoenix::new_<storm::formula::SteadyStateBoundOperator<double> >(storm::formula::PathBoundOperator<double>::LESS, qi::_1, qi::_2)] |
				(qi::lit("S") > qi::lit("<=") >> qi::double_ > qi::lit("[") > stateFormula > qi::lit("]"))[qi::_val =
										phoenix::new_<storm::formula::SteadyStateBoundOperator<double> >(storm::formula::PathBoundOperator<double>::LESS_EQUAL, qi::_1, qi::_2)]
				);
		steadyStateBoundOperator.name("state formula");

		//This block defines rules for parsing formulas with noBoundOperators
		noBoundOperator = (probabilisticNoBoundOperator | rewardNoBoundOperator);
		noBoundOperator.name("no bound operator");
		probabilisticNoBoundOperator = (qi::lit("P") >> qi::lit("=") >> qi::lit("?") >> qi::lit("[") >> pathFormula >> qi::lit("]"))[qi::_val =
				phoenix::new_<storm::formula::ProbabilisticNoBoundOperator<double> >(qi::_1)];
		probabilisticNoBoundOperator.name("no bound operator");
		rewardNoBoundOperator = (qi::lit("R") >> qi::lit("=") >> qi::lit("?") >> qi::lit("[") >> rewardPathFormula >> qi::lit("]"))[qi::_val =
				phoenix::new_<storm::formula::RewardNoBoundOperator<double> >(qi::_1)];
		rewardNoBoundOperator.name("no bound operator");

		//This block defines rules for parsing probabilistic path formulas
		pathFormula = (boundedEventually | eventually | globally | boundedUntil | until);
		pathFormula.name("path formula");
		boundedEventually = (qi::lit("F") >> qi::lit("<=") > qi::int_ > stateFormula)[qi::_val =
				phoenix::new_<storm::formula::BoundedEventually<double>>(qi::_2, qi::_1)];
		boundedEventually.name("path formula (for probablistic operator)");
		eventually = (qi::lit("F") > stateFormula)[qi::_val =
				phoenix::new_<storm::formula::Eventually<double> >(qi::_1)];
		eventually.name("path formula (for probablistic operator)");
		globally = (qi::lit("G") > stateFormula)[qi::_val =
				phoenix::new_<storm::formula::Globally<double> >(qi::_1)];
		globally.name("path formula (for probablistic operator)");
		boundedUntil = (stateFormula[qi::_a = phoenix::construct<std::shared_ptr<storm::formula::AbstractStateFormula<double>>>(qi::_1)] >> qi::lit("U") >> qi::lit("<=") > qi::int_ > stateFormula)
				[qi::_val = phoenix::new_<storm::formula::BoundedUntil<double>>(phoenix::bind(&storm::formula::AbstractStateFormula<double>::clone, phoenix::bind(&std::shared_ptr<storm::formula::AbstractStateFormula<double>>::get, qi::_a)), qi::_3, qi::_2)];
		boundedUntil.name("path formula (for probablistic operator)");
		until = (stateFormula[qi::_a = phoenix::construct<std::shared_ptr<storm::formula::AbstractStateFormula<double>>>(qi::_1)] >> qi::lit("U") > stateFormula)[qi::_val =
				phoenix::new_<storm::formula::Until<double>>(phoenix::bind(&storm::formula::AbstractStateFormula<double>::clone, phoenix::bind(&std::shared_ptr<storm::formula::AbstractStateFormula<double>>::get, qi::_a)), qi::_2)];
		until.name("path formula (for probablistic operator)");

		//This block defines rules for parsing reward path formulas
		rewardPathFormula = (cumulativeReward | reachabilityReward | instantaneousReward | steadyStateReward);
		rewardPathFormula.name("path formula (for reward operator)");
		cumulativeReward = (qi::lit("C") > qi::lit("<=") > qi::double_)
				[qi::_val = phoenix::new_<storm::formula::CumulativeReward<double>>(qi::_1)];
		cumulativeReward.name("path formula (for reward operator)");
		reachabilityReward = (qi::lit("F") > stateFormula)[qi::_val =
				phoenix::new_<storm::formula::ReachabilityReward<double>>(qi::_1)];
		reachabilityReward.name("path formula (for reward operator)");
		instantaneousReward = (qi::lit("I") > qi::lit("=") > qi::double_)
						[qi::_val = phoenix::new_<storm::formula::InstantaneousReward<double>>(qi::_1)];
		instantaneousReward.name("path formula (for reward operator)");
		steadyStateReward = (qi::lit("S"))[qi::_val = phoenix::new_<storm::formula::SteadyStateReward<double>>()];


		start = (noBoundOperator | stateFormula);
		start.name("PRCTL formula");
	}

	qi::rule<Iterator, storm::formula::AbstractFormula<double>*(), Skipper> start;

	qi::rule<Iterator, storm::formula::AbstractStateFormula<double>*(), Skipper> stateFormula;
	qi::rule<Iterator, storm::formula::AbstractStateFormula<double>*(), Skipper> atomicStateFormula;

	qi::rule<Iterator, storm::formula::AbstractStateFormula<double>*(), Skipper> andFormula;
	qi::rule<Iterator, storm::formula::AbstractStateFormula<double>*(), Skipper> atomicProposition;
	qi::rule<Iterator, storm::formula::AbstractStateFormula<double>*(), Skipper> orFormula;
	qi::rule<Iterator, storm::formula::AbstractStateFormula<double>*(), Skipper> notFormula;
	qi::rule<Iterator, storm::formula::ProbabilisticBoundOperator<double>*(), Skipper> probabilisticBoundOperator;
	qi::rule<Iterator, storm::formula::SteadyStateBoundOperator<double>*(), Skipper> steadyStateBoundOperator;
	qi::rule<Iterator, storm::formula::RewardBoundOperator<double>*(), Skipper> rewardBoundOperator;

	qi::rule<Iterator, storm::formula::PathNoBoundOperator<double>*(), Skipper> noBoundOperator;
	qi::rule<Iterator, storm::formula::PathNoBoundOperator<double>*(), Skipper> probabilisticNoBoundOperator;
	qi::rule<Iterator, storm::formula::PathNoBoundOperator<double>*(), Skipper> rewardNoBoundOperator;

	qi::rule<Iterator, storm::formula::AbstractPathFormula<double>*(), Skipper> pathFormula;
	qi::rule<Iterator, storm::formula::BoundedEventually<double>*(), Skipper> boundedEventually;
	qi::rule<Iterator, storm::formula::Eventually<double>*(), Skipper> eventually;
	qi::rule<Iterator, storm::formula::Globally<double>*(), Skipper> globally;
	qi::rule<Iterator, storm::formula::BoundedUntil<double>*(), qi::locals< std::shared_ptr<storm::formula::AbstractStateFormula<double>>>, Skipper> boundedUntil;
	qi::rule<Iterator, storm::formula::Until<double>*(), qi::locals< std::shared_ptr<storm::formula::AbstractStateFormula<double>>>, Skipper> until;

	qi::rule<Iterator, storm::formula::AbstractPathFormula<double>*(), Skipper> rewardPathFormula;
	qi::rule<Iterator, storm::formula::CumulativeReward<double>*(), Skipper> cumulativeReward;
	qi::rule<Iterator, storm::formula::ReachabilityReward<double>*(), Skipper> reachabilityReward;
	qi::rule<Iterator, storm::formula::InstantaneousReward<double>*(), Skipper> instantaneousReward;
	qi::rule<Iterator, storm::formula::SteadyStateReward<double>*(), Skipper> steadyStateReward;


	qi::rule<Iterator, std::string(), Skipper> freeIdentifierName;

};

CslParser::CslParser(std::string formulaString) {
	// TODO Auto-generated constructor stub

}

CslParser::~CslParser() {
	// TODO Auto-generated destructor stub
}

} /* namespace parser */
} /* namespace storm */
