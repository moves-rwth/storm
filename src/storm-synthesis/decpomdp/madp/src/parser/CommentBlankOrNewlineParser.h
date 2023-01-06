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
#ifndef _COMMENTBLANKORNEWLINEPARSER_H_
#define _COMMENTBLANKORNEWLINEPARSER_H_ 1

#if USE_BOOST_SPIRIT_CLASSIC
#include "boost/spirit/include/classic_core.hpp"
#else
#include "boost/spirit/core.hpp"
#endif

#include "CommentOrBlankParser.h"

#if USE_BOOST_SPIRIT_CLASSIC
using namespace boost::spirit::classic;
#else
using namespace boost::spirit;
#endif

/* aliases */

/* constants */
#define DEBUG_CBONL_COMPARS 0
#define DEBUG_CBONL_COBP 0

namespace comment_cbonlp {
    typedef char                    char_t;
    //typedef file_iterator<char_t>   iterator_t;
    typedef file_iterator<char_t>   iterator_t_fi;
    typedef position_iterator<iterator_t_fi>  iterator_t;
    typedef scanner<iterator_t>     scanner_t;
    typedef rule<scanner_t>         rule_t;
   
namespace{    
    void    cbonlp_eol(iterator_t, iterator_t)
    { 
        if(DEBUG_CBONL_COMPARS)	
            std::cout << "EOL\n"; 
    }
    void    cbonlp_comment_or_blank(iterator_t str, iterator_t end)
    {
	if(DEBUG_CBONL_COBP)
        {
            std::string  s(str, end);
	    std::cout<< "SKIPPED COMMENT: \""<< s << "\""<< std::endl; 
        }
    };
}

struct CommentBlankorNewLineParser : public sub_grammar<CommentBlankorNewLineParser>
{
    /*typedef int start_t; //<- used to infer the following type:*/
    typedef 
            alternative<
                action<
                    comment_cobp::CommentOrBlankParser, 
                    void (*)(position_iterator<
                        file_iterator<char> 
                    >, 
                    position_iterator<
                        file_iterator<char> 
                    >)
                >,
                action<
                    eol_parser, 
                    void (*)(position_iterator<
                        file_iterator<char> 
                    >, 
                    position_iterator<
                        file_iterator<char> 
                    >)
                > 
            > 
            start_t;

    CommentBlankorNewLineParser()
    : start
	(
	    comment_cobp::commentOrBlankParser_p[&cbonlp_comment_or_blank]
            |
            eol_p[&cbonlp_eol]
	)
    {}

    start_t start;

};

}
#endif /* !_COMMENTBLANKORNEWLINEPARSER_H_ */

