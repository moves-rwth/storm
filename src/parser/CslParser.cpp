/*
 * CslParser.cpp
 *
 *  Created on: 08.04.2013
 *      Author: Thomas Heinemann
 */

#include "src/parser/CslParser.h"
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
struct CslParser::CslGrammar : qi::grammar<Iterator, storm::formula::csl::AbstractCslFormula<double>*(), Skipper > {
	CslGrammar() : CslGrammar::base_type(start) {
		freeIdentifierName = qi::lexeme[+(qi::alpha | qi::char_('_'))];

		//This block defines rules for parsing state formulas
		stateFormula %= orFormula;
		stateFormula.name("state formula");
		orFormula = andFormula[qi::_val = qi::_1] > *(qi::lit("|") > andFormula)[qi::_val =
				phoenix::new_<storm::formula::csl::Or<double>>(qi::_val, qi::_1)];
		orFormula.name("state formula");
		andFormula = notFormula[qi::_val = qi::_1] > *(qi::lit("&") > notFormula)[qi::_val =
				phoenix::new_<storm::formula::csl::And<double>>(qi::_val, qi::_1)];
		andFormula.name("state formula");
		notFormula = atomicStateFormula[qi::_val = qi::_1] | (qi::lit("!") > atomicStateFormula)[qi::_val =
				phoenix::new_<storm::formula::csl::Not<double>>(qi::_1)];
		notFormula.name("state formula");

		//This block defines rules for "atomic" state formulas
		//(Propositions, probabilistic/reward formulas, and state formulas in brackets)
		atomicStateFormula %= probabilisticBoundOperator | steadyStateBoundOperator | atomicProposition | qi::lit("(") >> stateFormula >> qi::lit(")");
		atomicStateFormula.name("state formula");
		atomicProposition = (freeIdentifierName)[qi::_val =
				phoenix::new_<storm::formula::csl::Ap<double>>(qi::_1)];
		atomicProposition.name("state formula");
		probabilisticBoundOperator = (
				(qi::lit("P") >> qi::lit(">") >> qi::double_ > qi::lit("[") > pathFormula > qi::lit("]"))[qi::_val =
						phoenix::new_<storm::formula::csl::ProbabilisticBoundOperator<double> >(storm::formula::GREATER, qi::_1, qi::_2)] |
				(qi::lit("P") >> qi::lit(">=") > qi::double_ > qi::lit("[") > pathFormula > qi::lit("]"))[qi::_val =
						phoenix::new_<storm::formula::csl::ProbabilisticBoundOperator<double> >(storm::formula::GREATER_EQUAL, qi::_1, qi::_2)] |
				(qi::lit("P") >> qi::lit("<") >> qi::double_ > qi::lit("[") > pathFormula > qi::lit("]"))[qi::_val =
								phoenix::new_<storm::formula::csl::ProbabilisticBoundOperator<double> >(storm::formula::LESS, qi::_1, qi::_2)] |
				(qi::lit("P") > qi::lit("<=") > qi::double_ > qi::lit("[") > pathFormula > qi::lit("]"))[qi::_val =
						phoenix::new_<storm::formula::csl::ProbabilisticBoundOperator<double> >(storm::formula::LESS_EQUAL, qi::_1, qi::_2)]
				);
		probabilisticBoundOperator.name("state formula");
		steadyStateBoundOperator = (
				(qi::lit("S") >> qi::lit(">") >> qi::double_ > qi::lit("[") > stateFormula > qi::lit("]"))[qi::_val =
										phoenix::new_<storm::formula::csl::SteadyStateBoundOperator<double> >(storm::formula::GREATER, qi::_1, qi::_2)] |
				(qi::lit("S") >> qi::lit(">=") >> qi::double_ > qi::lit("[") > stateFormula > qi::lit("]"))[qi::_val =
										phoenix::new_<storm::formula::csl::SteadyStateBoundOperator<double> >(storm::formula::GREATER_EQUAL, qi::_1, qi::_2)] |
				(qi::lit("S") >> qi::lit("<") >> qi::double_ > qi::lit("[") > stateFormula > qi::lit("]"))[qi::_val =
										phoenix::new_<storm::formula::csl::SteadyStateBoundOperator<double> >(storm::formula::LESS, qi::_1, qi::_2)] |
				(qi::lit("S") > qi::lit("<=") >> qi::double_ > qi::lit("[") > stateFormula > qi::lit("]"))[qi::_val =
										phoenix::new_<storm::formula::csl::SteadyStateBoundOperator<double> >(storm::formula::LESS_EQUAL, qi::_1, qi::_2)]
				);
		steadyStateBoundOperator.name("state formula");

		//This block defines rules for parsing formulas with noBoundOperators
		noBoundOperator = (probabilisticNoBoundOperator | steadyStateNoBoundOperator);
		noBoundOperator.name("no bound operator");
		probabilisticNoBoundOperator = (qi::lit("P") >> qi::lit("=") >> qi::lit("?") >> qi::lit("[") >> pathFormula >> qi::lit("]"))[qi::_val =
				phoenix::new_<storm::formula::csl::ProbabilisticNoBoundOperator<double> >(qi::_1)];
		probabilisticNoBoundOperator.name("no bound operator");
		steadyStateNoBoundOperator = (qi::lit("S") >> qi::lit("=") >> qi::lit("?") >> qi::lit("[") >> stateFormula >> qi::lit("]"))[qi::_val =
				phoenix::new_<storm::formula::csl::SteadyStateNoBoundOperator<double> >(qi::_1)];
		steadyStateNoBoundOperator.name("no bound operator");

		//This block defines rules for parsing probabilistic path formulas
		pathFormula = (timeBoundedEventually | eventually | globally | timeBoundedUntil | until);
		pathFormula.name("path formula");
		timeBoundedEventually = (
				(qi::lit("F") >> qi::lit("[") > qi::double_ > qi::lit(",") > qi::double_ > qi::lit("]") > stateFormula)[qi::_val =
				phoenix::new_<storm::formula::csl::TimeBoundedEventually<double>>(qi::_1, qi::_2, qi::_3)] |
				(qi::lit("F") >> (qi::lit("<=") | qi::lit("<")) > qi::double_ > stateFormula)[qi::_val =
								phoenix::new_<storm::formula::csl::TimeBoundedEventually<double>>(0, qi::_1, qi::_2)] |
				(qi::lit("F") >> (qi::lit(">=") | qi::lit(">")) > qi::double_ > stateFormula)[qi::_val =
								phoenix::new_<storm::formula::csl::TimeBoundedEventually<double>>(qi::_1, std::numeric_limits<double>::infinity(), qi::_2)]
				);
		timeBoundedEventually.name("path formula (for probabilistic operator)");
		eventually = (qi::lit("F") > stateFormula)[qi::_val =
				phoenix::new_<storm::formula::csl::Eventually<double> >(qi::_1)];
		eventually.name("path formula (for probabilistic operator)");
		globally = (qi::lit("G") > stateFormula)[qi::_val =
				phoenix::new_<storm::formula::csl::Globally<double> >(qi::_1)];
		globally.name("path formula (for probabilistic operator)");
		timeBoundedUntil = (
					(stateFormula[qi::_a = phoenix::construct<std::shared_ptr<storm::formula::csl::AbstractStateFormula<double>>>(qi::_1)] >> qi::lit("U") >> qi::lit("[") > qi::double_ > qi::lit(",") > qi::double_ > qi::lit("]")  > stateFormula)
					[qi::_val = phoenix::new_<storm::formula::csl::TimeBoundedUntil<double>>(qi::_2, qi::_3, phoenix::bind(&storm::formula::csl::AbstractStateFormula<double>::clone, phoenix::bind(&std::shared_ptr<storm::formula::csl::AbstractStateFormula<double>>::get, qi::_a)), qi::_4)] |
					(stateFormula[qi::_a = phoenix::construct<std::shared_ptr<storm::formula::csl::AbstractStateFormula<double>>>(qi::_1)] >> qi::lit("U") >> (qi::lit("<=") | qi::lit("<")) > qi::double_  > stateFormula)
					[qi::_val = phoenix::new_<storm::formula::csl::TimeBoundedUntil<double>>(0, qi::_2, phoenix::bind(&storm::formula::csl::AbstractStateFormula<double>::clone, phoenix::bind(&std::shared_ptr<storm::formula::csl::AbstractStateFormula<double>>::get, qi::_a)), qi::_3)] |
					(stateFormula[qi::_a = phoenix::construct<std::shared_ptr<storm::formula::csl::AbstractStateFormula<double>>>(qi::_1)] >> qi::lit("U") >> (qi::lit(">=") | qi::lit(">")) > qi::double_  > stateFormula)
					[qi::_val = phoenix::new_<storm::formula::csl::TimeBoundedUntil<double>>(qi::_2, std::numeric_limits<double>::infinity(), phoenix::bind(&storm::formula::csl::AbstractStateFormula<double>::clone, phoenix::bind(&std::shared_ptr<storm::formula::csl::AbstractStateFormula<double>>::get, qi::_a)), qi::_3)]
				);
		timeBoundedUntil.name("path formula (for probabilistic operator)");
		until = (stateFormula[qi::_a = phoenix::construct<std::shared_ptr<storm::formula::csl::AbstractStateFormula<double>>>(qi::_1)] >> qi::lit("U") > stateFormula)[qi::_val =
				phoenix::new_<storm::formula::csl::Until<double>>(phoenix::bind(&storm::formula::csl::AbstractStateFormula<double>::clone, phoenix::bind(&std::shared_ptr<storm::formula::csl::AbstractStateFormula<double>>::get, qi::_a)), qi::_2)];
		until.name("path formula (for probabilistic operator)");

		start = (noBoundOperator | stateFormula);
		start.name("CSL formula");
	}

