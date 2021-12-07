// -*- coding: utf-8 -*-
// Copyright (C) 2012-2019 Laboratoire de Recherche et Développement
// de l'Epita (LRDE).
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

#include "common_sys.hh"
#include "common_output.hh"
#include "common_aoutput.hh"
#include "common_setup.hh"
#include <iostream>
#include <sstream>
#include <spot/tl/print.hh>
#include <spot/tl/length.hh>
#include <spot/tl/apcollect.hh>
#include <spot/tl/hierarchy.hh>
#include <spot/misc/formater.hh>
#include <spot/misc/escape.hh>
#include "common_cout.hh"
#include "error.h"

enum {
  OPT_CSV = 1,
  OPT_FORMAT,
  OPT_LATEX,
  OPT_SPOT,
  OPT_WRING,
};

output_format_t output_format = spot_output;
bool full_parenth = false;
bool escape_csv = false;
char output_terminator = '\n';

static const argp_option options[] =
  {
    { "full-parentheses", 'p', nullptr, 0,
      "output fully-parenthesized formulas", -20 },
    { "spin", 's', nullptr, 0, "output in Spin's syntax", -20 },
    { "spot", OPT_SPOT, nullptr, 0, "output in Spot's syntax (default)", -20 },
    { "lbt", 'l', nullptr, 0, "output in LBT's syntax", -20 },
    { "wring", OPT_WRING, nullptr, 0, "output in Wring's syntax", -20 },
    { "utf8", '8', nullptr, 0, "output using UTF-8 characters", -20 },
    { "latex", OPT_LATEX, nullptr, 0, "output using LaTeX macros", -20 },
    // --csv-escape was deprecated in Spot 2.1, we can remove it at
    // some point
    { "csv-escape", OPT_CSV, nullptr, OPTION_HIDDEN,
      "quote the formula for use in a CSV file", -20 },
    { "format", OPT_FORMAT, "FORMAT", 0,
      "specify how each line should be output (default: \"%f\")", -20 },
    { "stats", 0, nullptr, OPTION_ALIAS, nullptr, 0 },
    { "output", 'o', "FORMAT", 0,
      "send output to a file named FORMAT instead of standard output.  The"
      " first formula sent to a file truncates it unless FORMAT starts"
      " with '>>'.", 0 },
    { "zero-terminated-output", '0', nullptr, 0,
      "separate output formulas with \\0 instead of \\n "
      "(for use with xargs -0)", 0 },
    { nullptr, 0, nullptr, 0, nullptr, 0 }
  };

const struct argp output_argp = { options, parse_opt_output,
                                  nullptr, nullptr, nullptr,
                                  nullptr, nullptr };

static
void
report_not_ltl(spot::formula f,
               const char* filename, const char* linenum, const char* syn)
{
  std::string s = spot::str_psl(f);
  static const char msg[] =
    "formula '%s' cannot be written in %s's syntax because it is not LTL";
  if (filename)
    error_at_line(2, 0, filename, atoi(linenum), msg, s.c_str(), syn);
  else
    error(2, 0, msg, s.c_str(), syn);
}

std::ostream&
stream_formula(std::ostream& out,
               spot::formula f, const char* filename,
               const char* linenum)
{
  switch (output_format)
    {
    case lbt_output:
      if (f.is_ltl_formula())
        spot::print_lbt_ltl(out, f);
      else
        report_not_ltl(f, filename, linenum, "LBT");
      break;
    case spot_output:
      spot::print_psl(out, f, full_parenth);
      break;
    case spin_output:
      if (f.is_ltl_formula())
        spot::print_spin_ltl(out, f, full_parenth);
      else
        report_not_ltl(f, filename, linenum, "Spin");
      break;
    case wring_output:
      if (f.is_ltl_formula())
        spot::print_wring_ltl(out, f);
      else
        report_not_ltl(f, filename, linenum, "Wring");
      break;
    case utf8_output:
      spot::print_utf8_psl(out, f, full_parenth);
      break;
    case latex_output:
      spot::print_latex_psl(out, f, full_parenth);
      break;
    case count_output:
    case quiet_output:
      break;
    }
  return out;
}

