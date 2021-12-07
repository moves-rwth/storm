// -*- coding: utf-8 -*-
// Copyright (C) 2015, 2017, 2018 Laboratoire de Recherche et Développement de
// l'Epita (LRDE).
//
// This file is part of Spot, a model checking library.
//
// Spot is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3 of the License, or
// (at your option) any later version.
//
// Spot is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
// or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
// License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#pragma once

#include "common_sys.hh"
#include "error.h"

#include <argp.h>
#include <cstring>
#include <sys/stat.h>
#include <unistd.h>
#include <iostream>
#include <fstream>

#include "common_finput.hh"
#include <spot/parseaut/public.hh>


extern const struct argp hoaread_argp;

extern spot::automaton_parser_options opt_parse;

spot::twa_graph_ptr
read_automaton(const char* filename, spot::bdd_dict_ptr& dict);


class hoa_processor: public job_processor
{
protected:
  spot::bdd_dict_ptr dict_;
  bool abort_on_error_;
public:

  hoa_processor(spot::bdd_dict_ptr dict, bool abort_on_error = false)
    : dict_(dict), abort_on_error_(abort_on_error)
  {
  }

  int
  process_formula(spot::formula, const char*, int) override
  {
    SPOT_UNREACHABLE();
  }

  virtual int
  process_automaton(const spot::const_parsed_aut_ptr& haut) = 0;

  int process_string(const std::string& input, const char* filename,
                     int linenum) override
  {
    std::ostringstream loc;
    loc << filename << ':' << linenum;
    std::string locstr = loc.str();
    return process_automaton_stream
      (spot::automaton_stream_parser(input.c_str(), locstr, opt_parse));
  }

  int
  aborted(const spot::const_parsed_aut_ptr& h)
  {
    std::cerr << h->filename << ':' << h->loc << ": aborted input automaton\n";
    return 2;
  }

  int
  process_file(const char* filename) override
  {
    // If we have a filename like "foo/NN" such
    // that:
    // ① foo/NN is not a file,
    // ② NN is a number,
    // ③ foo is a file,
    // then it means we want to open foo as
    // a CSV file and process column NN.
    if (const char* slash = strrchr(filename, '/'))
      {
        char* end;
        errno = 0;
        long int col = strtol(slash + 1, &end, 10);
        if (errno == 0 && !*end && col != 0)
          {
            struct stat buf;
            if (stat(filename, &buf) != 0)
              {
                col_to_read = col;
                if (real_filename)
                  free(real_filename);
                real_filename = strndup(filename, slash - filename);

                // Special case for stdin.
                if (real_filename[0] == '-' && real_filename[1] == 0)
                  return process_stream(std::cin, real_filename);

                std::ifstream input(real_filename);
                if (input)
                  return process_stream(input, real_filename);

                error(2, errno, "cannot open '%s' nor '%s'",
                      filename, real_filename);
              }
          }
      }

    return process_automaton_stream(spot::automaton_stream_parser(filename,
                                                                  opt_parse));
  }

  int process_automaton_stream(spot::automaton_stream_parser&& hp)
  {
    int err = 0;
    while (!abort_run)
      {
        auto haut = hp.parse(dict_);
        if (!haut->aut && haut->errors.empty())
          break;
        if (haut->format_errors(std::cerr))
          err = 2;
        if (!haut->aut || (err && abort_on_error_))
          error(2, 0, "failed to read automaton from %s",
                haut->filename.c_str());
        else if (haut->aborted)
          err = std::max(err, aborted(haut));
        else
          process_automaton(haut);
      }
    return err;
  }
};