	qi::rule<Iterator, storm::formula::csl::AbstractCslFormula<double>*(), Skipper> start;

	qi::rule<Iterator, storm::formula::csl::AbstractStateFormula<double>*(), Skipper> stateFormula;
	qi::rule<Iterator, storm::formula::csl::AbstractStateFormula<double>*(), Skipper> atomicStateFormula;

	qi::rule<Iterator, storm::formula::csl::AbstractStateFormula<double>*(), Skipper> andFormula;
	qi::rule<Iterator, storm::formula::csl::AbstractStateFormula<double>*(), Skipper> atomicProposition;
	qi::rule<Iterator, storm::formula::csl::AbstractStateFormula<double>*(), Skipper> orFormula;
	qi::rule<Iterator, storm::formula::csl::AbstractStateFormula<double>*(), Skipper> notFormula;
	qi::rule<Iterator, storm::formula::csl::ProbabilisticBoundOperator<double>*(), Skipper> probabilisticBoundOperator;
	qi::rule<Iterator, storm::formula::csl::SteadyStateBoundOperator<double>*(), Skipper> steadyStateBoundOperator;

	qi::rule<Iterator, storm::formula::csl::AbstractNoBoundOperator<double>*(), Skipper> noBoundOperator;
	qi::rule<Iterator, storm::formula::csl::AbstractNoBoundOperator<double>*(), Skipper> probabilisticNoBoundOperator;
	qi::rule<Iterator, storm::formula::csl::AbstractNoBoundOperator<double>*(), Skipper> steadyStateNoBoundOperator;

