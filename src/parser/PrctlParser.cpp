#include "src/storage/SparseMatrix.h"
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
struct PrctlParser::PrctlGrammar : qi::grammar<Iterator, storm::formula::PctlFormula<double>*(), Skipper> {
	PrctlGrammar() : PrctlGrammar::base_type(start) {
		freeIdentifierName = qi::lexeme[(qi::alpha | qi::char_('_'))];

		//This block defines rules for parsing state formulas
		stateFormula %= (andFormula | atomicProposition | orFormula | notFormula | probabilisticBoundOperator | rewardBoundOperator);

		andFormula = (qi::lit("(") >> stateFormula >> qi::lit("&") >> stateFormula >> qi::lit(")"))[
		      qi::_val = phoenix::new_<storm::formula::And<double>>(qi::_1, qi::_2)];
		orFormula = (qi::lit("(") >> stateFormula >> '|' >> stateFormula >> ')')[qi::_val =
				phoenix::new_<storm::formula::Or<double>>(qi::_1, qi::_2)];
		notFormula = ('!' >> stateFormula)[qi::_val =
				phoenix::new_<storm::formula::Not<double>>(qi::_1)];
		atomicProposition = (freeIdentifierName)[qi::_val =
				phoenix::new_<storm::formula::Ap<double>>(qi::_1)];
		probabilisticBoundOperator = (
				("P>=" >> qi::double_ >> '[' >> pathFormula >> ']')[qi::_val =
						phoenix::new_<storm::formula::ProbabilisticBoundOperator<double> >(storm::formula::BoundOperator<double>::GREATER_EQUAL, qi::_1, qi::_2)] |
				("P<=" >> qi::double_ >> '[' >> pathFormula >> ']')[qi::_val =
						phoenix::new_<storm::formula::ProbabilisticBoundOperator<double> >(storm::formula::BoundOperator<double>::LESS_EQUAL, qi::_1, qi::_2)]
				);
		rewardBoundOperator = (
				("R>=" >> qi::double_ >> '[' >> pathFormula >> ']')[qi::_val =
						phoenix::new_<storm::formula::RewardBoundOperator<double> >(storm::formula::BoundOperator<double>::GREATER_EQUAL, qi::_1, qi::_2)] |
				("R<=" >> qi::double_ >> '[' >> pathFormula >> ']')[qi::_val =
						phoenix::new_<storm::formula::RewardBoundOperator<double> >(storm::formula::BoundOperator<double>::LESS_EQUAL, qi::_1, qi::_2)]
				);

		//This block defines rules for parsing formulas with noBoundOperators
		noBoundOperator %= (probabilisticNoBoundOperator | rewardNoBoundOperator);
		probabilisticNoBoundOperator = ("P=?[" >> pathFormula >> ']')[qi::_val =
				phoenix::new_<storm::formula::ProbabilisticNoBoundOperator<double> >(qi::_1)];
		rewardNoBoundOperator = ("R=?[" >> pathFormula >> ']')[qi::_val =
				phoenix::new_<storm::formula::RewardNoBoundOperator<double> >(qi::_1)];

		//This block defines rules for parsing path formulas
		pathFormula %= (eventually | boundedEventually | globally | boundedUntil | until);
		eventually = ('F' >> stateFormula)[qi::_val =
				phoenix::new_<storm::formula::Eventually<double> >(qi::_1)];
		boundedEventually = ("F<=" >> qi::int_ >> stateFormula)[qi::_val =
				phoenix::new_<storm::formula::BoundedEventually<double>>(qi::_2, qi::_1)];
		globally = ('G' >> stateFormula)[qi::_val =
				phoenix::new_<storm::formula::Globally<double> >(qi::_1)];
		until = (stateFormula >> 'U' >> stateFormula)[qi::_val =
				phoenix::new_<storm::formula::Until<double>>(qi::_1, qi::_2)];
		boundedUntil = (stateFormula >> "U<=" >> qi::int_ >> stateFormula)[qi::_val =
				phoenix::new_<storm::formula::BoundedUntil<double>>(qi::_1, qi::_3, qi::_2)];

		start %= (stateFormula | noBoundOperator);
	}

	qi::rule<Iterator, storm::formula::PctlFormula<double>*(), Skipper> start;

	qi::rule<Iterator, storm::formula::PctlStateFormula<double>*(), Skipper> stateFormula;
	qi::rule<Iterator, storm::formula::And<double>*(), Skipper> andFormula;
	qi::rule<Iterator, storm::formula::Ap<double>*(), Skipper> atomicProposition;
	qi::rule<Iterator, storm::formula::PctlStateFormula<double>*(), Skipper> orFormula;
	qi::rule<Iterator, storm::formula::PctlStateFormula<double>*(), Skipper> notFormula;
	qi::rule<Iterator, storm::formula::PctlStateFormula<double>*(), Skipper> probabilisticBoundOperator;
	qi::rule<Iterator, storm::formula::PctlStateFormula<double>*(), Skipper> rewardBoundOperator;

	qi::rule<Iterator, storm::formula::NoBoundOperator<double>*(), Skipper> noBoundOperator;
	qi::rule<Iterator, storm::formula::NoBoundOperator<double>*(), Skipper> probabilisticNoBoundOperator;
	qi::rule<Iterator, storm::formula::NoBoundOperator<double>*(), Skipper> rewardNoBoundOperator;

	qi::rule<Iterator, storm::formula::PctlPathFormula<double>*(), Skipper> pathFormula;
	qi::rule<Iterator, storm::formula::PctlPathFormula<double>*(), Skipper> boundedEventually;
	qi::rule<Iterator, storm::formula::PctlPathFormula<double>*(), Skipper> eventually;
	qi::rule<Iterator, storm::formula::PctlPathFormula<double>*(), Skipper> globally;
	qi::rule<Iterator, storm::formula::PctlPathFormula<double>*(), Skipper> boundedUntil;
	qi::rule<Iterator, storm::formula::PctlPathFormula<double>*(), Skipper> until;

	qi::rule<Iterator, std::string(), Skipper> freeIdentifierName;

};

} //namespace storm
} //namespace parser

storm::parser::PrctlParser::PrctlParser(std::string filename) {
	// Open file and initialize result.
	std::ifstream inputFileStream(filename, std::ios::in);

	// Prepare iterators to input.
	// TODO: Right now, this parses the whole contents of the file into a string first.
	// While this is usually not necessary, because there exist adapters that make an input stream
	// iterable in both directions without storing it into a string, using the corresponding
	// Boost classes gives an awful output under valgrind and is thus disabled for the time being.
	std::string fileContent((std::istreambuf_iterator<char>(inputFileStream)), (std::istreambuf_iterator<char>()));
	BaseIteratorType stringIteratorBegin = fileContent.begin();
	BaseIteratorType stringIteratorEnd = fileContent.end();
	PositionIteratorType positionIteratorBegin(stringIteratorBegin, stringIteratorEnd, filename);
	PositionIteratorType positionIteratorEnd;


	// Prepare resulting intermediate representation of input.
	storm::formula::PctlFormula<double>* result_pointer = nullptr;

	PrctlGrammar<PositionIteratorType,  BOOST_TYPEOF(boost::spirit::ascii::space)> grammar;

	qi::phrase_parse(positionIteratorBegin, positionIteratorEnd, grammar, boost::spirit::ascii::space, result_pointer);


}
