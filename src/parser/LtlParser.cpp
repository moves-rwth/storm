/*
 * LtlParser.cpp
 *
 *  Created on: 22.04.2013
 *      Author: thomas
 */

#include "LtlParser.h"

#include "src/utility/OsDetection.h"
#include "src/utility/constants.h"

// The action class headers.
#include "src/formula/actions/AbstractAction.h"
#include "src/formula/actions/BoundAction.h"
#include "src/formula/actions/InvertAction.h"
#include "src/formula/actions/RangeAction.h"
#include "src/formula/actions/SortAction.h"

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
#define MAKE(Type, ...) phoenix::construct<std::shared_ptr<Type>>(phoenix::new_<Type>(__VA_ARGS__))
typedef std::string::const_iterator BaseIteratorType;
typedef boost::spirit::classic::position_iterator2<BaseIteratorType> PositionIteratorType;
namespace qi = boost::spirit::qi;
namespace phoenix = boost::phoenix;
namespace ltl = storm::property::ltl;


namespace storm {

namespace parser {

template<typename Iterator, typename Skipper>
struct LtlParser::LtlGrammar : qi::grammar<Iterator, std::shared_ptr<storm::property::ltl::LtlFilter<double>>(), Skipper > {
	LtlGrammar() : LtlGrammar::base_type(start) {
		//This block contains helper rules that may be used several times
		freeIdentifierName = qi::lexeme[qi::alpha >> *(qi::alnum | qi::char_('_'))];
		comparisonType = (
				(qi::lit(">="))[qi::_val = storm::property::GREATER_EQUAL] |
				(qi::lit(">"))[qi::_val = storm::property::GREATER] |
				(qi::lit("<="))[qi::_val = storm::property::LESS_EQUAL] |
				(qi::lit("<"))[qi::_val = storm::property::LESS]);
		sortingCategory = (
				(qi::lit("index"))[qi::_val = storm::property::action::SortAction<double>::INDEX] |
				(qi::lit("value"))[qi::_val = storm::property::action::SortAction<double>::VALUE]
				);
		//Comment: Empty line or line starting with "//"
		comment = (qi::lit("//") >> *(qi::char_))[qi::_val = nullptr];

		freeIdentifierName = qi::lexeme[+(qi::alpha | qi::char_('_'))];

		//This block defines rules for parsing state formulas
		formula %= orFormula;
		formula.name("LTL formula");
		orFormula = andFormula[qi::_val = qi::_1] > *(qi::lit("|") > andFormula)[qi::_val =
				MAKE(ltl::Or<double>, qi::_val, qi::_1)];
		orFormula.name("LTL formula");
		andFormula = untilFormula[qi::_val = qi::_1] > *(qi::lit("&") > untilFormula)[qi::_val =
				MAKE(ltl::And<double>, qi::_val, qi::_1)];
		andFormula.name("LTL formula");
		untilFormula = notFormula[qi::_val = qi::_1] >
				*((qi::lit("U") >> qi::lit("<=") > qi::int_ > notFormula)[qi::_val = MAKE(ltl::BoundedUntil<double>, qi::_val, qi::_2, qi::_1)] |
				  (qi::lit("U") > notFormula)[qi::_val = MAKE(ltl::Until<double>, qi::_val, qi::_1)]);
		notFormula = atomicLtlFormula[qi::_val = qi::_1] | (qi::lit("!") > atomicLtlFormula)[qi::_val =
				MAKE(ltl::Not<double>, qi::_1)];
		notFormula.name("LTL formula");

		//This block defines rules for "atomic" state formulas
		//(Propositions, probabilistic/reward formulas, and state formulas in brackets)
		atomicLtlFormula %= pathFormula | atomicProposition | qi::lit("(") >> formula >> qi::lit(")");
		atomicLtlFormula.name("LTL formula");
		atomicProposition = (freeIdentifierName)[qi::_val =
				MAKE(ltl::Ap<double>, qi::_1)];
		atomicProposition.name("LTL formula");

		//This block defines rules for parsing probabilistic path formulas
		pathFormula = (boundedEventually | eventually | globally | next);
		pathFormula.name("LTL formula");
		boundedEventually = (qi::lit("F") >> qi::lit("<=") > qi::int_ > formula)[qi::_val =
				MAKE(ltl::BoundedEventually<double>, qi::_2, qi::_1)];
		boundedEventually.name("LTL formula");
		eventually = (qi::lit("F") >> formula)[qi::_val =
				MAKE(ltl::Eventually<double>, qi::_1)];
		eventually.name("LTL formula");
		globally = (qi::lit("G") >> formula)[qi::_val =
				MAKE(ltl::Globally<double>, qi::_1)];
		globally.name("LTL formula");
		next = (qi::lit("X") >> formula)[qi::_val =
				MAKE(ltl::Next<double>, qi::_1)];
		next.name("LTL formula");

		// This block defines rules for parsing filter actions.
		boundAction = (qi::lit("bound") > qi::lit("(") >> comparisonType >> qi::lit(",") >> qi::double_ >> qi::lit(")"))[qi::_val =
				        phoenix::new_<storm::property::action::BoundAction<double>>(qi::_1, qi::_2)];
		boundAction.name("bound action");

		invertAction = qi::lit("invert")[qi::_val = phoenix::new_<storm::property::action::InvertAction<double>>()];
		invertAction.name("invert action");

		rangeAction = (
				(qi::lit("range") >> qi::lit("(") >> qi::uint_ >> qi::lit(",") > qi::uint_ >> qi::lit(")"))[qi::_val =
						phoenix::new_<storm::property::action::RangeAction<double>>(qi::_1, qi::_2)] |
				(qi::lit("range") >> qi::lit("(") >> qi::uint_ >> qi::lit(")"))[qi::_val =
						phoenix::new_<storm::property::action::RangeAction<double>>(qi::_1, qi::_1 + 1)]
				);
		rangeAction.name("range action");

		sortAction = (
				(qi::lit("sort") > qi::lit("(") >> sortingCategory >> qi::lit(")"))[qi::_val =
						phoenix::new_<storm::property::action::SortAction<double>>(qi::_1)] |
				(qi::lit("sort") > qi::lit("(") >> sortingCategory >> qi::lit(", ") >> qi::lit("asc") > qi::lit(")"))[qi::_val =
						phoenix::new_<storm::property::action::SortAction<double>>(qi::_1, true)] |
				(qi::lit("sort") > qi::lit("(") >> sortingCategory >> qi::lit(", ") >> qi::lit("desc") > qi::lit(")"))[qi::_val =
						phoenix::new_<storm::property::action::SortAction<double>>(qi::_1, false)]
				);
		sortAction.name("sort action");

		abstractAction = (boundAction | invertAction | rangeAction | sortAction) >> (qi::eps | qi::lit(";"));
		abstractAction.name("filter action");

		filter = (qi::lit("filter") >> qi::lit("[") >> +abstractAction >> qi::lit("]") > qi::lit("(") >> formula >> qi::lit(")"))[qi::_val =
					MAKE(ltl::LtlFilter<double>, qi::_2, qi::_1)] |
				 (qi::lit("filter") >> qi::lit("[") >> qi::lit("max") > +abstractAction >> qi::lit("]") >> qi::lit("(") >> formula >> qi::lit(")"))[qi::_val =
					MAKE(ltl::LtlFilter<double>, qi::_2, qi::_1, storm::property::MAXIMIZE)] |
				 (qi::lit("filter") >> qi::lit("[") >> qi::lit("min") > +abstractAction >> qi::lit("]") >> qi::lit("(") >> formula >> qi::lit(")"))[qi::_val =
					MAKE(ltl::LtlFilter<double>, qi::_2, qi::_1, storm::property::MINIMIZE)] |
				 (formula)[qi::_val =
					MAKE(ltl::LtlFilter<double>, qi::_1)];
		filter.name("PRCTL formula filter");

		start = (((filter) > (comment | qi::eps))[qi::_val = qi::_1] | comment[qi::_val = MAKE(ltl::LtlFilter<double>, nullptr)] ) > qi::eoi;
		start.name("LTL formula");
	}

