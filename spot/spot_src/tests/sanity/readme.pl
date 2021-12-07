#! /usr/bin/perl -w
# -*- cperl; coding: utf-8 -*-
#
# Copyright (C) 2010, 2015, 2016 Laboratoire de Recherche et
# Développement de l'Epita (LRDE).
#
# This file is part of Spot, a model checking library.
#
# Spot is free software; you can redistribute it and/or modify it
# under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3 of the License, or
# (at your option) any later version.
#
# Spot is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
# or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
# License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
# Check that all the directories documented in README exist, and that
# all directories listed in configure.ac are documented.
#
# Also has an option --list to print directories which are
# documented.

use strict;
use warnings;

local $\ = "\n";
my $srcdir = $ENV{srcdir} || ".";
my $top_srcdir = "$srcdir/../..";
my $top_srcdir_len = length($top_srcdir) + 1;
my $list_mode = ($#ARGV != -1 && $ARGV[0] eq "--list");

unless (-f "$top_srcdir/README")
{
  print STDERR "$top_srcdir/README not found";
  exit 2;
}

open(FD, "$top_srcdir/README")
  or die "$!: cannot open $top_srcdir/README";
my @directory = ();
my $exit_status = 0;

my %documenteddirs;

while (<FD>)
{
  # Skip non-distributed directories.
  next if /Not distributed/i;
  # We consider Third party software?
  # last if (/^Third party software$/);
  next unless (m{^(\s*)(\S+/)\s+});
  my $level = length($1) / 3;
  my $name = $2;

  $directory[$level] = $name;
  $#directory = $level;
  my $filename = join "", @directory;

  # Use globbing on the filename.
  for my $directory (glob("$top_srcdir/$filename"))
  {
    my $striped = substr($directory, $top_srcdir_len);
    $documenteddirs{$striped} = 1;
    if ($list_mode)
    {
      print "$striped";
    }
    else
    {
      unless (-d $directory || -f $directory)
      {
        print STDERR "$directory mentioned in README does not exist.";
        $exit_status = 1;
      }
    }
  }
}

close(FD);

open(FD, "$top_srcdir/configure.ac")
  or die "$!: cannot open $top_srcdir/configure.ac";
while (<FD>)
{
  next unless m{\s*(\S+/)Makefile};
  unless (exists $documenteddirs{$1})
  {
    print STDERR "$1 directory undocumented in README.";
    $exit_status = 1;
  }
}
close(FD);

exit $exit_status;
