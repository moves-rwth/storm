#include "src/parser/PrctlParser.h"
#include "src/utility/OsDetection.h"
#include "src/utility/ConstTemplates.h"

// If the parser fails due to ill-formed data, this exception is thrown.
#include "src/exceptions/WrongFileFormatException.h"

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


// Some typedefs and namespace definitions to reduce code size.
typedef std::string::const_iterator BaseIteratorType;
typedef boost::spirit::classic::position_iterator2<BaseIteratorType> PositionIteratorType;
namespace qi = boost::spirit::qi;
namespace phoenix = boost::phoenix;



namespace storm {

namespace parser {

template<typename Iterator, typename Skipper>
struct PrctlParser::PrctlGrammar : qi::grammar<Iterator, storm::formula::AbstractFormula<double>*(), Skipper> {
	PrctlGrammar() : PrctlGrammar::base_type(start) {
		freeIdentifierName = qi::lexeme[(qi::alpha | qi::char_('_'))];

		//This block defines rules for parsing state formulas
		stateFormula %= orFormula;

		andFormula = notFormula[qi::_val = qi::_1] >> *(qi::lit("&") >> notFormula)[qi::_val =
				phoenix::new_<storm::formula::And<double>>(qi::_val, qi::_1)];
		orFormula = andFormula[qi::_val = qi::_1] >> *(qi::lit("|") >> andFormula)[qi::_val =
				phoenix::new_<storm::formula::Or<double>>(qi::_val, qi::_1)];
		notFormula = atomicStateFormula[qi::_val = qi::_1] | (qi::lit("!") >> atomicStateFormula)[qi::_val =
				phoenix::new_<storm::formula::Not<double>>(qi::_1)];

		atomicStateFormula %= probabilisticBoundOperator | rewardBoundOperator | atomicProposition | qi::lit("(") >> stateFormula >> qi::lit(")");
		atomicProposition = (freeIdentifierName)[qi::_val =
				phoenix::new_<storm::formula::Ap<double>>(qi::_1)];
		probabilisticBoundOperator = (
				(qi::lit("P") >> qi::lit(">") >> qi::double_ >> qi::lit("[") >> pathFormula >> qi::lit("]"))[qi::_val =
						phoenix::new_<storm::formula::ProbabilisticBoundOperator<double> >(storm::formula::PathBoundOperator<double>::GREATER, qi::_1, qi::_2)] |
				(qi::lit("P") >> qi::lit(">=") >> qi::double_ >> qi::lit("[") >> pathFormula >> qi::lit("]"))[qi::_val =
						phoenix::new_<storm::formula::ProbabilisticBoundOperator<double> >(storm::formula::PathBoundOperator<double>::GREATER_EQUAL, qi::_1, qi::_2)] |
				(qi::lit("P") >> qi::lit("<")  >> qi::double_ >> qi::lit("[") >> pathFormula >> qi::lit("]"))[qi::_val =
								phoenix::new_<storm::formula::ProbabilisticBoundOperator<double> >(storm::formula::PathBoundOperator<double>::LESS, qi::_1, qi::_2)] |
				(qi::lit("P") >> qi::lit("<=") >> qi::double_ >> qi::lit("[") >> pathFormula >> qi::lit("]"))[qi::_val =
						phoenix::new_<storm::formula::ProbabilisticBoundOperator<double> >(storm::formula::PathBoundOperator<double>::LESS_EQUAL, qi::_1, qi::_2)]
				);
		rewardBoundOperator = (
				(qi::lit("R") >> qi::lit(">") >> qi::double_ >> qi::lit("[") >> pathFormula >> qi::lit("]"))[qi::_val =
						phoenix::new_<storm::formula::RewardBoundOperator<double> >(storm::formula::PathBoundOperator<double>::GREATER, qi::_1, qi::_2)] |
				(qi::lit("R") >> qi::lit(">=") >> qi::double_ >> qi::lit("[") >> pathFormula >> qi::lit("]"))[qi::_val =
								phoenix::new_<storm::formula::RewardBoundOperator<double> >(storm::formula::PathBoundOperator<double>::GREATER_EQUAL, qi::_1, qi::_2)] |
				(qi::lit("R") >> qi::lit("<") >> qi::double_ >> qi::lit("[") >> pathFormula >> qi::lit("]"))[qi::_val =
										phoenix::new_<storm::formula::RewardBoundOperator<double> >(storm::formula::PathBoundOperator<double>::LESS, qi::_1, qi::_2)] |
				(qi::lit("R") >> qi::lit("<=")>> qi::double_ >> qi::lit("[") >> pathFormula >> qi::lit("]"))[qi::_val =
						phoenix::new_<storm::formula::RewardBoundOperator<double> >(storm::formula::PathBoundOperator<double>::LESS_EQUAL, qi::_1, qi::_2)]
				);

		//This block defines rules for parsing formulas with noBoundOperators
		noBoundOperator = (probabilisticNoBoundOperator | rewardNoBoundOperator);
		probabilisticNoBoundOperator = (qi::lit("P") >> qi::lit("=") >> qi::lit("?") >> qi::lit("[") >> pathFormula >> qi::lit("]"))[qi::_val =
				phoenix::new_<storm::formula::ProbabilisticNoBoundOperator<double> >(qi::_1)];
		rewardNoBoundOperator = (qi::lit("R") >> qi::lit("=") >> qi::lit("?") >> qi::lit("[") >> pathFormula >> qi::lit("]"))[qi::_val =
				phoenix::new_<storm::formula::RewardNoBoundOperator<double> >(qi::_1)];

		//This block defines rules for parsing path formulas
		pathFormula = (eventually | boundedEventually | globally | boundedUntil | until);
		eventually = (qi::lit("F") >> stateFormula)[qi::_val =
				phoenix::new_<storm::formula::Eventually<double> >(qi::_1)];
		boundedEventually = (qi::lit("F") >> qi::lit("<=") >> qi::int_ >> stateFormula)[qi::_val =
				phoenix::new_<storm::formula::BoundedEventually<double>>(qi::_2, qi::_1)];
		globally = (qi::lit("G") >> stateFormula)[qi::_val =
				phoenix::new_<storm::formula::Globally<double> >(qi::_1)];
		until = (stateFormula >> qi::lit("U") >> stateFormula)[qi::_val =
				phoenix::new_<storm::formula::Until<double>>(qi::_1, qi::_2)];
		boundedUntil = (stateFormula >> qi::lit("U") >> qi::lit("<=") >> qi::int_ >> stateFormula)[qi::_val =
				phoenix::new_<storm::formula::BoundedUntil<double>>(qi::_1, qi::_3, qi::_2)];

		start = (noBoundOperator | stateFormula);

	}

