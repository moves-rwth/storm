#include "src/parser/readPctlFile.h"

#include <boost/spirit/include/classic_core.hpp>
#include <boost/spirit/include/qi_grammar.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/qi_char_class.hpp>

namespace bs = boost::spirit;
namespace bsq = bs::qi;

namespace
{
	using namespace bsq;
	struct Parser : public bsq::grammar< char const* >
	{
		typedef rule< char const* > rule_t;
	
		rule_t atom, term, formula;
	
		Parser() : Parser::base_type(formula, "PCTL parser")
		{
			atom = double_ | (char_('(') >> formula >> char_(')') );
			term = atom >> *( char_('*') >> atom );
			formula = term >> *( char_('+') >> term );
		}
	};
}

void readPctlFile(const char* filename)
{
	Parser p;
}