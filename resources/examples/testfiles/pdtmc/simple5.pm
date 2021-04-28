dtmc

const double p;
const double q;

module test

	// local state
	s : [0..4] init 0;
	
	[] s=0 -> p : (s'=1) + (1-p) : (s'=2);
	[] s=1 -> q : (s'=3) + (1-q) : (s'=4);
	[] s=2 -> 0.5*p : (s'=3) + (1-0.5*p) : (s'=4);
	[] s=3 -> 1 : (s'=3);
	[] s=4 -> 1 : (s'=4);
	
endmodule

