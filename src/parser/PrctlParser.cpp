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



namespace storm {

namespace parser {

template<typename Iterator, typename Skipper>
struct PrctlParser::PrctlGrammar : qi::grammar<Iterator, storm::property::prctl::AbstractPrctlFormula<double>*(), Skipper > {
	PrctlGrammar() : PrctlGrammar::base_type(start) {

		//This block contains helper rules that may be used several times
		freeIdentifierName = qi::lexeme[qi::alpha >> *(qi::alnum | qi::char_('_'))];
		comparisonType = (
				(qi::lit(">="))[qi::_val = storm::property::GREATER_EQUAL] |
				(qi::lit(">"))[qi::_val = storm::property::GREATER] |
				(qi::lit("<="))[qi::_val = storm::property::LESS_EQUAL] |
				(qi::lit("<"))[qi::_val = storm::property::LESS]);
		comment = (qi::lit("//") >> *(qi::char_))[qi::_val = nullptr];

		//This block defines rules for parsing state formulas
		stateFormula %= orFormula;
		stateFormula.name("state formula");
		orFormula = andFormula[qi::_val = qi::_1] > *(qi::lit("|") > andFormula)[qi::_val =
				phoenix::new_<storm::property::prctl::Or<double>>(qi::_val, qi::_1)];
		orFormula.name("state formula");
		andFormula = notFormula[qi::_val = qi::_1] > *(qi::lit("&") > notFormula)[qi::_val =
				phoenix::new_<storm::property::prctl::And<double>>(qi::_val, qi::_1)];
		andFormula.name("state formula");
		notFormula = atomicStateFormula[qi::_val = qi::_1] | (qi::lit("!") > atomicStateFormula)[qi::_val =
				phoenix::new_<storm::property::prctl::Not<double>>(qi::_1)];
		notFormula.name("state formula");

		//This block defines rules for "atomic" state formulas
		//(Propositions, probabilistic/reward formulas, and state formulas in brackets)
		atomicStateFormula %= probabilisticBoundOperator | rewardBoundOperator | atomicProposition | qi::lit("(") >> stateFormula >> qi::lit(")");
		atomicStateFormula.name("state formula");
		atomicProposition = (freeIdentifierName)[qi::_val =
				phoenix::new_<storm::property::prctl::Ap<double>>(qi::_1)];
		atomicProposition.name("state formula");
		probabilisticBoundOperator = (
				(qi::lit("P") >> qi::lit("min") > comparisonType > qi::double_ > qi::lit("[") > pathFormula > qi::lit("]"))[qi::_val =
						phoenix::new_<storm::property::prctl::ProbabilisticBoundOperator<double> >(qi::_1, qi::_2, qi::_3, true)] |
				(qi::lit("P") >> qi::lit("max") > comparisonType > qi::double_ > qi::lit("[") > pathFormula > qi::lit("]"))[qi::_val =
						phoenix::new_<storm::property::prctl::ProbabilisticBoundOperator<double> >(qi::_1, qi::_2, qi::_3, false)] |
				(qi::lit("P") > comparisonType > qi::double_ > qi::lit("[") > pathFormula > qi::lit("]"))[qi::_val =
						phoenix::new_<storm::property::prctl::ProbabilisticBoundOperator<double> >(qi::_1, qi::_2, qi::_3)]
				);

		probabilisticBoundOperator.name("state formula");
		rewardBoundOperator = (
				(qi::lit("R") >> qi::lit("min") > comparisonType > qi::double_ > qi::lit("[") > rewardPathFormula > qi::lit("]"))[qi::_val =
						phoenix::new_<storm::property::prctl::RewardBoundOperator<double> >(qi::_1, qi::_2, qi::_3, true)] |
				(qi::lit("R") >> qi::lit("max") > comparisonType > qi::double_ > qi::lit("[") > rewardPathFormula > qi::lit("]"))[qi::_val =
						phoenix::new_<storm::property::prctl::RewardBoundOperator<double> >(qi::_1, qi::_2, qi::_3, false)] |
				(qi::lit("R") >> comparisonType > qi::double_ > qi::lit("[") > rewardPathFormula > qi::lit("]"))[qi::_val =
						phoenix::new_<storm::property::prctl::RewardBoundOperator<double> >(qi::_1, qi::_2, qi::_3)]
				);
		rewardBoundOperator.name("state formula");

		//This block defines rules for parsing formulas with noBoundOperators
		noBoundOperator = (probabilisticNoBoundOperator | rewardNoBoundOperator);
		noBoundOperator.name("no bound operator");
		probabilisticNoBoundOperator = (
				(qi::lit("P") >> qi::lit("min") >> qi::lit("=") >> qi::lit("?") >> qi::lit("[") >> pathFormula >> qi::lit("]"))[qi::_val =
										phoenix::new_<storm::property::prctl::ProbabilisticNoBoundOperator<double> >(qi::_1, true)] |
				(qi::lit("P") >> qi::lit("max") >> qi::lit("=") >> qi::lit("?") >> qi::lit("[") >> pathFormula >> qi::lit("]"))[qi::_val =
										phoenix::new_<storm::property::prctl::ProbabilisticNoBoundOperator<double> >(qi::_1, false)] |
				(qi::lit("P") >> qi::lit("=") >> qi::lit("?") > qi::lit("[") > pathFormula > qi::lit("]"))[qi::_val =
										phoenix::new_<storm::property::prctl::ProbabilisticNoBoundOperator<double> >(qi::_1)]
				);
		probabilisticNoBoundOperator.name("no bound operator");

		rewardNoBoundOperator = (
				(qi::lit("R") >> qi::lit("min") >> qi::lit("=") >> qi::lit("?") >> qi::lit("[") >> rewardPathFormula >> qi::lit("]"))[qi::_val =
								phoenix::new_<storm::property::prctl::RewardNoBoundOperator<double> >(qi::_1, true)] |
				(qi::lit("R") >> qi::lit("max") >> qi::lit("=") >> qi::lit("?") >> qi::lit("[") >> rewardPathFormula >> qi::lit("]"))[qi::_val =
								phoenix::new_<storm::property::prctl::RewardNoBoundOperator<double> >(qi::_1, false)] |
				(qi::lit("R") >> qi::lit("=") >> qi::lit("?") >> qi::lit("[") >> rewardPathFormula >> qi::lit("]"))[qi::_val =
								phoenix::new_<storm::property::prctl::RewardNoBoundOperator<double> >(qi::_1)]

				);
		rewardNoBoundOperator.name("no bound operator");

		//This block defines rules for parsing probabilistic path formulas
		pathFormula = (boundedEventually | eventually | globally | boundedUntil | until);
		pathFormula.name("path formula");
		boundedEventually = (qi::lit("F") >> qi::lit("<=") > qi::int_ > stateFormula)[qi::_val =
				phoenix::new_<storm::property::prctl::BoundedEventually<double>>(qi::_2, qi::_1)];
		boundedEventually.name("path formula (for probabilistic operator)");
		eventually = (qi::lit("F") > stateFormula)[qi::_val =
				phoenix::new_<storm::property::prctl::Eventually<double> >(qi::_1)];
		eventually.name("path formula (for probabilistic operator)");
		globally = (qi::lit("G") > stateFormula)[qi::_val =
				phoenix::new_<storm::property::prctl::Globally<double> >(qi::_1)];
		globally.name("path formula (for probabilistic operator)");
		boundedUntil = (stateFormula[qi::_a = phoenix::construct<std::shared_ptr<storm::property::prctl::AbstractStateFormula<double>>>(qi::_1)] >> qi::lit("U") >> qi::lit("<=") > qi::int_ > stateFormula)
				[qi::_val = phoenix::new_<storm::property::prctl::BoundedUntil<double>>(phoenix::bind(&storm::property::prctl::AbstractStateFormula<double>::clone, phoenix::bind(&std::shared_ptr<storm::property::prctl::AbstractStateFormula<double>>::get, qi::_a)), qi::_3, qi::_2)];
		boundedUntil.name("path formula (for probabilistic operator)");
		until = (stateFormula[qi::_a = phoenix::construct<std::shared_ptr<storm::property::prctl::AbstractStateFormula<double>>>(qi::_1)] >> qi::lit("U") > stateFormula)[qi::_val =
				phoenix::new_<storm::property::prctl::Until<double>>(phoenix::bind(&storm::property::prctl::AbstractStateFormula<double>::clone, phoenix::bind(&std::shared_ptr<storm::property::prctl::AbstractStateFormula<double>>::get, qi::_a)), qi::_2)];
		until.name("path formula (for probabilistic operator)");

		//This block defines rules for parsing reward path formulas
		rewardPathFormula = (cumulativeReward | reachabilityReward | instantaneousReward | steadyStateReward);
		rewardPathFormula.name("path formula (for reward operator)");
		cumulativeReward = (qi::lit("C") > qi::lit("<=") > qi::double_)
				[qi::_val = phoenix::new_<storm::property::prctl::CumulativeReward<double>>(qi::_1)];
		cumulativeReward.name("path formula (for reward operator)");
		reachabilityReward = (qi::lit("F") > stateFormula)[qi::_val =
				phoenix::new_<storm::property::prctl::ReachabilityReward<double>>(qi::_1)];
		reachabilityReward.name("path formula (for reward operator)");
		instantaneousReward = (qi::lit("I") > qi::lit("=") > qi::double_)
						[qi::_val = phoenix::new_<storm::property::prctl::InstantaneousReward<double>>(qi::_1)];
		instantaneousReward.name("path formula (for reward operator)");
		steadyStateReward = (qi::lit("S"))[qi::_val = phoenix::new_<storm::property::prctl::SteadyStateReward<double>>()];

		start = (comment | noBoundOperator | stateFormula) >> qi::eoi;
		start.name("PRCTL formula");
	}

