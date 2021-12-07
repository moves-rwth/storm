#!/usr/bin/perl -w

use strict;

my $num = 0;
my @v = ();
while (<>)
{
    # G(a | Fb), 1, trad, 2, 4, 8, 1, 1, 1, 1, 0.00165238, DBA, 2, 4, 8, 1, 1, 1, 1, 0.00197852, minDBA, 2, 4, 8, 1, 1, 1, 1, 0.00821457, minDTGBA, 2, 4, 8, 1, 1, 1, 1, 0.0081701
    chomp;
    next if /.*realizable.*/;
    $v[$num] = [split /,/];
#    print $v[$num]->[48], " ",  $#{$v[$num]}, "\n";
#    if ($#{$v[$num]} != 49)
#    {
#	pop $v[$num];
#	push $v[$num], '-', '-';
#   }
#    print $v[$num]->[48], " ",  $#{$v[$num]}, "\n";

    ++$num;
}

sub dratcong($$)
{
    my ($a, $b) = @_;
    return 0 if ($a =~ /.*CONG/ && $b =~ /.*CONG/);
    return $a cmp $b;
}

sub mycmp()
{
    my $v = dratcong($b->[2], $a->[2]);
    return $v if $v;
    my $n1 = lc $a->[0];
    $n1 =~ s/\W//g;
    my $n2 = lc $b->[0];
    $n2 =~ s/\W//g;
    return $n1 cmp $n2 || $a->[0] cmp $b->[0] || $b->[2] cmp $a->[2];;
}

my @w = sort mycmp @v;

print "\\documentclass{standalone}\n
\\usepackage{amsmath}
\\usepackage{colortbl}
\\definecolor{mygray}{gray}{0.75} % 1 = white, 0 = black
\\definecolor{lightgray}{gray}{0.7} % 1 = white, 0 = black
\\def\\E{\\cellcolor{mygray}}
\\def\\P{\\cellcolor{red}}
\\def\\PP{\\cellcolor{yellow}}
\\def\\F{\\mathsf{F}} % in future
\\def\\G{\\mathsf{G}} % globally
\\def\\X{\\mathsf{X}} % next
\\DeclareMathOperator{\\W}{\\mathbin{\\mathsf{W}}} % weak until
\\DeclareMathOperator{\\M}{\\mathbin{\\mathsf{M}}} % strong release
\\DeclareMathOperator{\\U}{\\mathbin{\\mathsf{U}}} % until
\\DeclareMathOperator{\\R}{\\mathbin{\\mathsf{R}}} % release
";

print "\\begin{document}\n";
print "\\begin{tabular}{lrcr|r|rrrr|rrr|rr|rrr|rrr|rrrr}";
print "\\multicolumn{24}{l}{Column \\textbf{type} shows how the initial det. aut. was obtained: T = translation produces DTGBA; W = WDBA minimization works; P = powerset construction transforms TBA to DTBA; R = DRA to DBA.}\\\\\n";
print "\\multicolumn{24}{l}{Column \\textbf{C.} tells whether the output automaton is complete: rejecting sink states are always omitted (add 1 state when C=0 if you want the size of the complete automaton).}\\\\\n";
print "&&&&DRA&\\multicolumn{4}{|c}{DTGBA} & \\multicolumn{3}{|c}{DBA} &  \\multicolumn{2}{|c}{DBA\\footnotesize minimizer}&  \\multicolumn{3}{|c}{min DBA} & \\multicolumn{3}{|c}{minDTBA} & \\multicolumn{4}{|c}{minDTGBA}\\\\\n";
print "formula & \$m\$ & type & C. & st. & st. & tr. & acc. & time & st. & tr. & time & st. & time & st. & tr. & time & st. & tr. & time & st. & tr. & acc. & time \\\\\n";

sub nonnull($)
{
    return 1 if $_[0] == 0;
    return $_[0];
}

sub getlastsuccesful($$)
{
    my ($n,$type) = @_;
    open LOG, "<$n.$type.satlog" or return "";
    my $min = "";
    while (my $line = <LOG>)
    {
	my @f = split(/,/, $line);
	$min = $f[2] if $f[2] ne '';
    }
    $min = ", \$\\le\$$min" if $min ne "";
    return $min;
}


