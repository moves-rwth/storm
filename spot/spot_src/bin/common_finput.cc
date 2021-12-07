// -*- coding: utf-8 -*-
// Copyright (C) 2012-2017, 2019 Laboratoire de Recherche et
// Développement de l'Epita (LRDE).
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

#include "common_finput.hh"
#include "common_setup.hh"
#include "error.h"

#include <fstream>
#include <cstring>
#include <unistd.h>

enum {
  OPT_LBT = 1,
  OPT_LENIENT,
};

jobs_t jobs;
bool lbt_input = false;
static bool lenient = false;

static const argp_option options[] =
  {
    { nullptr, 0, nullptr, 0, "Input options:", 1 },
    { "formula", 'f', "STRING", 0, "process the formula STRING", 0 },
    { "file", 'F', "FILENAME[/COL]", 0,
      "process each line of FILENAME as a formula; if COL is a "
      "positive integer, assume a CSV file and read column COL; use "
      "a negative COL to drop the first line of the CSV file", 0 },
    { "lbt-input", OPT_LBT, nullptr, 0,
      "read all formulas using LBT's prefix syntax", 0 },
    { "lenient", OPT_LENIENT, nullptr, 0,
      "parenthesized blocks that cannot be parsed as subformulas "
      "are considered as atomic properties", 0 },
    { nullptr, 0, nullptr, 0, nullptr, 0 }
  };

const struct argp finput_argp = { options, parse_opt_finput,
                                  nullptr, nullptr, nullptr,
                                  nullptr, nullptr };

const struct argp finput_argp_headless = { options + 1, parse_opt_finput,
                                           nullptr, nullptr, nullptr,
                                           nullptr, nullptr };


int
parse_opt_finput(int key, char* arg, struct argp_state*)
{
  // Called from C code, so should not raise any exception.
  BEGIN_EXCEPTION_PROTECT;
  // This switch is alphabetically-ordered.
  switch (key)
    {
    case 'f':
      jobs.emplace_back(arg, false);
      break;
    case 'F':
      jobs.emplace_back(arg, true);
      break;
    case OPT_LBT:
      lbt_input = true;
      break;
    case OPT_LENIENT:
      lenient = true;
      break;
    default:
      return ARGP_ERR_UNKNOWN;
    }
  END_EXCEPTION_PROTECT;
  return 0;
}

spot::parsed_formula
parse_formula(const std::string& s)
{
  if (lbt_input)
    return spot::parse_prefix_ltl(s);
  else
    return spot::parse_infix_psl
      (s, spot::default_environment::instance(), false, lenient);
}

job_processor::job_processor()
  : abort_run(false), real_filename(nullptr),
    col_to_read(0), prefix(nullptr), suffix(nullptr)
{
}

job_processor::~job_processor()
{
  if (real_filename)
    free(real_filename);
  if (prefix)
    free(prefix);
  if (suffix)
    free(suffix);
}


int
job_processor::process_string(const std::string& input,
                              const char* filename,
                              int linenum)
{
  auto pf = parse_formula(input);

  if (!pf.f || !pf.errors.empty())
    {
      if (filename)
        error_at_line(0, 0, filename, linenum, "parse error:");
      pf.format_errors(std::cerr);
      return 1;
    }
  return process_formula(pf.f, filename, linenum);
}

