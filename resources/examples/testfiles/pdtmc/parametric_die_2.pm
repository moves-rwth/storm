// Knuth's model of a fair die using only fair coins
dtmc

const double p;
const double q;

module die

	// local state
	s : [0..7] init 0;
	// value of the dice
	d : [0..6] init 0;
	
	[] s=0 -> p : (s'=1) + (1-p) : (s'=2);
	[] s=1 -> q : (s'=3) + (1-q) : (s'=4);
	[] s=2 -> q : (s'=5) + (1-q) : (s'=6);
	[] s=3 -> p : (s'=1) + (1-p) : (s'=7) & (d'=1);
	[] s=4 -> p : (s'=7) & (d'=2) + (1-p) : (s'=7) & (d'=3);
	[] s=5 -> p : (s'=7) & (d'=4) + (1-p) : (s'=7) & (d'=5);
	[] s=6 -> p : (s'=2) + (1-p) : (s'=7) & (d'=6);
	[] s=7 -> 1: (s'=7);
	
endmodule

rewards "coin_flips"
	[] s<7 : 1;
endrewards

label "one" = s=7&d=1;
label "two" = s=7&d=2;
label "three" = s=7&d=3;
label "four" = s=7&d=4;
label "five" = s=7&d=5;
label "six" = s=7&d=6;
label "done" = s=7;
