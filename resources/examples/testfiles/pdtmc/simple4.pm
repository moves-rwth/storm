dtmc

const double p;

module test

	// local state
	s : [0..4] init 0;
	
	[] s=0 -> p*(1-p) : (s'=1) + (1-p*(1-p)) : (s'=2);
	[] s=1 -> p : (s'=3) + (1-p) : (s'=4);
	[] s=2 -> (1-p) : (s'=3) + (p) : (s'=4);
	[] s=3 -> 1 : (s'=3);
	[] s=4 -> 1 : (s'=4);
	
endmodule