static void
stream_escapable_formula(std::ostream& os,
                         spot::formula f,
                         const char* filename,
                         const char* linenum)
{
  if (escape_csv)
    {
      std::ostringstream out;
      stream_formula(out, f, filename, linenum);
      os << '"';
      spot::escape_rfc4180(os, out.str());
      os << '"';
    }
  else
    {
      stream_formula(os, f, filename, linenum);
    }
}


namespace
{
  struct formula_with_location
  {
    spot::formula f;
    const char* filename;
    const char* line;
    const char* prefix;
    const char* suffix;
  };

  class printable_formula_class final:
    public spot::printable_value<char>
  {
  public:
    printable_formula_class&
    operator=(char new_val)
    {
      val_ = new_val;
      return *this;
    }

    virtual void
    print(std::ostream& os, const char* opt) const override
    {
      if (*opt == '[')
        os << spot::mp_class(val_, opt + 1);
      else
        os << val_;
    }
  };

  class printable_formula_nesting final:
    public spot::printable_value<spot::formula>
  {
  public:
    printable_formula_nesting&
    operator=(spot::formula new_val)
    {
      val_ = new_val;
      return *this;
    }

    virtual void
    print(std::ostream& os, const char* opt) const override
    {
      if (*opt == '[' && opt[1] != ']')
        os << spot::nesting_depth(val_, opt + 1);
      else
        throw std::runtime_error("%n expects arguments, e.g. %[X]n");
    }
  };

  class printable_formula_with_location final:
    public spot::printable_value<const formula_with_location*>
  {
  public:
    printable_formula_with_location&
    operator=(const formula_with_location* new_val)
    {
      val_ = new_val;
      return *this;
    }

    virtual void
    print(std::ostream& os, const char*) const override
    {
      stream_escapable_formula(os, val_->f, val_->filename, val_->line);
    }
  };

  class formula_printer final: protected spot::formater
  {
  public:
    formula_printer(std::ostream& os, const char* format)
      : format_(format)
    {
      declare('a', &ap_);       // deprecated in 2.3.2
      declare('b', &bool_size_);
      declare('f', &fl_);
      declare('F', &filename_);
      declare('R', &timer_);
      declare('r', &timer_);
      declare('L', &line_);
      declare('s', &size_);
      declare('h', &class_);
      declare('n', &nesting_);
      declare('x', &ap_);
      declare('<', &prefix_);
      declare('>', &suffix_);
      set_output(os);
      prime(format);
    }

    std::ostream&
    print(const formula_with_location& fl, spot::process_timer* ptimer)
    {
      if (has('R') || has('r'))
        timer_ = *ptimer;

      fl_ = &fl;
      filename_ = fl.filename ? fl.filename : "";
      line_ = fl.line;
      prefix_ = fl.prefix ? fl.prefix : "";
      suffix_ = fl.suffix ? fl.suffix : "";
      auto f = fl_.val()->f;
      if (has('a') || has('x'))
        {
          auto s = spot::atomic_prop_collect(f);
          ap_.set(s->begin(), s->end());
          delete s;
        }
      if (has('b'))
        bool_size_ = spot::length_boolone(f);
      if (has('s'))
        size_ = spot::length(f);
      if (has('h'))
        class_ = spot::mp_class(f);
      nesting_ = f;
      auto& res = format(format_);
      // Make sure we do not store the formula until the next one is
      // printed, as the order in which APs are registered may
      // influence the automata output.
      fl_ = nullptr;
      nesting_ = nullptr;
      ap_.clear();
      return res;
    }

  private:
    const char* format_;
    printable_formula_with_location fl_;
    printable_timer timer_;
    spot::printable_value<const char*> filename_;
    spot::printable_value<const char*> line_;
    spot::printable_value<const char*> prefix_;
    spot::printable_value<const char*> suffix_;
    spot::printable_value<int> size_;
    printable_formula_class class_;
    printable_formula_nesting nesting_;
    spot::printable_value<int> bool_size_;
    printable_varset ap_;
  };
}

