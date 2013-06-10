/* 
 * File:   Includes
 * Author: Gereon Kremer
 *
 * Created on April 10, 2013, 4:46 PM
 */

#ifndef BOOSTINCLUDES_H
#define	BOOSTINCLUDES_H

// Used for Boost spirit.
#include <boost/typeof/typeof.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix.hpp>

// Include headers for spirit iterators. Needed for diagnostics and input stream iteration.
#include <boost/spirit/include/classic_position_iterator.hpp>
#include <boost/spirit/include/support_multi_pass.hpp>

namespace qi = boost::spirit::qi;
namespace phoenix = boost::phoenix;

typedef std::string::const_iterator BaseIteratorType;
typedef boost::spirit::classic::position_iterator2<BaseIteratorType> PositionIteratorType;
typedef PositionIteratorType Iterator;
typedef BOOST_TYPEOF(boost::spirit::ascii::space | qi::lit("//") >> *(qi::char_ - qi::eol) >> qi::eol) Skipper;
typedef BOOST_TYPEOF(qi::lit("//") >> *(qi::char_ - qi::eol) >> qi::eol | boost::spirit::ascii::space) Skipper2;
typedef boost::spirit::unused_type Unused;

#include "src/ir/IR.h"
using namespace storm::ir;
using namespace storm::ir::expressions;

#include "log4cplus/logger.h"
#include "log4cplus/loggingmacros.h"
extern log4cplus::Logger logger;

#endif	/* BOOSTINCLUDES_H */

