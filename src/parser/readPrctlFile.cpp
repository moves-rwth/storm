#include "src/parser/readPrctlFile.h"

#include "src/parser/parser.h"

#include <iostream>

#include <map>
//#include <pair>

#include <boost/spirit/include/classic_core.hpp>
#include <boost/spirit/include/qi_grammar.hpp>
#include <boost/spirit/include/qi_rule.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/qi_char_class.hpp>

#include <boost/bind.hpp>

namespace bs = boost::spirit;

namespace
{
	using namespace bs;
	using namespace bs::qi;
	using namespace bs::standard;
	
	struct PRCTLParser : public grammar< char const* >
	{
		typedef rule< char const* > rule_none;
		typedef rule< char const*, double() > rule_double;
		typedef rule< char const*, std::string() > rule_string;
			
		/*!
		 *	@brief Generic Nonterminals.
		 */
		rule_none ws;
		rule_string variable;
		rule_double value;
		
		/*!
		 *	@brief Nonterminals for file header.
		 */
		rule< char const* > varDef;
		rule_none type;
		
		/*!
		 *	@brief Nonterminals for formula.
		 */
		rule_none formula, opP;
		
		/*!
		 *	@brief Nonterminals for high-level file structure.
		 */
		rule_none file, header;
		
		/*!
		 *	@brief Variable assignments.
		 */
		std::map<std::string, double> variables;
		
		/*!
		 *	@brief Resulting formula.
		 */
		mrmc::formula::PCTLFormula<double>* result;
		
		struct dump
		{
			void print(double const& i, std::string& s)
			{
				std::cout << s << " = " << i << std::endl;
			}
			void operator()(double const& a, unused_type, unused_type) const
			{
				std::cout << a << std::endl;
			}
			void operator()(std::string const& a, unused_type, unused_type) const
			{
				std::cout << a << std::endl;
			}
			void operator()(utree const& a, unused_type, unused_type) const
			{
				std::cout << &a << std::endl;
			}
		};
	
		PRCTLParser() : PRCTLParser::base_type(file, "PRCTL parser")
		{
			variable %= alnum;
			ws = *( space );
			value %= ( double_ | int_ ); // double_ must be *before* int_
			type = string("int") | string("double");
			varDef = 
				string("const") >> ws >> 
				type >> ws >> 
				variable >> ws >>
				string("=") >> ws >>
				value >> ws >>
				string(";");
			
			header = +( varDef >> ws );
			
			file = header;
		}
		
		
	};
}

mrmc::formula::PCTLFormula<double>* mrmc::parser::readPrctlFile(const char* filename)
{
	PRCTLParser p;
	mrmc::parser::MappedFile file(filename);
	
	char* data = file.data;
	if (bs::qi::parse< char const* >(data, file.dataend, p))
	{
		std::cout << "File was parsed" << std::endl;
		std::string rest(data, file.dataend);
		std::cout << "Rest: " << rest << std::endl;
		return p.result;
	}
	else return NULL;
}
