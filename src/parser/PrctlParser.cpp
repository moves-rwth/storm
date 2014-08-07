#include "src/parser/PrctlParser.h"
#include "src/utility/OsDetection.h"
#include "src/utility/constants.h"

// The action class headers.
#include "src/formula/actions/AbstractAction.h"
#include "src/formula/actions/BoundAction.h"
#include "src/formula/actions/InvertAction.h"
#include "src/formula/actions/FormulaAction.h"
#include "src/formula/actions/RangeAction.h"
#include "src/formula/actions/SortAction.h"

// If the parser fails due to ill-formed data, this exception is thrown.
#include "src/exceptions/WrongFormatException.h"

// Used for Boost spirit.
#include <boost/typeof/typeof.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix.hpp>
#include <boost/spirit/include/phoenix_function.hpp>

// Include headers for spirit iterators. Needed for diagnostics and input stream iteration.
#include <boost/spirit/include/classic_position_iterator.hpp>
#include <boost/spirit/include/support_multi_pass.hpp>

// Needed for file IO.
#include <fstream>
#include <iomanip>
#include <map>


// Some typedefs, namespace definitions and a macro to reduce code size.
#define MAKE(Type, ...) phoenix::construct<std::shared_ptr<Type>>(phoenix::new_<Type>(__VA_ARGS__))
typedef std::string::const_iterator BaseIteratorType;
typedef boost::spirit::classic::position_iterator2<BaseIteratorType> PositionIteratorType;
namespace qi = boost::spirit::qi;
namespace phoenix = boost::phoenix;
namespace prctl = storm::property::prctl;