	qi::rule<Iterator, std::shared_ptr<storm::property::ltl::LtlFilter<double>>(), Skipper> start;
	qi::rule<Iterator, std::shared_ptr<storm::property::ltl::LtlFilter<double>>(), Skipper> filter;

	qi::rule<Iterator, storm::property::action::AbstractAction<double>*(), Skipper> abstractAction;
	qi::rule<Iterator, storm::property::action::BoundAction<double>*(), Skipper> boundAction;
	qi::rule<Iterator, storm::property::action::InvertAction<double>*(), Skipper> invertAction;
	qi::rule<Iterator, storm::property::action::RangeAction<double>*(), Skipper> rangeAction;
	qi::rule<Iterator, storm::property::action::SortAction<double>*(), Skipper> sortAction;

	qi::rule<Iterator, std::shared_ptr<storm::property::ltl::AbstractLtlFormula<double>>(), Skipper> comment;
	qi::rule<Iterator, std::shared_ptr<storm::property::ltl::AbstractLtlFormula<double>>(), Skipper> formula;
	qi::rule<Iterator, std::shared_ptr<storm::property::ltl::AbstractLtlFormula<double>>(), Skipper> atomicLtlFormula;

	qi::rule<Iterator, std::shared_ptr<storm::property::ltl::AbstractLtlFormula<double>>(), Skipper> andFormula;
	qi::rule<Iterator, std::shared_ptr<storm::property::ltl::AbstractLtlFormula<double>>(), Skipper> untilFormula;
	qi::rule<Iterator, std::shared_ptr<storm::property::ltl::AbstractLtlFormula<double>>(), Skipper> atomicProposition;
	qi::rule<Iterator, std::shared_ptr<storm::property::ltl::AbstractLtlFormula<double>>(), Skipper> orFormula;
	qi::rule<Iterator, std::shared_ptr<storm::property::ltl::AbstractLtlFormula<double>>(), Skipper> notFormula;

