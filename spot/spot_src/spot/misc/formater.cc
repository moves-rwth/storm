// -*- coding: utf-8 -*-
// Copyright (C) 2012, 2013, 2016 Laboratoire de Recherche et
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

#include "config.h"
#include <spot/misc/formater.hh>
#include <spot/misc/escape.hh>
#include <iostream>
#include <sstream>
#include <cstring>

namespace spot
{
  static void unclosed_bracket(const char* s)
  {
    std::ostringstream ss;
    ss << '\'' << s << "' has unclosed bracket";
    throw std::runtime_error(ss.str());
  }


  void
  formater::scan(const char* fmt, std::vector<bool>& has) const
  {
    for (const char* pos = fmt; *pos; ++pos)
      if (*pos == '%')
        {
          char c = *++pos;
          // %[...]c
          const char* mark = pos;
          if (c == '[')
            {
              do
                {
                  ++pos;
                  if (SPOT_UNLIKELY(!*pos))
                    unclosed_bracket(mark - 1);
                }
              while (*pos != ']');
              c = *++pos;
            }
          has[c] = true;
          if (!c)
            break;
        }
  }

  void
  formater::prime(const char* fmt)
  {
    scan(fmt, has_);
  }

  std::ostream&
  formater::format(const char* fmt)
  {
    for (const char* pos = fmt; *pos; ++pos)
      {
        if (*pos == '"')
          {
            *output_ << '"';
            const char* end = strchr(pos + 1, '"');
            if (!end)
              continue;
            std::string tmp(pos + 1, end - (pos + 1));
            std::ostringstream os;
            format(os, tmp);
            escape_rfc4180(*output_, os.str());
            pos = end;
            // the end double-quote will be printed below
          }
        if (*pos != '%')
          {
            *output_ << *pos;
          }
        else
          {
            char c = *++pos;
            const char* next = pos;
            // in case we have %[...]X... , we want to pass
            // [...]X... to the printer, and continue after the X once
            // that is done.
            if (c == '[')
              {
                do
                  {
                    ++next;
                    if (SPOT_UNLIKELY(*next == 0))
                      unclosed_bracket(pos - 1);
                  }
                while (*next != ']');
                c = *++next;
              }
            call_[c]->print(*output_, pos);
            if (!c)
              break;
            pos = next;
          }
      }
    return *output_;
  }
}
