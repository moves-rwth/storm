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


#ifndef RUNPROGRAM_HPP
#define RUNPROGRAM_HPP

/** @file
 * Provides OS independent interface to run external tools.
 */

#include "common/TempFile.hpp"

#include <string>
#include <vector>
#include <cstdio>

#if (__WIN32__ || _WIN32)
 #include <windows.h>
 typedef HANDLE processID_t;

 #define RUNPROGRAM_WIN32 1
#else
 #include <sys/types.h>
 #include <fcntl.h>
 #include <sys/wait.h>
 #include <unistd.h>
 typedef pid_t   processID_t;

 #define RUNPROGRAM_POSIX 1
#endif

/** OS independent way to run external programs */
class RunProgram {
public:
  /** 
   * Constructor, runs external program.
   * @param program the path to the executable
   * @param arguments a vector of arguments
   * @param inheritStdStreams inherit the stdin, stdout, stdout streams from 
   *                          calling program?
   * @param std_in Redirect stdin of external program from TempFile
   * @param std_out Redirect stdout of external program to TempFile
   * @param std_err Redirect stderr stream of external program to TempFile
   **/
  RunProgram(const std::string& program,
	     const std::vector<std::string>& arguments,
	     bool inheritStdStreams=false,
	     TempFile *std_in=0,
	     TempFile *std_out=0,
	     TempFile *std_err=0);
  
  /** Destructor */
  ~RunProgram();

  /** Wait for child program to terminate and return exit code */
  int waitForTermination();

  /** Call extern program */
  static int system(const std::string& program,
		    const std::vector<std::string>& arguments) {
    return RunProgram(program, arguments).waitForTermination();
  }
private:
  processID_t _id;
};

#endif