	qi::rule<Iterator, std::shared_ptr<storm::property::ltl::AbstractLtlFormula<double>>(), Skipper> pathFormula;
	qi::rule<Iterator, std::shared_ptr<storm::property::ltl::BoundedEventually<double>>(), Skipper> boundedEventually;
	qi::rule<Iterator, std::shared_ptr<storm::property::ltl::Eventually<double>>(), Skipper> eventually;
	qi::rule<Iterator, std::shared_ptr<storm::property::ltl::Globally<double>>(), Skipper> globally;
	qi::rule<Iterator, std::shared_ptr<storm::property::ltl::Next<double>>(), Skipper> next;
	qi::rule<Iterator, std::shared_ptr<storm::property::ltl::AbstractLtlFormula<double>>(), qi::locals< std::shared_ptr<storm::property::ltl::AbstractLtlFormula<double>>>, Skipper> boundedUntil;
	qi::rule<Iterator, std::shared_ptr<storm::property::ltl::AbstractLtlFormula<double>>(), qi::locals< std::shared_ptr<storm::property::ltl::AbstractLtlFormula<double>>>, Skipper> until;

	qi::rule<Iterator, std::string(), Skipper> freeIdentifierName;
	qi::rule<Iterator, storm::property::ComparisonType(), Skipper> comparisonType;
	qi::rule<Iterator, storm::property::action::SortAction<double>::SortingCategory(), Skipper> sortingCategory;

};

std::shared_ptr<storm::property::ltl::LtlFilter<double>> LtlParser::parseLtlFormula(std::string formulaString) {
	// Prepare iterators to input.
	BaseIteratorType stringIteratorBegin = formulaString.begin();
	BaseIteratorType stringIteratorEnd = formulaString.end();
	PositionIteratorType positionIteratorBegin(stringIteratorBegin, stringIteratorEnd, formulaString);
	PositionIteratorType positionIteratorEnd;


	// Prepare resulting intermediate representation of input.
	std::shared_ptr<storm::property::ltl::LtlFilter<double>> result_pointer(nullptr);

	LtlGrammar<PositionIteratorType,  BOOST_TYPEOF(boost::spirit::ascii::space)> grammar;

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
	if (!result_pointer) {
		throw storm::exceptions::WrongFormatException() << "Syntax error in formula";
	}

	return result_pointer;
}

} //namespace parser
} //namespace storm
