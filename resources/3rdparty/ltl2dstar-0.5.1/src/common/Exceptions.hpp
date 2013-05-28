/*
 * This file is part of the program ltl2dstar (http://www.ltl2dstar.de/).
 * Copyright (C) 2005-2007 Joachim Klein <j.klein@ltl2dstar.de>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as 
 *  published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */


/**
 * Exception framework
 */

#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

/** @file
 * Provides exception framework for ltl2dstar
 */

#include <exception>
#include <string>
#include <boost/lexical_cast.hpp>
#include <iostream>

/** @def THROW_EXCEPTION(exception, message)
 * Throw an exception while automatically keeping track of the 
 * file and line number using the macros __FILE__ and __LINE__
 */
#define THROW_EXCEPTION(exception, message) throw exception(message, __FILE__, __LINE__)


/**
 * Exception base class
 */
class Exception : public std::exception {
public:
  /** Constructor
   * @param msg A message with the reason for the exception
   * @param file the filename where the exception was thrown
   * @param line the linenumber where the exception was thrown
   */
  explicit Exception(const std::string& msg, 
		     const char* file=0,
		     unsigned int line=0) :
    _message(msg), _file(file), _line(line) {
  }

  /** Destructor */
  virtual ~Exception() throw () {}

  /** Get message */
  virtual const char *what() const throw() {
    return _message.c_str(); 
  } 

  /** Print an explanation for this exception on an output stream*/
  virtual void print(std::ostream& out) {
    out << "Exception";
    if (_file!=0) {
      out << " in file " << _file;
      out << " at line " << _line;
    }
    out << ":" << std::endl;

    out << _message << std::endl;
  }

 private:
  /** The message */
  std::string _message;
  /** The filename*/
  const char *_file;
  /** The line number */
  unsigned int _line;
};


/**
 * IllegalArgumentException is thrown when an argument to a function
 * is malformed, has the wrong type or lacks other required properties.
 */
class IllegalArgumentException : public Exception {
 public:
  /** Constructor
   * @param msg A message with the reason for the exception
   * @param file the filename where the exception was thrown
   * @param line the linenumber where the exception was thrown
   */
  explicit IllegalArgumentException(const std::string& msg,
				    const char* file=0,
				    unsigned int line=0)
    : Exception(msg, file, line) {};
};

/**
 * IndexOutOfBoundsException is thrown when an index is out of bounds
 */
class IndexOutOfBoundsException : public Exception {
 public:
  /** Constructor
   * @param msg A message with the reason for the exception
   * @param file the filename where the exception was thrown
   * @param line the linenumber where the exception was thrown
   */
  IndexOutOfBoundsException(const std::string& msg,
			    const char* file=0,
			    unsigned int line=0)
    : Exception(msg, file, line) {};
};


/**
 * LimitReachedException is thrown when a specified limit on the number of states
 * was reached in the construction of an automaton.
 */
class LimitReachedException : public Exception {
 public:
  /** Constructor
   * @param msg A message with the reason for the exception
   * @param file the filename where the exception was thrown
   * @param line the linenumber where the exception was thrown
   */
  LimitReachedException(const std::string& msg,
			const char* file=0,
			unsigned int line=0)
    : Exception(msg, file, line) {};
};

/**
 * CmdLineException is thrown when there are errors in the command line parameters
 */
class CmdLineException : public Exception {
 public:
  /** Constructor
   * @param msg A message with the reason for the exception
   * @param file the filename where the exception was thrown
   * @param line the linenumber where the exception was thrown
   */
  CmdLineException(const std::string& msg,
		   const char* file=0,
		   unsigned int line=0)
    : Exception(msg, file, line) {};
};



#endif
