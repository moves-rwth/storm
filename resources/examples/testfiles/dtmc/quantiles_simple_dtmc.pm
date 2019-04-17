
dtmc

module main
	s : [0..3] init 0;

	[] s=0 -> 0.4 : (s'=1) + 0.4 : (s'=2) + 0.2 : (s'=3);
	[a] s=1 -> 1 : (s'=0);
	[b] s=2 -> 1 : (s'=0);
	[] s=3 -> 1 : (s'=3);
endmodule
 
rewards "first"
	[a] true : 1;
endrewards
 
rewards "second"
	[b] true : 2;
endrewards

