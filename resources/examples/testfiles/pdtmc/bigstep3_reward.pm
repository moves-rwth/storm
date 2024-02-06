dtmc

const double p;
const double q;
module test
	// local state
	s : [0..5] init 0;

	[] s=0 -> 0.4*p*p+0.6 : (s'=1) + 0.4*p*(1-p) : (s'=2) + 0.4*p*(1-p) : (s'=3) + 0.4*(1-p)*(1-p) : (s'=4);
	[] s=1 -> 0.25 : (s'=4) + 0.75 : (s'=5);
	[] s=2 -> 0.5 : (s'=4) + 0.5 : (s'=5);
	[] s=3 -> 0.2 : (s'=4) + 0.8 : (s'=5);
	[] s=4 -> q : (s'=5) + (1 - q) : (s'=2);
	[] s=5 -> 1 : (s'=5);
endmodule

rewards
	[] s=2 : 5;
	[] s=3 : 10;
endrewards

label "target" = s=5;