int
job_processor::process_stream(std::istream& is,
                              const char* filename)
{
  int error = 0;
  int linenum = 1;
  std::string line;

  // Discard the first line of a CSV file if requested.
  if (col_to_read < 0)
    {
      std::getline(is, line);
      col_to_read = -col_to_read;
      ++linenum;
    }

  // Each line of the file and send them to process_string,
  // optionally extracting a column of a CSV file.
  while (!abort_run && std::getline(is, line))
    if (!line.empty())
      {
        if (col_to_read == 0)
          {
            error |= process_string(line, filename, linenum++);
          }
        else // We are reading column COL_TO_READ in a CSV file.
          {
            // Some people save CSV files with MSDOS encoding, and
            // we don't want to include the \r in any %> output.
            if (*line.rbegin() == '\r')
              line.pop_back();

            // If the line we have read contains an odd number
            // of double-quotes, then it is an incomplete CSV line
            // that should be completed by the next lines.
            unsigned dquotes = 0;
            std::string fullline;
            unsigned csvlines = 0;
            do
              {
                ++csvlines;
                size_t s = line.size();
                for (unsigned i = 0; i < s; ++i)
                  dquotes += line[i] == '"';
                if (fullline.empty())
                  fullline = line;
                else
                  (fullline += '\n') += line;
                if (!(dquotes &= 1))
                  break;
              }
            while (std::getline(is, line));
            if (dquotes)
              error_at_line(2, errno, filename, linenum,
                            "mismatched double-quote, "
                            "reached EOF while parsing this line");

            // Now that we have a full CSV line, extract the right
            // column.

            const char* str = fullline.c_str();
            const char* col1_start = str;
            // Delimiters for the extracted column.
            const char* coln_start = str;
            const char* coln_end = nullptr;
            // The current column.  (1-based)
            int colnum = 1;
            // Whether we are parsing a double-quoted string.
            bool instring = false;
            // Note that RFC 4180 has strict rules about
            // double-quotes: ① if a field is double-quoted, the first
            // and last characters of the field should be
            // double-quotes; ② if a field contains a double-quote
            // then it should be double quoted, and the occurrences
            // of double-quotes should be doubled.  Therefore a CSV file
            // may no contain a line such as:
            //    foo,bar"ba""z",12
            // Tools have different interpretation of such a line.
            // For instance Python's pandas.read_csv() function will
            // load the second field verbatim as the string 'bar"ba""z"',
            // while R's read.csv() function will load it as the
            // string 'barba"z'.  We use this second interpretation, because
            // it also makes it possible to parse CSVs fields formatted
            // with leading spaces (often for cosmetic purpose).  When
            // extracting the second field of
            //    foo, "ba""z", 12
            // we will return ' baz' and the leading space will be ignored
            // by our LTL formula parser.
            while (*str)
              {
                switch (*str)
                  {
                  case '"':
                    // Doubled double-quotes are used to escape
                    // double-quotes.
                    if (instring && str[1] == '"')
                      ++str;
                    else
                      instring = !instring;
                    break;
                  case ',':
                    if (!instring)
                      {
                        if (col_to_read == colnum)
                          coln_end = str;
                        ++colnum;
                        if (col_to_read == colnum)
                          coln_start = str + 1;
                      }
                    break;
                  }
                // Once we have the end delimiter for our target
                // column, we have all we need.
                if (coln_end)
                  break;
                ++str;
              }
            if (!*str)
              {
                if (colnum != col_to_read)
                  // Skip this line as it has no enough columns.
                  continue;
                else
                  // The target columns ends at the end of the line.
                  coln_end = str;
              }

            // Skip the line if it has an empty field.
            if (coln_start == coln_end)
              continue;

            // save the contents before and after that columns for the
            // %< and %> escapes (ignoring the trailing and leading
            // commas).
            prefix = (col_to_read != 1) ?
              strndup(col1_start, coln_start - col1_start - 1) : nullptr;
            suffix = (*coln_end != 0) ? strdup(coln_end + 1) : nullptr;
            std::string field(coln_start, coln_end);
            // Remove double-quotes if any.
            if (field.find('"') != std::string::npos)
              {
                unsigned dst = 0;
                bool instring = false;
                for (; coln_start != coln_end; ++coln_start)
                  if (*coln_start == '"')
                    // A doubled double-quote instead a double-quoted
                    // string is an escaped double-quote.
                    if (instring && coln_start[1] == '"')
                      field[dst++] = *++coln_start;
                    else
                      instring = !instring;
                  else
                    field[dst++] = *coln_start;
                field.resize(dst);
              }
            error |= process_string(field, filename, linenum);
            linenum += csvlines;
            if (prefix)
              {
                free(prefix);
                prefix = nullptr;
              }
            if (suffix)
              {
                free(suffix);
                suffix = nullptr;
              }
          }
      }
  return error;
}

int
job_processor::process_file(const char* filename)
{
  // Special case for stdin.
  if (filename[0] == '-' && filename[1] == 0)
    return process_stream(std::cin, filename);

  errno = 0;
  std::ifstream input(filename);
  if (input)
    return process_stream(input, filename);
  int saved_errno = errno;

  // If we have a filename like "foo/NN" such
  // that:
  // ① foo/NN is not a file (already the case),
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

  error(2, saved_errno, "cannot open '%s'", filename);
  return -1;
}

int
job_processor::run()
{
  int error = 0;
  for (auto& j: jobs)
    {
      if (!j.file_p)
        error |= process_string(j.str);
      else
        error |= process_file(j.str);
      if (abort_run)
        break;
    }
  return error;
}

void check_no_formula()
{
  if (!jobs.empty())
    return;
  if (isatty(STDIN_FILENO))
    error(2, 0, "No formula to translate?  Run '%s --help' for help.\n"
          "Use '%s -' to force reading formulas from the standard "
          "input.", program_name, program_name);
  jobs.emplace_back("-", true);
}

void check_no_automaton()
{
  if (!jobs.empty())
    return;
  if (isatty(STDIN_FILENO))
    error(2, 0, "No automaton to process?  Run '%s --help' for help.\n"
          "Use '%s -' to force reading automata from the standard "
          "input.", program_name, program_name);
  jobs.emplace_back("-", true);
}
