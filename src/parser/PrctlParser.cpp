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
typedef std::istreambuf_iterator<char> base_iterator_type;
typedef boost::spirit::multi_pass<base_iterator_type> forward_iterator_type;
typedef boost::spirit::classic::position_iterator2<forward_iterator_type> pos_iterator_type;
namespace qi = boost::spirit::qi;
namespace phoenix = boost::phoenix;

namespace storm {

namespace parser {

template<typename Iterator, typename Skipper>
struct PrctlParser::PrctlGrammar : qi::grammar<Iterator, storm::formula::PctlFormula<double>*(), Skipper> {
	PrctlGrammar() : PrctlGrammar::base_type(start) {
		freeIdentifierName = qi::lexeme[(qi::alpha | qi::char_('_'))];

		//This block defines rules for parsing state formulas
		stateFormula = (andFormula | atomicProposition | orFormula | notFormula | probabilisticBoundOperator | rewardBoundOperator);
		andFormula = ("(" << stateFormula << "&" << stateFormula << ")")[qi::_val =
				phoenix::new_<storm::formula::And<double>>(qi::_1, qi::_2)];
		orFormula = ('(' << stateFormula << '|' << stateFormula << ')')[qi::_val =
				phoenix::new_<storm::formula::Or<double>>(qi::_1, qi::_2)];
		notFormula = ('!' << stateFormula)[qi::_val =
				phoenix::new_<storm::formula::Not<double>>(qi::_1)];
		atomicProposition = (freeIdentifierName)[qi::_val =
				phoenix::new_<storm::formula::Ap<double>>(qi::_1)];
		probabilisticBoundOperator = (
				("P=" << qi::double_ << '[' << pathFormula << ']') [qi::_val =
						phoenix::new_<storm::formula::ProbabilisticBoundOperator<double>>(qi::_1, qi::_1, qi::_2)] |
				("P[" << qi::double_ << qi::double_ << ']' << '[' << pathFormula << ']')[qi::_val =
						phoenix::new_<storm::formula::ProbabilisticBoundOperator<double> >(qi::_1, qi::_2, qi::_3)] |
				("P>=" << qi::double_ << '[' << pathFormula << ']')[qi::_val =
						phoenix::new_<storm::formula::ProbabilisticBoundOperator<double> >(qi::_1, 1, qi::_2)] |
				("P<=" << qi::double_ << '[' << pathFormula << ']')[qi::_val =
						phoenix::new_<storm::formula::PctlStateFormula<double> >(0, qi::_1, qi::_2)]
				);
		rewardBoundOperator = (
				("R=" << qi::double_ << '[' << pathFormula << ']') [qi::_val =
						phoenix::new_<storm::formula::RewardBoundOperator<double> >(qi::_1, qi::_1, qi::_2)] |
				("R[" << qi::double_ << qi::double_ << ']' << '[' << pathFormula << ']')[qi::_val =
						phoenix::new_<storm::formula::RewardBoundOperator<double> >(qi::_1, qi::_2, qi::_3)] |
				("R>=" << qi::double_ << '[' << pathFormula << ']')[qi::_val =
						phoenix::new_<storm::formula::RewardBoundOperator<double> >(qi::_1, storm::utility::constGetInfinity<double>(), qi::_2)] |
				("R<=" << qi::double_ << '[' << pathFormula << ']')[qi::_val =
						phoenix::new_<storm::formula::RewardBoundOperator<double> >(0, qi::_1, qi::_2)]
				);

		//This block defines rules for parsing formulas with noBoundOperators
		noBoundOperator %= (probabilisticNoBoundOperator | rewardNoBoundOperator);
		probabilisticNoBoundOperator = ("P=?[" << pathFormula << ']')[qi::_val =
				phoenix::new_<storm::formula::ProbabilisticNoBoundOperator<double> >(qi::_1)];
		rewardNoBoundOperator = ("R=?[" << pathFormula << ']')[qi::_val =
				phoenix::new_<storm::formula::RewardNoBoundOperator<double> >(qi::_1)];

		//This block defines rules for parsing path formulas
		pathFormula %= (eventually | boundedEventually | globally | boundedUntil | until);
		eventually = ('F' << pathFormula)[qi::_val =
				phoenix::new_<storm::formula::Eventually<double> >(qi::_1)];
		boundedEventually = ("F<=" << qi::double_ << pathFormula)[qi::_val =
				phoenix::new_<storm::formula::BoundedEventually<double>>(qi::_2, qi::_1)];
		globally = ('G' << pathFormula)[qi::_val =
				phoenix::new_<storm::formula::Globally<double> >(qi::_1)];
		until = (stateFormula << 'U' << stateFormula)[qi::_val =
				phoenix::new_<storm::formula::Until<double>>(qi::_1, qi::_2)];
		boundedUntil = (stateFormula << "U<=" << qi::double_ << stateFormula)[qi::_val =
				phoenix::new_<storm::formula::BoundedUntil<double>>(qi::_1, qi::_3, qi::_2)];

		start %= (stateFormula | noBoundOperator);
	}

	qi::rule<Iterator, storm::formula::PctlFormula<double>*(), Skipper> start;

	qi::rule<Iterator, storm::formula::PctlStateFormula<double>*(), Skipper> stateFormula;
	qi::rule<Iterator, storm::formula::And<double>*(), Skipper> andFormula;
	qi::rule<Iterator, storm::formula::PctlStateFormula<double>*(), Skipper> atomicProposition;
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

storm::parser::PrctlParser::PrctlParser(std::string filename)
{
	// Open file and initialize result.
	std::ifstream inputFileStream(filename, std::ios::in);

	// Prepare iterators to input.
	base_iterator_type in_begin(inputFileStream);
	forward_iterator_type fwd_begin = boost::spirit::make_default_multi_pass(in_begin);
	forward_iterator_type fwd_end;
	pos_iterator_type position_begin(fwd_begin, fwd_end, filename);
	pos_iterator_type position_end;

	// Prepare resulting intermediate representation of input.
	storm::formula::PctlFormula<double>* result_pointer;

	PrctlGrammar<pos_iterator_type,  BOOST_TYPEOF(boost::spirit::ascii::space)> grammar;

	qi::phrase_parse(position_begin, position_end, grammar, boost::spirit::ascii::space, result_pointer);


}
