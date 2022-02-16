mdp

// parameter
const double p;

module test

	// local state
	s : [0..5] init 0;

	[] s=0 -> 0.5 : (s'=1) + 0.5 : (s'=2);

	// s1 beta
	[] s=1 -> 0.5*p : (s'=4) + 0.5*p : (s'=3) + 1-p : (s'=5);
	// s1 gamma
	[] s=1 -> p : (s'=4) + 1-p : (s'=3);

	// s2 alpha
	[] s=2 -> p : (s'=1) + 1-p : (s'=5);
	// s2 delta
	[] s=2 -> 1-p : (s'=1) + 0.5*p : (s'=3) + 0.5*p : (s'=5);

	[] s=3 -> p : (s'=4) + 1-p : (s'=5);

	[] s=4 -> 1 : (s'=4);
	[] s=5 -> 1 : (s'=5);


endmodule

