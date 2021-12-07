#!/usr/bin/perl
## -*- coding: utf-8 -*-
## Copyright (C) 2015, 2016 Laboratoire de Recherche et Développement de
## l'Epita (LRDE).
##
## This file is part of Spot, a model checking library.
##
## Spot is free software; you can redistribute it and/or modify it
## under the terms of the GNU General Public License as published by
## the Free Software Foundation; either version 3 of the License, or
## (at your option) any later version.
##
## Spot is distributed in the hope that it will be useful, but WITHOUT
## ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
## or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
## License for more details.
##
## You should have received a copy of the GNU General Public License
## along with this program.  If not, see <http://www.gnu.org/licenses/>.

use strict;

sub error($)
{
    print "$0: ", @_;
    exit 1;
}

error "Specify a directory with man pages and a directory for html pages\n"
    if @ARGV != 2;

my $dir = @ARGV[0];
my $out = @ARGV[1];

opendir(DIR, $dir) or die $!;
while (my $file = readdir(DIR))
{
    next unless $file =~ m/\.\d$/;
    my $ofile = "$out/$file.html";
    $file = "$dir/$file";
    print "converting $file to $ofile with groff\n";
    my $html = `(echo '.HEAD <LINK REL="stylesheet" TYPE="text/css" HREF="../spot.css">'
                 echo '.HEAD <meta name="viewport" content="width=device-width, initial-scale=1">'
                 cat $file) | groff -Kutf8 -mandoc -Thtml - -P -r`;
    $html =~ s|GNU GPL version 3 or later.*http://gnu.org/licenses/gpl.html&gt;|<a href="http://www.gnu.org/licenses/gpl.html">GNU GPL version 3 or later</a>|s;
    $html =~ s|<h2>.*?</h2>|<div class="outline-2">$&</div>|smg;
    $html =~ s|(<a href="#.*?">.*?</a><br>\n)+|<div id="table-of-contents"><h2>Table of Contents</h2><div id="text-table-of-contents"><ul>\n$&</ul></div></div>|sm;
    $html =~ s|(<a href="#.*?">.*?</a>)<br>|<li>$1</li>|g;
    $html =~ s|<p(.*?)>&bull;</p></td>.<td width="10%"></td>.<td width="78%">|<p$1>&bull;</p></td>\n<td width="88%">|smg;
    $html =~ s|&lt;spot\@lrde.epita.fr&gt;|&lt;<a href="mailto:spot\@lrde.epita.fr">spot\@lrde.epita.fr</a>&gt;|;
    $html =~ s|&lt;(https?://.*)&gt;|&lt;<a href="$1">$1</a>&gt;|;
    $html =~ s|<p style="margin-left:11%; margin-top: 1em"><b>([^<>]*?:)\s*<br>|<h3 style="margin-left:11%">$1</h3><p style="margin-left:11%"><b>|smg;
    $html =~ s|<p style="margin-left:11%; margin-top: 1em"><b>([^<>]*?:)\s*</b>\s*<br>|<h3 style="margin-left:11%">$1</h3><p style="margin-left:11%">|smg;
    $html =~ s|<p style="margin-left:11%; margin-top: 1em"><b>([^<>]*?:)\s*</b></p>|<h3 style="margin-left:11%">$1</h3>|smg;
    $html =~ s@<body>@<body class="man"><div id="org-div-home-and-up"><a accesskey="h" href="../tools.html"> UP </a>| <a accesskey="H" href="../index.html"> HOME </a></div>@;
    $html =~ s{<b>([\w-]+)</b>\((\d+)\)}{
      (-f "$dir/$1.$2") ? "<a href=\"$1.$2.html\"><b>$1</b></a>($2)" : $&;
    }xge;
    open(FILE, ">$ofile") or die $!;
    print FILE $html;
    close(FILE);
}
closedir(DIR);
exit 0;
