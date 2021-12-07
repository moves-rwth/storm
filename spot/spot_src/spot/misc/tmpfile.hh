// -*- coding: utf-8 -*-
// Copyright (C) 2013, 2015, 2017 Laboratoire de Recherche et
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

#pragma once

#include <spot/misc/common.hh>
#include <new>
#include <stdexcept>
#include <iostream>
#include <list>
#include <spot/misc/formater.hh>

namespace spot
{
  /// \ingroup misc_tools
  /// @{

  /// \brief Temporary file name
  ///
  /// This class serves a dual purpose.
  ///
  /// 1. It carries the name of a temporary file, created with
  ///    create_tmpfile().
  /// 2. It represents the life of the associated file.  The file is
  ///    erased as soon as the temporary_file instance is destroyed.
  ///
  /// Note that there are two ways to destroy temporary_file
  /// instances.  Either directly with delete, or indirectly by
  /// calling cleanup_tmpfiles().
  /// You should never delete an instance that has been created
  /// before the last call to cleanup_tmpfiles(), because that
  /// instance has already been deleted.
  class SPOT_API temporary_file: public printable
  {
  public:
    typedef std::list<temporary_file*>::iterator cleanpos_t;

    SPOT_LOCAL temporary_file(char* name, cleanpos_t cp);

    temporary_file(const temporary_file& other) = delete;

    virtual ~temporary_file() override;

    const char* name() const
    {
      return name_;
    }

    friend std::ostream& operator<<(std::ostream& os, const temporary_file* f)
    {
      os << f->name();
      return os;
    }

    virtual void
    print(std::ostream& os, const char*) const final override
    {
      os << this;
    }

  protected:
    char* name_;
    cleanpos_t cleanpos_;
  };

  /// \brief Open temporary file
  ///
  /// This is a specialization of temporary_file that also holds an
  /// open file descriptor, as created by create_open_tmpfile().
  ///
  /// Use the open_temporary_file::close() method if you want to close
  /// that descriptor; do no call the POSIX close() function directly.
  class SPOT_API open_temporary_file final: public temporary_file
  {
  public:
    SPOT_LOCAL open_temporary_file(char* name, cleanpos_t cp, int fd);
    virtual ~open_temporary_file() override;

    void close();

    int fd() const
    {
      return fd_;
    }

  protected:
    int fd_;
  };

  /// \brief Create a temporary file.
  ///
  /// The file name will start with \a prefix, be followed by 6
  /// randomish characters and will end in \a suffix.  Usually suffix
  /// is used to set an extension (you should include the dot).
  ///
  /// The temporary file is created and left empty.  If you need
  /// to fill it, consider using create_open_tmpfile() instead.
  SPOT_API temporary_file*
  create_tmpfile(const char* prefix, const char* suffix = nullptr);

  /// \brief Create a temporary file and leave it open for writing.
  ///
  /// Same as create_tmpfile, be leave the file open for writing.  The
  /// open_temporary_file::fd() method returns the file descriptor.
  SPOT_API open_temporary_file*
  create_open_tmpfile(const char* prefix, const char* suffix = nullptr);

  /// \brief Delete all temporary files.
  ///
  /// Delete all temporary files that have been created but haven't
  /// been deleted so far.  The verb "delete" should be understood as
  /// both the C++ delete operator (all temporary_file and
  /// open_temporary_file instance are destroyed) and as the file
  /// system operation (the actual files are removed).
  ///
  /// Even in programs where temporary_file instance are consciously
  /// destroyed when they are not needed, cleanup_tmpfiles() could
  /// still be useful in signal handlers, for instance to clean all
  /// temporary files upon SIGINT.
  SPOT_API void
  cleanup_tmpfiles();

  /// @}
}
