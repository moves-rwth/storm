dtmc

const double p;

module test

	// local state
	s : [0..6] init 0;

	[] s=0 -> p : (s'=1) + (1-p) : (s'=2);
	[] s=1 -> p : (s'=3) + (1-p) : (s'=4);
	[] s=2 -> p : (s'=4) + (1-p) : (s'=5);
    	[] s=3 -> 1 : (s'=6);
    	[] s=4 -> 1 : (s'=6);
    	[] s=5 -> 1 : (s'=6);
    	[] s=6 -> 1 : (s'=6);

endmodule

rewards "rew1"
    [] s=0 : 0;
    [] s=1 : 1;
    [] s=2 : 10;
    [] s=3 : 1;
    [] s=4 : 2;
    [] s=5 : 1;
    [] s=6 : 0;
endrewards
