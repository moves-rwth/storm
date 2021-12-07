#!/usr/bin/perl -w

use strict;
use Time::HiRes;

my $start = Time::HiRes::gettimeofday();
my $res = `@ARGV` || die;
my $end = Time::HiRes::gettimeofday();

if ($res =~ /States:\s*(\d+)\w*$/som)
{
    printf("%d, %f\n", $1, $end - $start);
}
else
{
    printf("-, -\n");
}
