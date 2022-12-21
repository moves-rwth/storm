/* This file is part of the Multiagent Decision Process (MADP) Toolbox. 
 *
 * The majority of MADP is free software released under GNUP GPL v.3. However,
 * some of the included libraries are released under a different license. For 
 * more information, see the included COPYING file. For other information, 
 * please refer to the included README file.
 *
 * This file has been written and/or modified by the following people:
 *
 * Frans Oliehoek 
 * Matthijs Spaan 
 *
 * For contact information please see the included AUTHORS file.
 */

/* Only include this header file once. */
#ifndef _COMMENTORBLANKPARSER_H_
#define _COMMENTORBLANKPARSER_H_ 1

#if USE_BOOST_SPIRIT_CLASSIC
#include "boost/spirit/include/classic_core.hpp"
#else
#include "boost/spirit/core.hpp"
#endif

namespace boost { namespace spirit
#if USE_BOOST_SPIRIT_CLASSIC
    { namespace classic
#endif
{
    template <typename DerivedT>
        struct sub_grammar : public parser<DerivedT>
    {
	typedef sub_grammar self_t;
	typedef DerivedT const& embed_t;

	template <typename ScannerT>
	struct result
	{
	    typedef typename parser_result<
		typename DerivedT::start_t, ScannerT>::type
	    type;
	};

	DerivedT const& derived() const
	{ return *static_cast<DerivedT const*>(this); }

	template <typename ScannerT>
	typename parser_result<self_t, ScannerT>::type
	parse(ScannerT const& scan) const
	{
	    return derived().start.parse(scan);
	}
    };
}}
#if USE_BOOST_SPIRIT_CLASSIC
}
#endif

#if USE_BOOST_SPIRIT_CLASSIC
using namespace boost::spirit::classic;
#else
using namespace boost::spirit;
#endif

/* aliases */

/* constants */
#define DEBUG_COMPARS 0
#define DEBUG_COBP 0

namespace comment_cobp {
    typedef char                    char_t;
    //typedef file_iterator<char_t>   iterator_t;
    typedef file_iterator<char_t>   iterator_t_fi;
    typedef position_iterator<iterator_t_fi>  iterator_t;
    typedef scanner<iterator_t>     scanner_t;
    typedef rule<scanner_t>         rule_t;
   
namespace{    
    void    cp_eol(iterator_t, iterator_t)
    { if(DEBUG_COMPARS)	std::cout << "EOL\n"; }
    //void    cp_endinput(iterator_t, iterator_t)
    //{ if(DEBUG_COMPARS)	std::cout << "END OF INPUT\n"; }
    void    cp_startcomment(char_t)
    { if(DEBUG_COMPARS) std::cout<< "#-STARTCOMMENT\n"; }

    void    cobp_blank(char_t)    
    {if(DEBUG_COBP) std::cout << "SKIPPED BLANK\n";};
    //void    cobp_emptyline(iterator_t str, iterator_t end)    
    //{ if(DEBUG_COBP) std::cout << "SKIPPED EMPTYLINE\n"; };
    void    cobp_comment(iterator_t str, iterator_t end)
    {
        std::string  s(str, end);
	if(DEBUG_COBP)
	    std::cout<< "SKIPPED COMMENT: \""<< s << "\""<< std::endl; 
    };
}

//CommentParser
struct CommentParser : public sub_grammar<CommentParser>
{    
    typedef
	sequence<sequence<action<chlit<char>, void (*)(comment_cobp::char_t)>, kleene_star<alternative<print_parser, blank_parser> > >, kleene_star<action<eol_parser, void (*)(comment_cobp::iterator_t, comment_cobp::iterator_t)> > >
    start_t;

    CommentParser() 
    : start
	(   //commenttoken 
	    ch_p('#')[&cp_startcomment]
	    >> 
	    //lineremainder
	    *( print_p | /*  alnum_p |*/ blank_p ) 
	    >>	
	    *(   eol_p[&cp_eol] )
	)
    {}

    start_t start;
};

CommentParser const commentParser_p = CommentParser();
struct CommentOrBlankParser : public sub_grammar<CommentOrBlankParser>
{

    typedef
	alternative<action<blank_parser, void (*)(comment_cobp::char_t)>, action<comment_cobp::CommentParser, void (*)(comment_cobp::iterator_t, comment_cobp::iterator_t)> >
    start_t;

    CommentOrBlankParser()
    : start
	(
	    blank_p[&cobp_blank]
	    |
	    commentParser_p[&cobp_comment]
	)
    {}

    start_t start;

};
CommentOrBlankParser const commentOrBlankParser_p = CommentOrBlankParser();

}
#endif /* !_COMMENTORBLANKPARSER_H_ */