	qi::rule<Iterator, storm::formula::csl::AbstractPathFormula<double>*(), Skipper> pathFormula;
	qi::rule<Iterator, storm::formula::csl::TimeBoundedEventually<double>*(), Skipper> timeBoundedEventually;
	qi::rule<Iterator, storm::formula::csl::Eventually<double>*(), Skipper> eventually;
	qi::rule<Iterator, storm::formula::csl::Globally<double>*(), Skipper> globally;
	qi::rule<Iterator, storm::formula::csl::TimeBoundedUntil<double>*(), qi::locals< std::shared_ptr<storm::formula::csl::AbstractStateFormula<double>>>, Skipper> timeBoundedUntil;
	qi::rule<Iterator, storm::formula::csl::Until<double>*(), qi::locals< std::shared_ptr<storm::formula::csl::AbstractStateFormula<double>>>, Skipper> until;


	qi::rule<Iterator, std::string(), Skipper> freeIdentifierName;

};

CslParser::CslParser(std::string formulaString) {
	// Prepare iterators to input.
	BaseIteratorType stringIteratorBegin = formulaString.begin();
	BaseIteratorType stringIteratorEnd = formulaString.end();
	PositionIteratorType positionIteratorBegin(stringIteratorBegin, stringIteratorEnd, formulaString);
	PositionIteratorType positionIteratorEnd;


	// Prepare resulting intermediate representation of input.
	storm::formula::csl::AbstractCslFormula<double>* result_pointer = nullptr;

	CslGrammar<PositionIteratorType,  BOOST_TYPEOF(boost::spirit::ascii::space)> grammar;

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
	if (result_pointer == nullptr) {
		throw storm::exceptions::WrongFormatException() << "Syntax error in formula";
	}

	formula = result_pointer;
}

CslParser::~CslParser() {
	// Intentionally left empty
	// Parsed formula is not deleted with the parser!
}

} /* namespace parser */
} /* namespace storm */
