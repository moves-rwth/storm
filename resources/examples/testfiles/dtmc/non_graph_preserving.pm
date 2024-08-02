dtmc

const p;

module nongraphpreserving
	s : [0..2] init 0;
	
	[] s=0 -> p : (s'=1) + 1-p : (s'=2);
	[] s=1 -> p : (s'=0) + 1-p : (s'=2);
	[] s=2 -> 1 : (s'=2);
endmodule

label "target" = s=2;
