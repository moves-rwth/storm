dtmc

const double p;

module test

	// local state
	s : [0..5] init 0;
	
	[] s=0 -> p : (s'=4) + (1-p) : (s'=5);
	[] s=4 -> 1 : (s'=4);
	[] s=5 -> 1 : (s'=5);
		
endmodule


label "target" = s=4;
// Dot output:
//digraph model {
//	0 [ label = "0: {init}" ];
//	1 [ label = "1: {}" ];
//	2 [ label = "2: {}" ];
//	3 [ label = "3: {}" ];
//	4 [ label = "4: {}" ];
//	5 [ label = "5: {}" ];
//	0 -> 1 [ label= "(2 * (p))/(5)" ];
//	0 -> 2 [ label= "(-1 * (p+(-1)))/(1)" ];
//	0 -> 3 [ label= "(3 * (p))/(5)" ];
//	1 -> 3 [ label= "(p)/(2)" ];
//	1 -> 4 [ label= "(p)/(2)" ];
//	1 -> 5 [ label= "(-1 * (p+(-1)))/(1)" ];
//	2 -> 4 [ label= "(3 * (p))/(10)" ];
//	2 -> 5 [ label= "(-1 * (3*p+(-10)))/(10)" ];
//	3 -> 4 [ label= "(7 * (p))/(10)" ];
//	3 -> 5 [ label= "(-1 * (7*p+(-10)))/(10)" ];
//	4 -> 4 [ label= "1" ];
//	5 -> 5 [ label= "1" ];
//}