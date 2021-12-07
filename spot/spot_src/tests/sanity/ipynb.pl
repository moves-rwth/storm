#! /usr/bin/perl -w
# -*- cperl; coding: utf-8 -*-
#
# Copyright (C) 2015, 2016 Laboratoire de Recherche et Développement
# de l'Epita (LRDE).
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

my $tut = "$srcdir/../../doc/org/tut.org";
my $dir = "$srcdir/../../tests/python";
unless (-f $tut)
{
  print STDERR "$tut not found";
  exit 2;
}

open(FD, "$tut") or die "$!: cannot open $tut";
my $exit_status = 0;
my %seen;
while (<FD>)
{
    if (m:/([\w-]+)\.html\]\[=([\w-]+\.ipynb)=\]\]:)
    {
	# print "$1 documented";
	$seen{$2} = 1;
	unless (-f "$dir/$2")
	{
	    print STDERR "notebook $2 mentioned in tut.org does not exist";
	    $exit_status = 1;
	}
	if ("$1.ipynb" ne "$2")
	{
	    print STDERR "in tut.org, notebook $2 links to $1.html";
	    $exit_status = 1;
	}
    }
}
close(FD);

open(FD, "$dir/../Makefile.am") or die "$!";
while (<FD>)
{
    if (m:python/([^_][\w-]*\.ipynb):)
    {
	unless (exists $seen{$1})
	{
	    print STDERR "notebook $1 is not mentioned in tut.org";
	    $exit_status = 1;
	}
    }
}
close(FD);

die "No notebook found?" if scalar(keys %seen) == 0;

exit $exit_status;