	qi::rule<Iterator, storm::formula::AbstractFormula<double>*(), Skipper> start;

	qi::rule<Iterator, storm::formula::AbstractStateFormula<double>*(), Skipper> stateFormula;
	qi::rule<Iterator, storm::formula::AbstractStateFormula<double>*(), Skipper> atomicStateFormula;

	qi::rule<Iterator, storm::formula::AbstractStateFormula<double>*(), Skipper> andFormula;
	qi::rule<Iterator, storm::formula::AbstractStateFormula<double>*(), Skipper> atomicProposition;
	qi::rule<Iterator, storm::formula::AbstractStateFormula<double>*(), Skipper> orFormula;
	qi::rule<Iterator, storm::formula::AbstractStateFormula<double>*(), Skipper> notFormula;
	qi::rule<Iterator, storm::formula::ProbabilisticBoundOperator<double>*(), Skipper> probabilisticBoundOperator;
	qi::rule<Iterator, storm::formula::RewardBoundOperator<double>*(), Skipper> rewardBoundOperator;

	qi::rule<Iterator, storm::formula::NoBoundOperator<double>*(), Skipper> noBoundOperator;
	qi::rule<Iterator, storm::formula::NoBoundOperator<double>*(), Skipper> probabilisticNoBoundOperator;
	qi::rule<Iterator, storm::formula::NoBoundOperator<double>*(), Skipper> rewardNoBoundOperator;

	qi::rule<Iterator, storm::formula::AbstractPathFormula<double>*(), Skipper> pathFormula;
	qi::rule<Iterator, storm::formula::BoundedEventually<double>*(), Skipper> boundedEventually;
	qi::rule<Iterator, storm::formula::Eventually<double>*(), Skipper> eventually;
	qi::rule<Iterator, storm::formula::Globally<double>*(), Skipper> globally;
	qi::rule<Iterator, storm::formula::BoundedUntil<double>*(), Skipper> boundedUntil;
	qi::rule<Iterator, storm::formula::Until<double>*(), Skipper> until;

	qi::rule<Iterator, std::string(), Skipper> freeIdentifierName;

};

} //namespace storm
} //namespace parser

void storm::parser::PrctlParser::parse(std::string formulaString) {
	BaseIteratorType stringIteratorBegin = formulaString.begin();
	BaseIteratorType stringIteratorEnd = formulaString.end();
	PositionIteratorType positionIteratorBegin(stringIteratorBegin, stringIteratorEnd, formulaString);
	PositionIteratorType positionIteratorEnd;


	// Prepare resulting intermediate representation of input.
	storm::formula::AbstractFormula<double>* result_pointer = nullptr;

	PrctlGrammar<PositionIteratorType,  BOOST_TYPEOF(boost::spirit::ascii::space)> grammar;

	qi::phrase_parse(positionIteratorBegin, positionIteratorEnd, grammar, boost::spirit::ascii::space, result_pointer);

	formula = result_pointer;
}

storm::parser::PrctlParser::PrctlParser(std::string formula) {
	parse(formula);
}
