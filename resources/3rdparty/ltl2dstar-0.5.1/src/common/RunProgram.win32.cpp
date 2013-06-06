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

/** @file
 * The win32 version of RunProgram.
 */

#include "common/RunProgram.hpp"

#ifdef RUNPROGRAM_WIN32

#include "common/Exceptions.hpp"

RunProgram::RunProgram(const std::string& program,
		       const std::vector<std::string>& arguments,
		       bool inheritStdStreams,
		       TempFile* std_in,
		       TempFile* std_out,
		       TempFile* std_err) {
  const char *filename=program.c_str();
  std::string command_line;
  command_line.append("\"");
  command_line.append(program);
  command_line.append("\"");

  for (unsigned int i=0;i<arguments.size();i++) {
    command_line.append(" \"");
    command_line.append(arguments[i]);
    command_line.append("\"");
  }

  STARTUPINFOA startUpInfo;
  PROCESS_INFORMATION processInfo;
  GetStartupInfoA(&startUpInfo);
  
  if (!inheritStdStreams) {
    startUpInfo.hStdInput=INVALID_HANDLE_VALUE;
    startUpInfo.hStdOutput=INVALID_HANDLE_VALUE;
    startUpInfo.hStdError=INVALID_HANDLE_VALUE;
    startUpInfo.dwFlags|=STARTF_USESTDHANDLES;

    if (std_in!=0) {
      startUpInfo.hStdInput=std_in->getFileHandle();
    }

    if (std_out!=0) {
      startUpInfo.hStdOutput=std_out->getFileHandle();
    }

    if (std_err!=0) {
      startUpInfo.hStdError=std_err->getFileHandle();
    }
  }

  if (CreateProcessA((CHAR*)filename,
		    (CHAR*)command_line.c_str(),
		    0,  // process attributes
		    0,  // thread attributes
		    TRUE,  // inherit handles
		    0,  // creation flags
		    0,  // environment
		    0,  // current directory
		    &startUpInfo,
		    &processInfo)) {
    _id=processInfo.hProcess;
    CloseHandle(processInfo.hThread);
  } else {
    THROW_EXCEPTION(Exception, "CreateProcess failed...");
  }
}

RunProgram::~RunProgram() {
  CloseHandle(_id);
}

int 
RunProgram::waitForTermination() {
  DWORD exit_code;
  do {
    WaitForSingleObject(_id, INFINITE);
    GetExitCodeProcess(_id, &exit_code);
  } while (exit_code == STILL_ACTIVE);
  return exit_code;
}


#endif
