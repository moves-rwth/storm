// Knuth's model of a fair die using only fair coins
dtmc

const double p;
const double q;

module die1

	// local state
	s1 : [0..7] init 0;
	// value of the dice
	d1 : [0..6] init 0;
	
	[] s1=0 -> p : (s1'=1) + 1-p : (s1'=2);
	[] s1=1 -> q : (s1'=3) + 1-q : (s1'=4);
	[] s1=2 -> q : (s1'=5) + 1-q : (s1'=6);
	[] s1=3 -> p : (s1'=1) + 1-p : (s1'=7) & (d1'=1);
	[] s1=4 -> p : (s1'=7) & (d1'=3) + 1-p : (s1'=7) & (d1'=2);
	[] s1=5 -> p : (s1'=2) + 1-p : (s1'=7) & (d1'=4);
	[] s1=6 -> p : (s1'=7) & (d1'=6) + 1-p : (s1'=7) & (d1'=5);
	[] s1=7 -> 1: (s1'=7);
	
endmodule

module die2 = die1 [ s1=s2, s2=s1, d1=d2 ] endmodule

rewards "coin_flips"
	[] s1<7 | s2<7 : 1;
endrewards

label "two" =   s1=7 & s2=7 & d1+d2=2;
label "three" = s1=7 & s2=7 & d1+d2=3;
label "four" =  s1=7 & s2=7 & d1+d2=4;
label "five" =  s1=7 & s2=7 & d1+d2=5;
label "six" =   s1=7 & s2=7 & d1+d2=6;
label "seven" =   s1=7 & s2=7 & d1+d2=7;
label "eight" =   s1=7 & s2=7 & d1+d2=8;
label "nine" =   s1=7 & s2=7 & d1+d2=9;
label "ten" =   s1=7 & s2=7 & d1+d2=10;
label "eleven" =   s1=7 & s2=7 & d1+d2=11;
label "twelve" =   s1=7 & s2=7 & d1+d2=12;
label "same" =   s1=7 & s2=7 & d1=d2;

label "end" =   s1=7 & s2=7;
