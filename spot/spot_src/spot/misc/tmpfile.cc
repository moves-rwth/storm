// -*- coding: utf-8 -*-
// Copyright (C) 2013, 2015, 2017-2019 Laboratoire de Recherche et
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
#include <spot/misc/tmpfile.hh>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

using namespace std::string_literals;

namespace spot
{
  namespace
  {
    std::list<temporary_file*> to_clean;

    static const char*
    get_tmpdir()
    {
      const char* res = secure_getenv("SPOT_TMPDIR");
      if (res)
        return res;
      return secure_getenv("TMPDIR");
    }

    static int
    create_temporary_file(const char* prefix,
                          const char* suffix,
                          char** name)
    {
      static const char* tmpdir = get_tmpdir();
      static int tmpdirlen = tmpdir ? strlen(tmpdir) : 0;

      size_t len = strlen(prefix);
      size_t slen = 0;
      if (suffix)
        len += slen = strlen(suffix);
      char* x = *name = static_cast<char*>(malloc(tmpdirlen + 1 + len + 6 + 1));
      if (!x)
        throw std::bad_alloc();
      if (tmpdir)
        {
          x = stpcpy(x, tmpdir);
          if (x[-1] != '/')
            *x++ = '/';
        }
      x = stpcpy(x, prefix);
      x = stpcpy(x, "XXXXXX");
      int fd;
      if (suffix)
        {
          stpcpy(x, suffix);
          fd = mkstemps(*name, slen);
        }
      else
        {
          fd = mkstemp(*name);
        }
      if (fd < 0)
        {
          std::string err = ("failed to create temporary file "s + *name
                             + ": " + strerror(errno));
          if (errno == EACCES)
            {
              if (tmpdir)
                err += ("\nConsider setting the SPOT_TMPDIR environment "
                        "variable to a writable directory.");
              else
                err += ("\nConsider executing this from a writable "
                        "directory, or setting\nthe SPOT_TMPDIR environment "
                        "variable to such a directory.");
            }
          else if (tmpdir)
            {
              const char* dir =
                secure_getenv("SPOT_TMPDIR") ? "SPOT_TMPDIR" : "TMPDIR";
              err += ("\nNote that the directory comes from the "s
                      + dir
                      + " environment variable.");
            }
          throw std::runtime_error(err);
        }
      return fd;
    }
  }


  temporary_file::temporary_file(char* name, cleanpos_t cp)
    : name_(name), cleanpos_(cp)
  {
  }

  temporary_file::~temporary_file()
  {
    static bool must_unlink = !secure_getenv("SPOT_TMPKEEP");
    if (must_unlink)
      unlink(name_);
    free(name_);
    to_clean.erase(cleanpos_);
  }

  open_temporary_file::open_temporary_file(char* name, cleanpos_t cp, int fd)
    : temporary_file(name, cp), fd_(fd)
  {
  }

  open_temporary_file::~open_temporary_file()
  {
    close();
  }

  void
  open_temporary_file::close()
  {
    if (fd_ < 0)
      return;
    if (::close(fd_))
      throw std::runtime_error("failed to close "s + name_);
    fd_ = -1;
  }

  temporary_file*
  create_tmpfile(const char* prefix, const char* suffix)
  {
    char* name;
    int fd = create_temporary_file(prefix, suffix, &name);
    if (close(fd))
      throw std::runtime_error("failed to close "s + name);
    auto cp = to_clean.insert(to_clean.end(), nullptr);
    *cp = new temporary_file(name, cp);
    return *cp;
  }

  open_temporary_file*
  create_open_tmpfile(const char* prefix, const char* suffix)
  {
    char* name;
    int fd = create_temporary_file(prefix, suffix, &name);
    auto cp = to_clean.insert(to_clean.end(), nullptr);
    open_temporary_file* otf = new open_temporary_file(name, cp, fd);
    *cp = otf;
    return otf;
  }

  void
  cleanup_tmpfiles()
  {
    while (!to_clean.empty())
      delete to_clean.front();
  }
}
