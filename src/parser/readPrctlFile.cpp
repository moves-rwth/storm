#include "src/parser/readPrctlFile.h"

#include "src/parser/parser.h"

#include <iostream>

#include <boost/spirit/include/classic_core.hpp>
#include <boost/spirit/include/qi_grammar.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/qi_char_class.hpp>

namespace bs = boost::spirit;

namespace
{
	using namespace bs::qi;
	using namespace bs::standard;
	
	struct PRCTLParser : public grammar< char const* >
	{
		typedef rule< char const* > rule_t;
	
		/*!
		 *	@brief Generic Nonterminals.
		 */
		rule_t variable, value;
		
		/*!
		 *	@brief Nonterminals for file header.
		 */
		rule_t varDef, type;
		
		/*!
		 *	@brief Nonterminals for formula.
		 */
		rule_t formula, opP;
		
		/*!
		 *	@brief Nonterminals for high-level file structure.
		 */
		rule_t file, header;
	
		PRCTLParser() : PRCTLParser::base_type(file, "PRCTL parser")
		{
			variable = alnum;
			value = int_ | double_;
			type = string("int") | string("double");
			varDef = string("const") >> type >> variable >> string("=") >> value >> string(";");
			
			header = *( varDef );
			
			file = header;
		}
	};
}

void readPrctlFile(const char* filename)
{
	PRCTLParser p;
	mrmc::parser::MappedFile file(filename);
	
	if (bs::qi::parse< char const* >(file.data, file.dataend, p))
	{
		std::cout << "File was parsed" << std::endl;
	}
}