// tiny LRA example

dtmc

module main
	
	s : [0..2] init 0;
	
	[] s=0 -> 1:(s'=1);
	[] s=1 -> 0.5:(s'=1) + 0.5:(s'=2);
	[] s=2 -> 0.5:(s'=1) + 0.5:(s'=2);
	
endmodule

label "a" = s=1;

