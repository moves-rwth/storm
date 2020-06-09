dtmc

const double p;

module test

	// local state
	s : [0..4] init 0;
	
	[] s=0 -> p : (s'=1) + (1-p) : (s'=2);
	[] s=1 -> p : (s'=3) + (1-p) : (s'=4);
	[] s=2 -> 0.5*p : (s'=3) + (1-0.5*p) : (s'=4);
	[] s=3 -> 1 : (s'=3);
	[] s=4 -> 1 : (s'=4);
	
endmodule

// Dot output:
//digraph model {
//	0 [ label = "0: {init}" ];
//	1 [ label = "1: {}" ];
//	2 [ label = "2: {}" ];
//	3 [ label = "3: {}" ];
//	4 [ label = "4: {}" ];
//	0 -> 1 [ label= "(p)/(1)" ];
//	0 -> 2 [ label= "(-1 * (p+(-1)))/(1)" ];
//	1 -> 3 [ label= "(p)/(1)" ];
//	1 -> 4 [ label= "(-1 * (p+(-1)))/(1)" ];
//	2 -> 3 [ label= "(p)/(2)" ];
//	2 -> 4 [ label= "(-1 * (p+(-2)))/(2)" ];
//	3 -> 3 [ label= "1" ];
//	4 -> 4 [ label= "1" ];
//}