my $lasttype = '';
my $linenum = 0;
foreach my $tab (@w)
{
    sub val($)
    {
	my ($n) = @_;
	my $v = $tab->[$n];
	return 0+'inf' if !defined($v) || $v =~ /\s*-\s*/;
	return $v;
    }

    if (dratcong($lasttype, $tab->[2]))
    {
	print "\\hline\n";
	$linenum = 0;
    }
    $lasttype = $tab->[2];
    if ($linenum++ % 4 == 0)
    {
	print "\\arrayrulecolor{lightgray}\\hline\\arrayrulecolor{black}";
    }

    my $n = $tab->[52];
    my $f = $tab->[0];
    $f =~ s/\&/\\land /g;
    $f =~ s/\|/\\lor /g;
    $f =~ s/!/\\bar /g;
    $f =~ s/<->/\\leftrightarrow /g;
    $f =~ s/->/\\rightarrow /g;
    $f =~ s/[XRWMGFU]/\\$& /g;
    print "\$$f\$\t& ";
    print "$tab->[1] & ";
    if ($tab->[2] =~ /trad/) { print "T & "; }
    elsif ($tab->[2] =~ /TCONG/) { print "P & "; }
    elsif ($tab->[2] =~ /DRA/) { print "R & "; }
    elsif ($tab->[2] =~ /WDBA/) { print "W & "; }
    else { print "$tab->[2]& "; }
    # If one of the automata is not deterministic highlight the "Complete" column.
    print "{\\P}" if val(8) == 0 || val(17) == 0 || val(26) == 0 || val(35) == 0  || val(44) == 0;
    print "$tab->[9] & ";

    if ($tab->[51] =~ m:\s*n/a\s*:) #  DRA
    {
	print "&";
	$tab->[51] = 0+'inf';
    }
    else
    {
	# Remove sink state if not complete.
	my $st = $tab->[51] - 1 + $tab->[9] || 1;
	print "$st & ";
    }

    print "$tab->[3] & "; # states
    print "$tab->[5] & "; # transitions
    print "$tab->[6] & "; # acc
    printf("%.2f &", $tab->[10]);

    if ($tab->[12] =~ /\s*-\s*/)  # DBA
    {
	print "- & - & - &";
	$tab->[12] = 0+'inf';
    }
    elsif ($tab->[12] =~ /\s*!\s*/)  # DBA
    {
	print "! & ! & ! &";
	$tab->[12] = 0+'inf';
    }
    else
    {
	print "$tab->[12] & "; # states
	print "$tab->[14] & "; # transitions
	printf("%.2f &", $tab->[19]);
    }

    if ($tab->[48] =~ /\s*-\s*/) #  DBAminimizer
    {
	print "\\multicolumn{2}{c|}{(killed)}&";
	$tab->[48] = 0+'inf';
    }
    elsif ($tab->[48] =~ m:\s*n/a\s*:) #  DBAminimizer
    {
	print " & &";
	$tab->[48] = 0+'inf';
    }
    else
    {
	# Remove sink state if not complete.
	my $st = $tab->[48] - 1 + $tab->[9] || 1;
	print "{\\E}" if ($st < $tab->[12]);
	print "{\\P}" if ($st > $tab->[12]);
	print "$st & "; # states
	printf("%.2f &", $tab->[49]);
    }

    if ($tab->[21] =~ /\s*-\s*/) #  minDBA
    {
	my $s = getlastsuccesful($n, "DBA");
	print "\\multicolumn{3}{c|}{(killed$s)}&";
	$tab->[21] = 0+'inf';
    }
    elsif ($tab->[21] =~ /\s*!\s*/) #  minDBA
    {
	my $s = getlastsuccesful($n, "DBA");
	print "\\multicolumn{3}{c|}{(intmax$s)}&";
	$tab->[21] = 0+'inf';
    }
    else
    {
	print "{\\E}" if ($tab->[21] < $tab->[12]);
	print "{\\P}" if ($tab->[21] > $tab->[12]);
	print "$tab->[21] & "; # states
	print "$tab->[23] & "; # transitions
	printf("%.2f &", $tab->[28]);
    }

    if ($tab->[39] =~ /\s*-\s*/) # min DTBA
    {
	my $s = getlastsuccesful($n, "DTBA");
	print "\\multicolumn{3}{c|}{(killed$s)}&";
	$tab->[39] = 0+'inf';
    }
    elsif ($tab->[39] =~ /\s*!\s*/) # min DTBA
    {
	my $s = getlastsuccesful($n, "DTBA");
	print "\\multicolumn{3}{c|}{(intmax$s)}&";
	$tab->[39] = 0+'inf';
    }
    else
    {
	print "{\\E}" if ($tab->[39] < $tab->[3]);
	print "{\\P}" if ($tab->[39] > $tab->[3] * nonnull($tab->[6])) or ($tab->[39] > $tab->[12]);
	print "\\textbf" if ($tab->[39] < $tab->[21]);
	print "{$tab->[39]} & "; # states
	print "$tab->[41] & "; # transitions
	printf("%.2f &", $tab->[46]);
    }

    if ($tab->[30] =~ /\s*-\s*/)   # minTGBA
    {
	my $s = getlastsuccesful($n, "DTGBA");
	print "\\multicolumn{4}{c}{(killed$s)}";
	$tab->[30] = 0+'inf';
    }
    elsif ($tab->[30] =~ /\s*!\s*/)   # minTGBA
    {
	my $s = getlastsuccesful($n, "DTGBA");
	print "\\multicolumn{4}{c}{(intmax$s)}";
	$tab->[30] = 0+'inf';
    }
    else
    {
	print "{\\E}" if ($tab->[30] < $tab->[3]);
	print "{\\P}" if ($tab->[30] > $tab->[3]) or ($tab->[30] > $tab->[12]) or ($tab->[30] > $tab->[21]) or ($tab->[30] > $tab->[39]);
	print "{\\PP}" if ($tab->[21] ne 'inf' && $tab->[30] * ($tab->[33] + 1) < $tab->[21]);
	print "\\textbf" if ($tab->[30] < $tab->[39]);
	print "{$tab->[30]} & "; # states
	print "$tab->[32] & "; # transitions
	print "\\textbf" if ($tab->[33] > $tab->[6]);
	print "{$tab->[33]} & "; # acc
	printf("%.2f ", $tab->[37]);
    }

    print "\\\\\n";
}
print "\\end{tabular}\n";
print "\\end{document}\n";
