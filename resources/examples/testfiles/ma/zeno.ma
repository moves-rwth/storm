
ma

module simple
	
	s : [0..4];

	[alpha] (s=0) -> 1 : (s' = 1);
	[beta]  (s=0) -> 0.8 : (s'=0) + 0.2 : (s'=2);
	<> (s=1) -> 9 : (s'=0) + 1 : (s'=3);
	<> (s=2) -> 12 : (s'=4);
	[gamma] (s>2) -> 1 : true;
	
endmodule