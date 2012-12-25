#include "src/parser/PrctlParser.h"

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
	
	struct SpiritParser : public grammar< char const* >
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
		storm::formula::PctlFormula<double>* result;
		
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
	
		SpiritParser() : SpiritParser::base_type(file, "PRCTL parser")
		{
			variable %= alnum;
			ws = *( space );
			value %= ( double_ | int_ ); // double_ must be *before* int_
			type = string("int") | string("double");
			
			/*
			 *	Todo:
			 *	Use two arguments at one point in the code, e.g. do something like
			 *		this->variables[ variable ] = value
			 *	
			 *	You can have local variables in rules, but somehow does not work.
			 *	You can also (somehow) let a rule return some arbitrary class and let boost magically collect the arguments for the constructor.
			 *	No idea how this can possibly work, did not get this to work.
			 *	You can also generate a syntax tree and do this manually (-> utree)... somehow.
			 *
			 *	Attention: spirit had some major upgrades in the last few releases. 1.48 already lacks some features that might be useful.
			 *
			 *	The rules parse the const definitions of
			 *	http://www.prismmodelchecker.org/manual/PropertySpecification/PropertiesFiles
			 *	We actually do not need them, but the problems I described above are fairly generic.
			 *	We will not be able to parse the formulas if we don't solve them...
			 *
			 *	Example input:
			 *		const int k = 7;
			 *		const double T = 9;
			 *		const double p = 0.01;
			 *
			 *	Parser can be run via ./storm --test-prctl <filename> foo bar
			 *		foo and bar are necessary, otherwise the option parser fails...
			 */
			
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

storm::parser::PrctlParser::PrctlParser(const char* filename)
{
	SpiritParser p;
	storm::parser::MappedFile file(filename);
	
	char* data = file.data;
	if (bs::qi::parse< char const* >(data, file.dataend, p))
	{
		std::cout << "File was parsed" << std::endl;
		std::string rest(data, file.dataend);
		std::cout << "Rest: " << rest << std::endl;
		this->formula = p.result;
	}
	else this->formula = NULL;
}