namespace storm {

namespace parser {

template<typename Iterator, typename Skipper>
struct PrctlParser::PrctlGrammar : qi::grammar<Iterator, std::shared_ptr<storm::property::prctl::PrctlFilter<double>>(), Skipper > {
	PrctlGrammar() : PrctlGrammar::base_type(start) {
		// This block contains helper rules that may be used several times
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
		// Comment: Empty line or line starting with "//"
		comment = (qi::lit("//") >> *(qi::char_))[qi::_val = nullptr];

		// This block defines rules for parsing state formulas
		stateFormula %= orFormula;
		stateFormula.name("state formula");
		orFormula = andFormula[qi::_val = qi::_1] > *(qi::lit("|") > andFormula)[qi::_val =
				MAKE(prctl::Or<double>, qi::_val, qi::_1)];
		orFormula.name("or formula");
		andFormula = notFormula[qi::_val = qi::_1] > *(qi::lit("&") > notFormula)[qi::_val =
				MAKE(prctl::And<double>, qi::_val, qi::_1)];
		andFormula.name("and formula");
		notFormula = atomicStateFormula[qi::_val = qi::_1] | (qi::lit("!") > atomicStateFormula)[qi::_val =
				MAKE(prctl::Not<double>, qi::_1)];
		notFormula.name("not formula");

		// This block defines rules for "atomic" state formulas
		// (Propositions, probabilistic/reward formulas, and state formulas in brackets)
		atomicStateFormula %= probabilisticBoundOperator | rewardBoundOperator | atomicProposition | qi::lit("(") >> stateFormula >> qi::lit(")") | qi::lit("[") >> stateFormula >> qi::lit("]");
		atomicStateFormula.name("atomic state formula");
		atomicProposition = (freeIdentifierName)[qi::_val =
				MAKE(prctl::Ap<double>, qi::_1)];
		atomicProposition.name("atomic proposition");
		probabilisticBoundOperator = ((qi::lit("P") >> comparisonType > qi::double_ > pathFormula)[qi::_val =
				MAKE(prctl::ProbabilisticBoundOperator<double>, qi::_1, qi::_2, qi::_3)]);
		probabilisticBoundOperator.name("probabilistic bound operator");
		rewardBoundOperator = ((qi::lit("R") >> comparisonType > qi::double_ > rewardPathFormula)[qi::_val =
				MAKE(prctl::RewardBoundOperator<double>, qi::_1, qi::_2, qi::_3)]);
		rewardBoundOperator.name("reward bound operator");

		// This block defines rules for parsing probabilistic path formulas
		pathFormula = (boundedEventually | eventually | next | globally | boundedUntil | until | qi::lit("(") >> pathFormula >> qi::lit(")") | qi::lit("[") >> pathFormula >> qi::lit("]"));
		pathFormula.name("path formula");
		boundedEventually = (qi::lit("F") >> qi::lit("<=") > qi::int_ > stateFormula)[qi::_val =
				MAKE(prctl::BoundedEventually<double>, qi::_2, qi::_1)];
		boundedEventually.name("bounded eventually");
		eventually = (qi::lit("F") > stateFormula)[qi::_val =
				MAKE(prctl::Eventually<double>, qi::_1)];
		eventually.name("eventually");
		next = (qi::lit("X") > stateFormula)[qi::_val =
				MAKE(prctl::Next<double>, qi::_1)];
		next.name("next");
		globally = (qi::lit("G") > stateFormula)[qi::_val =
				MAKE(prctl::Globally<double>, qi::_1)];
		globally.name("globally");
		boundedUntil = (stateFormula[qi::_a = qi::_1] >> qi::lit("U") >> qi::lit("<=") > qi::int_ > stateFormula)[qi::_val =
				MAKE(prctl::BoundedUntil<double>, qi::_a, qi::_3, qi::_2)];
		boundedUntil.name("boundedUntil");
		until = (stateFormula[qi::_a = qi::_1] >> qi::lit("U") > stateFormula)[qi::_val =
				MAKE(prctl::Until<double>, qi::_a, qi::_2)];
		until.name("until");

		// This block defines rules for parsing reward path formulas.
		rewardPathFormula = (cumulativeReward | reachabilityReward | instantaneousReward | steadyStateReward | qi::lit("(") >> rewardPathFormula >> qi::lit(")") | qi::lit("[") >> rewardPathFormula >> qi::lit("]"));
		rewardPathFormula.name("path formula (for reward operator)");
		cumulativeReward = (qi::lit("C") > qi::lit("<=") > qi::double_)[qi::_val =
				MAKE(prctl::CumulativeReward<double>, qi::_1)];
		cumulativeReward.name("path formula (for reward operator)");
		reachabilityReward = (qi::lit("F") > stateFormula)[qi::_val =
				MAKE(prctl::ReachabilityReward<double>, qi::_1)];
		reachabilityReward.name("path formula (for reward operator)");
		instantaneousReward = (qi::lit("I") > qi::lit("=") > qi::double_)[qi::_val =
				MAKE(prctl::InstantaneousReward<double>, qi::_1)];
		instantaneousReward.name("path formula (for reward operator)");
		steadyStateReward = (qi::lit("S"))[qi::_val =
				MAKE(prctl::SteadyStateReward<double>, )];

		formula = (pathFormula | stateFormula);
		formula.name("PRCTL formula");

		// This block defines rules for parsing formulas with noBoundOperators.
		// Note that this is purely for legacy support.
		// NoBoundOperators are no longer part of the formula tree and are therefore deprecated.
		noBoundOperator = (probabilisticNoBoundOperator | rewardNoBoundOperator);
		noBoundOperator.name("no bound operator");
		probabilisticNoBoundOperator = (
				(qi::lit("P") >> qi::lit("min") >> qi::lit("=") > qi::lit("?") >> pathFormula )[qi::_val =
						MAKE(prctl::PrctlFilter<double>, qi::_1, storm::property::MINIMIZE)] |
				(qi::lit("P") >> qi::lit("max") >> qi::lit("=") > qi::lit("?") >> pathFormula )[qi::_val =
						MAKE(prctl::PrctlFilter<double>, qi::_1, storm::property::MAXIMIZE)] |
				(qi::lit("P") >> qi::lit("=") > qi::lit("?") >> pathFormula )[qi::_val =
						MAKE(prctl::PrctlFilter<double>, qi::_1)]
				);
		probabilisticNoBoundOperator.name("no bound operator");

		rewardNoBoundOperator = (
				(qi::lit("R") >> qi::lit("min") >> qi::lit("=") > qi::lit("?") >> rewardPathFormula )[qi::_val =
						MAKE(prctl::PrctlFilter<double>, qi::_1, storm::property::MINIMIZE)] |
				(qi::lit("R") >> qi::lit("max") >> qi::lit("=") > qi::lit("?") >> rewardPathFormula )[qi::_val =
						MAKE(prctl::PrctlFilter<double>, qi::_1, storm::property::MAXIMIZE)] |
				(qi::lit("R") >> qi::lit("=") >> qi::lit("?") >> rewardPathFormula )[qi::_val =
						MAKE(prctl::PrctlFilter<double>, qi::_1)]

				);
		rewardNoBoundOperator.name("no bound operator");


		// This block defines rules for parsing filter actions.
		boundAction = (qi::lit("bound") > qi::lit("(") >> comparisonType >> qi::lit(",") >> qi::double_ >> qi::lit(")"))[qi::_val =
				        phoenix::new_<storm::property::action::BoundAction<double>>(qi::_1, qi::_2)];
		boundAction.name("bound action");

		invertAction = qi::lit("invert")[qi::_val = phoenix::new_<storm::property::action::InvertAction<double>>()];
		invertAction.name("invert action");

		formulaAction = (qi::lit("formula") > qi::lit("(") >> stateFormula >> qi::lit(")"))[qi::_val =
						phoenix::new_<storm::property::action::FormulaAction<double>>(qi::_1)];
		formulaAction.name("formula action");

		rangeAction = (
				(qi::lit("range") >> qi::lit("(") >> qi::uint_ >> qi::lit(",") > qi::uint_ >> qi::lit(")"))[qi::_val =
						phoenix::new_<storm::property::action::RangeAction<double>>(qi::_1, qi::_2)] |
				(qi::lit("range") >> qi::lit("(") >> qi::uint_ >> qi::lit(")"))[qi::_val =
						phoenix::new_<storm::property::action::RangeAction<double>>(qi::_1, qi::_1 + 1)]
				);
		rangeAction.name("range action");

		sortAction = (
				(qi::lit("sort") >> qi::lit("(") >> sortingCategory >> qi::lit(")"))[qi::_val =
						phoenix::new_<storm::property::action::SortAction<double>>(qi::_1)] |
				(qi::lit("sort") >> qi::lit("(") >> sortingCategory >> qi::lit(", ") >> (qi::lit("ascending") | qi::lit("asc")) > qi::lit(")"))[qi::_val =
						phoenix::new_<storm::property::action::SortAction<double>>(qi::_1, true)] |
				(qi::lit("sort") >> qi::lit("(") >> sortingCategory >> qi::lit(", ") >> (qi::lit("descending") | qi::lit("desc")) > qi::lit(")"))[qi::_val =
						phoenix::new_<storm::property::action::SortAction<double>>(qi::_1, false)]
				);
		sortAction.name("sort action");

		abstractAction = (boundAction | invertAction | formulaAction | rangeAction | sortAction) >> (qi::lit(";") | qi::eps);
		abstractAction.name("filter action");

		filter = (qi::lit("filter") >> qi::lit("[") >> +abstractAction >> qi::lit("]") > qi::lit("(") >> formula >> qi::lit(")"))[qi::_val =
					MAKE(prctl::PrctlFilter<double>, qi::_2, qi::_1)] |
				 (qi::lit("filter") >> qi::lit("[") >> qi::lit("max") > +abstractAction >> qi::lit("]") >> qi::lit("(") >> formula >> qi::lit(")"))[qi::_val =
					MAKE(prctl::PrctlFilter<double>, qi::_2, qi::_1, storm::property::MAXIMIZE)] |
				 (qi::lit("filter") >> qi::lit("[") >> qi::lit("min") > +abstractAction >> qi::lit("]") >> qi::lit("(") >> formula >> qi::lit(")"))[qi::_val =
					MAKE(prctl::PrctlFilter<double>, qi::_2, qi::_1, storm::property::MINIMIZE)] |
				 (noBoundOperator)[qi::_val =
					qi::_1] |
				 (formula)[qi::_val =
					MAKE(prctl::PrctlFilter<double>, qi::_1)];
		filter.name("PRCTL formula filter");

		start = (((filter) > (comment | qi::eps))[qi::_val = qi::_1] | comment[qi::_val = MAKE(prctl::PrctlFilter<double>, nullptr)]) > qi::eoi;
		start.name("start");

	}

