dtmc

const double p;

module test

	// local state
	s : [0..7] init 0;

	[] s=0 -> p : (s'=1) + (1-p) : (s'=2);
	[] s=1 -> p : (s'=3) + (1-p) : (s'=4);
	[] s=2 -> p : (s'=4) + (1-p) : (s'=3);
    	[] s=3 -> p : (s'=6) + (1-p) : (s'=5);
    	[] s=4 -> p : (s'=7) + (1-p) : (s'=5);
    	[] s=5 -> p : (s'=6) + (1-p) : (s'=7);
    	[] s=6 -> 1 : (s'=6);
	[] s=7 -> 1 : (s'=7);

endmodule

rewards "rew1"
    [] s=0 : 1;
    [] s=1 : 1;
    [] s=2 : 2;
    [] s=3 : 3;
    [] s=4 : 8;
    [] s=5 : 5;
    [] s=6 : 6;
    [] s=7 : 7;
endrewards