	qi::rule<Iterator, storm::property::prctl::AbstractPrctlFormula<double>*(), Skipper> start;
	qi::rule<Iterator, storm::property::prctl::AbstractPrctlFormula<double>*(), Skipper> comment;

	qi::rule<Iterator, storm::property::prctl::AbstractStateFormula<double>*(), Skipper> stateFormula;
	qi::rule<Iterator, storm::property::prctl::AbstractStateFormula<double>*(), Skipper> atomicStateFormula;

	qi::rule<Iterator, storm::property::prctl::AbstractStateFormula<double>*(), Skipper> andFormula;
	qi::rule<Iterator, storm::property::prctl::AbstractStateFormula<double>*(), Skipper> atomicProposition;
	qi::rule<Iterator, storm::property::prctl::AbstractStateFormula<double>*(), Skipper> orFormula;
	qi::rule<Iterator, storm::property::prctl::AbstractStateFormula<double>*(), Skipper> notFormula;
	qi::rule<Iterator, storm::property::prctl::ProbabilisticBoundOperator<double>*(), Skipper> probabilisticBoundOperator;
	qi::rule<Iterator, storm::property::prctl::RewardBoundOperator<double>*(), Skipper> rewardBoundOperator;

	qi::rule<Iterator, storm::property::prctl::AbstractNoBoundOperator<double>*(), Skipper> noBoundOperator;
	qi::rule<Iterator, storm::property::prctl::AbstractNoBoundOperator<double>*(), Skipper> probabilisticNoBoundOperator;
	qi::rule<Iterator, storm::property::prctl::AbstractNoBoundOperator<double>*(), Skipper> rewardNoBoundOperator;