	qi::rule<Iterator, std::shared_ptr<storm::property::prctl::PrctlFilter<double>>(), Skipper> start;
	qi::rule<Iterator, std::shared_ptr<storm::property::prctl::PrctlFilter<double>>(), Skipper> filter;

	qi::rule<Iterator, std::shared_ptr<storm::property::prctl::PrctlFilter<double>>(), Skipper> noBoundOperator;
	qi::rule<Iterator, std::shared_ptr<storm::property::prctl::PrctlFilter<double>>(), Skipper> probabilisticNoBoundOperator;
	qi::rule<Iterator, std::shared_ptr<storm::property::prctl::PrctlFilter<double>>(), Skipper> rewardNoBoundOperator;

	qi::rule<Iterator, storm::property::action::AbstractAction<double>*(), Skipper> abstractAction;
	qi::rule<Iterator, storm::property::action::BoundAction<double>*(), Skipper> boundAction;
	qi::rule<Iterator, storm::property::action::InvertAction<double>*(), Skipper> invertAction;
	qi::rule<Iterator, storm::property::action::FormulaAction<double>*(), Skipper> formulaAction;
	qi::rule<Iterator, storm::property::action::RangeAction<double>*(), Skipper> rangeAction;
	qi::rule<Iterator, storm::property::action::SortAction<double>*(), Skipper> sortAction;