static formula_printer* format = nullptr;
static std::ostringstream outputname;
static formula_printer* outputnamer = nullptr;
static std::map<std::string, std::unique_ptr<output_file>> outputfiles;

int
parse_opt_output(int key, char* arg, struct argp_state*)
{
  // Called from C code, so should not raise any exception.
  BEGIN_EXCEPTION_PROTECT;
  // This switch is alphabetically-ordered.
  switch (key)
    {
    case '0':
      output_terminator = 0;
      break;
    case '8':
      output_format = utf8_output;
      break;
    case 'l':
      output_format = lbt_output;
      break;
    case 'o':
      outputnamer = new formula_printer(outputname, arg);
      break;
    case 'p':
      full_parenth = true;
      break;
    case 's':
      output_format = spin_output;
      break;
    case OPT_CSV:
      escape_csv = true;
      break;
    case OPT_LATEX:
      output_format = latex_output;
      break;
    case OPT_SPOT:
      output_format = spot_output;
      break;
    case OPT_WRING:
      output_format = wring_output;
      break;
    case OPT_FORMAT:
      delete format;
      format = new formula_printer(std::cout, arg);
      break;
    default:
      return ARGP_ERR_UNKNOWN;
    }
  END_EXCEPTION_PROTECT;
  return 0;
}


static void
output_formula(std::ostream& out,
               spot::formula f, spot::process_timer* ptimer,
               const char* filename, const char* linenum,
               const char* prefix, const char* suffix)
{
  if (!format)
    {
      if (prefix)
        out << prefix << ',';
      // For backward compatibility, we still run
      // stream_escapable_formula when --csv-escape has been given.
      // But eventually --csv-escape should be removed, and the
      // formula printed raw.
      if ((prefix || suffix) && !escape_csv)
        {
          std::ostringstream tmp;
          stream_formula(tmp, f, filename, linenum);
          std::string tmpstr = tmp.str();
          if (tmpstr.find_first_of("\",") != std::string::npos)
            {
              out << '"';
              spot::escape_rfc4180(out, tmpstr);
              out << '"';
            }
          else
            {
              out << tmpstr;
            }
        }
      else
        {
          stream_escapable_formula(out, f, filename, linenum);
        }
      if (suffix)
        out << ',' << suffix;
    }
  else
    {
      formula_with_location fl = { f, filename, linenum, prefix, suffix };
      format->print(fl, ptimer);
    }
}

void
output_formula_checked(spot::formula f, spot::process_timer* ptimer,
                       const char* filename, const char* linenum,
                       const char* prefix, const char* suffix)
{
  if (output_format == count_output)
    {
      if (outputnamer)
        throw std::runtime_error
          ("options --output and --count are incompatible");
      return;
    }
  if (output_format == quiet_output)
    return;
  std::ostream* out = &std::cout;
  if (outputnamer)
    {
      outputname.str("");
      formula_with_location fl = { f, filename, linenum, prefix, suffix };
      outputnamer->print(fl, ptimer);
      std::string fname = outputname.str();
      auto p = outputfiles.emplace(fname, nullptr);
      if (p.second)
        p.first->second.reset(new output_file(fname.c_str()));
      out = &p.first->second->ostream();
    }
  output_formula(*out, f, ptimer, filename, linenum, prefix, suffix);
  *out << output_terminator;
  // Make sure we abort if we can't write to std::cout anymore
  // (like disk full or broken pipe with SIGPIPE ignored).
  check_cout();
}

void output_formula_checked(spot::formula f,
                            spot::process_timer* ptimer,
                            const char* filename, int linenum,
                            const char* prefix,
                            const char* suffix)
{
  output_formula_checked(f, ptimer, filename,
                         std::to_string(linenum).c_str(),
                         prefix, suffix);
}
