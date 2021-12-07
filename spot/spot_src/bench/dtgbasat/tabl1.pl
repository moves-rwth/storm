#!/usr/bin/perl -w

use strict;

my $num = 0;
my @v = ();
while (<>)
{
    # G(a | Fb), 1, trad, 2, 4, 8, 1, 1, 1, 1, 0.00165238, DBA, 2, 4, 8, 1, 1, 1, 1, 0.00197852, minDBA, 2, 4, 8, 1, 1, 1, 1, 0.00821457, minDTGBA, 2, 4, 8, 1, 1, 1, 1, 0.0081701
    chomp;
    next if /.*realizable.*/;
    next unless /WDBA/;
    $v[$num] = [split /,/];
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
\\usepackage{booktabs}
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
print "\\begin{tabular}{lc|c|rr|c|r}";
print "&& \\multicolumn{1}{c|}{minDBA} & trad & +SAT & DRA & {DBA\\footnotesize m..zer} \\\\\n";
print " & C. & \$|Q|\$ & time & time & \$|Q|\$ & time \\\\\n";
print "\\midrule\n";

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

    $lasttype = $tab->[2];
    if ($linenum++ % 4 == 0)
    {
	# print "\\arrayrulecolor{lightgray}\\hline\\arrayrulecolor{black}";
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
    # If one of the automata is not deterministic highlight the "Complete" column.
    print "{\\P}" if val(8) == 0 || val(17) == 0 || val(26) == 0 || val(35) == 0  || val(44) == 0;
    print "$tab->[9] & ";

    # print "$tab->[3] & "; # states
    # print "$tab->[5] & "; # transitions
    # print "$tab->[6] & "; # acc
    # printf("%.2f &", $tab->[10]);

    if ($tab->[12] =~ /\s*-\s*/)  # DBA
    {
	print "- & - &";
	$tab->[12] = 0+'inf';
    }
    elsif ($tab->[12] =~ /\s*!\s*/)  # DBA
    {
	print "! & ! &";
	$tab->[12] = 0+'inf';
    }
    else
    {
	print "$tab->[12] & "; # states
	# print "$tab->[14] & "; # transitions
	printf("%.2f &", $tab->[19]);
    }

    if ($tab->[21] =~ /\s*-\s*/) #  minDBA
    {
        my $s = getlastsuccesful($n, "DBA");
	print "\\multicolumn{1}{c|}{(killed$s)}&";
	$tab->[21] = 0+'inf';
    }
    elsif ($tab->[21] =~ /\s*!\s*/) #  minDBA
    {
        my $s = getlastsuccesful($n, "DBA");
	print "\\multicolumn{1}{c|}{(intmax$s)}&";
	$tab->[21] = 0+'inf';
    }
    else
    {
	printf("%.2f &", $tab->[28]);
    }

    if ($tab->[51] =~ m:\s*n/a\s*:) #  DRA
    {
	print "&";
	$tab->[51] = 0+'inf';
    }
    else
    {
	# Remove sink state if not complete.
	my $st = $tab->[51] - 1 + $tab->[9] || 1;
	print "\\textbf" if ($st > $tab->[12]);
	print "{$st} & ";
    }

    if ($tab->[48] =~ /\s*-\s*/) #  DBAminimizer
    {
	print "\\multicolumn{1}{c}{(killed)}&";
	$tab->[48] = 0+'inf';
    }
    elsif ($tab->[48] =~ m:\s*n/a\s*:) #  DBAminimizer
    {
	print " &";
	$tab->[48] = 0+'inf';
    }
    else
    {
	# Remove sink state if not complete.
	my $st = $tab->[48] - 1 + $tab->[9] || 1;
	print "{\\E}" if ($st < $tab->[12]);
	print "{\\P}" if ($st > $tab->[12]);
	printf("%.2f", $tab->[49]);
    }

    print "\\\\\n";
}
print "\\end{tabular}\n";
print "\\end{document}\n";