	qi::rule<Iterator, std::shared_ptr<storm::property::prctl::AbstractPrctlFormula<double>>(), Skipper> formula;
	qi::rule<Iterator, std::shared_ptr<storm::property::prctl::AbstractPrctlFormula<double>>(), Skipper> comment;

	qi::rule<Iterator, std::shared_ptr<storm::property::prctl::AbstractStateFormula<double>>(), Skipper> stateFormula;
	qi::rule<Iterator, std::shared_ptr<storm::property::prctl::AbstractStateFormula<double>>(), Skipper> atomicStateFormula;

	qi::rule<Iterator, std::shared_ptr<storm::property::prctl::AbstractStateFormula<double>>(), Skipper> andFormula;
	qi::rule<Iterator, std::shared_ptr<storm::property::prctl::AbstractStateFormula<double>>(), Skipper> atomicProposition;
	qi::rule<Iterator, std::shared_ptr<storm::property::prctl::AbstractStateFormula<double>>(), Skipper> orFormula;
	qi::rule<Iterator, std::shared_ptr<storm::property::prctl::AbstractStateFormula<double>>(), Skipper> notFormula;
	qi::rule<Iterator, std::shared_ptr<storm::property::prctl::ProbabilisticBoundOperator<double>>(), Skipper> probabilisticBoundOperator;
	qi::rule<Iterator, std::shared_ptr<storm::property::prctl::RewardBoundOperator<double>>(), Skipper> rewardBoundOperator;

	qi::rule<Iterator, std::shared_ptr<storm::property::prctl::AbstractPathFormula<double>>(), Skipper> pathFormula;
	qi::rule<Iterator, std::shared_ptr<storm::property::prctl::BoundedEventually<double>>(), Skipper> boundedEventually;
	qi::rule<Iterator, std::shared_ptr<storm::property::prctl::Eventually<double>>(), Skipper> eventually;
	qi::rule<Iterator, std::shared_ptr<storm::property::prctl::Next<double>>(), Skipper> next;
	qi::rule<Iterator, std::shared_ptr<storm::property::prctl::Globally<double>>(), Skipper> globally;
	qi::rule<Iterator, std::shared_ptr<storm::property::prctl::BoundedUntil<double>>(), qi::locals< std::shared_ptr<storm::property::prctl::AbstractStateFormula<double>>>, Skipper> boundedUntil;
	qi::rule<Iterator, std::shared_ptr<storm::property::prctl::Until<double>>(), qi::locals< std::shared_ptr<storm::property::prctl::AbstractStateFormula<double>>>, Skipper> until;

	qi::rule<Iterator, std::shared_ptr<storm::property::prctl::AbstractRewardPathFormula<double>>(), Skipper> rewardPathFormula;
	qi::rule<Iterator, std::shared_ptr<storm::property::prctl::CumulativeReward<double>>(), Skipper> cumulativeReward;
	qi::rule<Iterator, std::shared_ptr<storm::property::prctl::ReachabilityReward<double>>(), Skipper> reachabilityReward;
	qi::rule<Iterator, std::shared_ptr<storm::property::prctl::InstantaneousReward<double>>(), Skipper> instantaneousReward;
	qi::rule<Iterator, std::shared_ptr<storm::property::prctl::SteadyStateReward<double>>(), Skipper> steadyStateReward;


	qi::rule<Iterator, std::string(), Skipper> freeIdentifierName;
	qi::rule<Iterator, storm::property::ComparisonType(), Skipper> comparisonType;
	qi::rule<Iterator, storm::property::action::SortAction<double>::SortingCategory(), Skipper> sortingCategory;
};

std::shared_ptr<storm::property::prctl::PrctlFilter<double>> PrctlParser::parsePrctlFormula(std::string formulaString) {
	// Prepare iterators to input.
	BaseIteratorType stringIteratorBegin = formulaString.begin();
	BaseIteratorType stringIteratorEnd = formulaString.end();
	PositionIteratorType positionIteratorBegin(stringIteratorBegin, stringIteratorEnd, formulaString);
	PositionIteratorType positionIteratorEnd;


	// Prepare resulting intermediate representation of input.
	std::shared_ptr<storm::property::prctl::PrctlFilter<double>> result_pointer(nullptr);

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
	if (!result_pointer) {
		throw storm::exceptions::WrongFormatException() << "Syntax error in formula";
	}

	return result_pointer;
}

} //namespace parser
} //namespace storm