	qi::rule<Iterator, storm::property::prctl::AbstractPathFormula<double>*(), Skipper> pathFormula;
	qi::rule<Iterator, storm::property::prctl::BoundedEventually<double>*(), Skipper> boundedEventually;
	qi::rule<Iterator, storm::property::prctl::Eventually<double>*(), Skipper> eventually;
	qi::rule<Iterator, storm::property::prctl::Globally<double>*(), Skipper> globally;
	qi::rule<Iterator, storm::property::prctl::BoundedUntil<double>*(), qi::locals< std::shared_ptr<storm::property::prctl::AbstractStateFormula<double>>>, Skipper> boundedUntil;
	qi::rule<Iterator, storm::property::prctl::Until<double>*(), qi::locals< std::shared_ptr<storm::property::prctl::AbstractStateFormula<double>>>, Skipper> until;

	qi::rule<Iterator, storm::property::prctl::AbstractPathFormula<double>*(), Skipper> rewardPathFormula;
	qi::rule<Iterator, storm::property::prctl::CumulativeReward<double>*(), Skipper> cumulativeReward;
	qi::rule<Iterator, storm::property::prctl::ReachabilityReward<double>*(), Skipper> reachabilityReward;
	qi::rule<Iterator, storm::property::prctl::InstantaneousReward<double>*(), Skipper> instantaneousReward;
	qi::rule<Iterator, storm::property::prctl::SteadyStateReward<double>*(), Skipper> steadyStateReward;


	qi::rule<Iterator, std::string(), Skipper> freeIdentifierName;
	qi::rule<Iterator, storm::property::ComparisonType(), Skipper> comparisonType;

};

} //namespace storm
} //namespace parser

storm::parser::PrctlParser::PrctlParser(std::string formulaString) {
	// Prepare iterators to input.
	BaseIteratorType stringIteratorBegin = formulaString.begin();
	BaseIteratorType stringIteratorEnd = formulaString.end();
	PositionIteratorType positionIteratorBegin(stringIteratorBegin, stringIteratorEnd, formulaString);
	PositionIteratorType positionIteratorEnd;


	// Prepare resulting intermediate representation of input.
	storm::property::prctl::AbstractPrctlFormula<double>* result_pointer = nullptr;

	PrctlGrammar<PositionIteratorType,  BOOST_TYPEOF(boost::spirit::ascii::space)> grammar;

	// Now, parse the formula from the given string
	try {
		qi::phrase_parse(positionIteratorBegin, positionIteratorEnd, grammar, boost::spirit::ascii::space, result_pointer);
	} catch(const qi::expectation_failure<PositionIteratorType>& e) {
		// If the parser expected content different than the one provided, display information
		// about the location of the error.
		const boost::spirit::classic::file_position_base<std::string>& pos = e.first.get_position();

		// Construct the error message including a caret display of the position in the
		// erroneous line.
		std::stringstream msg;
		msg << pos.file << ", line " << pos.line << ", column " << pos.column
				<< ": parse error: expected " << e.what_ << std::endl << "\t"
				<< e.first.get_currentline() << std::endl << "\t";
		int i = 0;
		for (i = 0; i < pos.column; ++i) {
			msg << "-";
		}
		msg << "^";
		for (; i < 80; ++i) {
			msg << "-";
		}
		msg << std::endl;

		std::cerr << msg.str();

		// Now propagate exception.
		throw storm::exceptions::WrongFormatException() << msg.str();
	}

	// The syntax can be so wrong that no rule can be matched at all
	// In that case, no expectation failure is thrown, but the parser just returns nullptr
	// Then, of course the result is not usable, hence we throw a WrongFormatException, too.
	if (positionIteratorBegin != positionIteratorEnd) {
		throw storm::exceptions::WrongFormatException() << "Syntax error in formula";
	}

	formula = result_pointer;
}
