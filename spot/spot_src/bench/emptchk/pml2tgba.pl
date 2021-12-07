#!/usr/bin/perl -w
#
# Copyright (C) 2004  Stefan Schwoon
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.#
#
# This script was originally distributed by Schwoon alongside
#
# @InProceedings{   schwoon.05.tacas,
#   author        = {Stefan Schwoon and Javier Esparza},
#   title         = {A note on on-the-fly verification algorithms.},
#   booktitle     = {Proceedings of the 11th International Conference on Tools
#                   and Algorithms for the Construction and Analysis of Systems
#                   (TACAS'05)},
#   year          = {2005},
#   series        = {Lecture Notes in Computer Science},
#   publisher     = {Springer-Verlag},
#   month         = apr
# }
#
# It has been modified in 2005 by Alexandre Duret-Lutz to
#   - extract the system's state space instead of the product space
#     (we want to use the LTL->TGBA translation of Spot, not that of Spin)
#   - output the state space in Spot's format
#   - optionally output weak fairness constraints
#   - allow partial order or not

use strict;

my @prop_list;
my %props;

sub usage()
{
  print <<EOF;
Usage: pml2tgba.pl [-w] promela-model properties...
Extract the state-space of the promela-model, observing properties.

Options:
  -w    output acceptance conditions to ensure weak fairness
  -r    let Spin reduce the state-space using partial order
EOF
  exit 1;
}

sub create_2n_automaton (@)
{
  my @props = @_;
  my @res;
  for my $p (@props)
    {
      if (@res)
	{
	  @res = map { ("$_ && ($p)", "$_ && !($p)") } @res;
	}
      else
	{
	  @res = ("($p)", "!($p)");
	}
    }

  my $trans = "\n  if\n";
  my $nres = $#res + 1;

  for my $p (@res)
    {
      push @prop_list, $p;
      $trans .= "  :: ($p) -> goto T0_init\n";
    }
  $trans .= "  fi;\n";
  if ($nres == 0)
    {
      push @prop_list, "(1)";
    }

  return "never {\nT0_init:$trans}\n";
}

usage unless @ARGV;

my $weak = 0;
my $reduce = ' -DNOREDUCE';
while (1)
{
  if ($ARGV[0] eq '-w')
    {
      $weak = 1;
    }
  elsif ($ARGV[0] eq '-r')
    {
      $reduce = '';
    }
  else
    {
      last;
    }
  shift;
}

my $model = shift @ARGV;

# Create the automaton
open NEVER, ">never.$$";
print NEVER create_2n_automaton (@ARGV);
close NEVER;

# Make a local copy of the model.  Recent versions of Spin (at least
# Spin 6.4.3) have problem compiling models that are in
# subdirectories; this was not the case in the past.
system "cp \"$model\" model.$$";
system "spin -a -N never.$$ model.$$";
unlink "never.$$", "model.$$";
system "gcc -DCHECK$reduce -O -o pan pan.c 2>/dev/null";

# Match Büchi states to propositions
my $buechitrans = 'BUG';
open PAN, "./pan -d|";
while (<PAN>)
{
  last if /^proctype :never/ || /^claim never/;
}
while (<PAN>)
{
  next
    unless (/^\s+state\s+\d+\s+-\(tr\s+(\d+)\s*\)->.*\d+ =>/o);
  # We are assuming that transition are output by -d in the same order
  # as we specified them in the neverclaim.
  my $prop = shift @prop_list;
  $props{$1} = $prop;
}
close PAN;

# Build the state graph from pan's DFS output
open PAN, "./pan 2>/dev/null |";

my $dfsstate = 0;
my @stack = ();
while (<PAN>) {
  last if (/	New state 0/);
}
my %acc = ();
push @stack, [$dfsstate, $buechitrans, %acc];

my %allaccs = ();
my %trans_list;
my $prop = "BUG";
while (<PAN>) {
  if (/^\d*: Down/) {
    push @stack, [$dfsstate, $buechitrans, %acc];
  } elsif (/^	New state (\d+)/) {
    pop @stack;
    push (@{$trans_list{$dfsstate}}, ["S$dfsstate, S$1, \"$prop\"", %acc]);
    %acc = ();
    $dfsstate = $1;
    push @stack, [$dfsstate, $buechitrans, %acc];
  } elsif (/^	(Old|Stack) state (\d+)/) {
    push (@{$trans_list{$dfsstate}}, ["S$dfsstate, S$2, \"$prop\"", %acc]);
    %acc = ();
  } elsif (/^ *\d+: proc 0 exec (\d+), \d+ to \d+/) {
    $buechitrans = $1;
    $prop = $props{$buechitrans};
  } elsif (/^ *\d+: proc (\d+) exec \d+, \d+ to \d+/) {
    $acc{"PR$1"} = 1;
    $allaccs{"PR$1"} = 1;
  } elsif (/^\d*: Up/) {
    pop @stack;
    ($dfsstate, $buechitrans, %acc) = @{$stack[$#stack]};
    $prop = $props{$buechitrans};
  }
}
close PAN;

unlink "pan", "pan.exe", "pan.c", "pan.h", "pan.b", "pan.t", "pan.m";


print "acc = @{[sort keys %allaccs]};\n" if $weak;
for my $state (sort {$a <=> $b} (keys %trans_list))
{
  my %missing = %allaccs;
  for my $t (@{$trans_list{$state}})
    {
      my ($trans, %acc) = @$t;
      for my $key (keys %acc)
	{
	  delete $missing{$key};
	}
    }
  for my $t (@{$trans_list{$state}})
    {
      my ($trans, %acc) = @$t;
      print "$trans,";
      print " @{[sort keys(%acc)]}  @{[sort keys(%missing)]}" if $weak;
      print ";\n";
    }
}
exit 0;

### Setup "GNU" style for perl-mode and cperl-mode.
## Local Variables:
## perl-indent-level: 2
## perl-continued-statement-offset: 2
## perl-continued-brace-offset: 0
## perl-brace-offset: 0
## perl-brace-imaginary-offset: 0
## perl-label-offset: -2
## cperl-indent-level: 2
## cperl-brace-offset: 0
## cperl-continued-brace-offset: 0
## cperl-label-offset: -2
## cperl-extra-newline-before-brace: t
## cperl-merge-trailing-else: nil
## cperl-continued-statement-offset: 2
## End:
