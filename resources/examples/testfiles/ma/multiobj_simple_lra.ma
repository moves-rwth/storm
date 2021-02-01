ma
module main
	
	x : [0..4];
	[a] x=0 -> 0.5 : (x'=1) + 0.5 : (x'=0);
	[b] x=0 -> (x'=2);
	<> x=1 -> 6 : (x'=2) + 4 : (x'=4);
	<> x=2 -> 5 : (x'=2) + 5 : (x'=3);
	[c] x=3 -> 0.3 : (x'=2) + 0.7 : (x'=4);
	[d] x=3 -> 0.3 : (x'=4) + 0.7 : (x'=2);
	<> x=4 -> 5 : (x'=3);
endmodule

rewards "first"
	true : 3;
    [] x=2 : 10;
	[d] true : 5;
endrewards

rewards "second"
    x<2 : 42;
endrewards
