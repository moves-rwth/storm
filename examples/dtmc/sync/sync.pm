// A simple model using synchronization
dtmc

module generator

	s : [0..2] init 0;
	
	[] s=0 -> 0.2 : (s'=1) + 0.8 : (s'=0);
	[yield] s=1 -> 1 : (s'=2);
	[] s=2 -> 0.5 : (s'=0) + 0.5 : (s'=2);
	
endmodule

module consumer
	
	t : [0..2] init 0;

	[] t=0 -> 0.8 : (t'=1) + 0.2 : (t'=0);
	[yield] t=1 -> 1 : (t'=2);
	[] t=2 -> 0.2 : (t'=0) + 0.8 : (t'=2);
	
endmodule
