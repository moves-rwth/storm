
ma


module main
	
	s : [0..4]; // current state:
	
	
	<> s=0 -> 4 : (s'=1) + 4 : (s'=2);
	[alpha] s=1 -> 1 : (s'=0);
	[beta] s=1 -> 0.3 : (s'=2) + 0.7 : (s'=1);
	[gamma] s=2 -> 1 : (s'=1);
	[delta] s=2 -> 0.5 : (s'=2) + 0.5 : (s'=3);
	<> s=3 -> 1 : (s'=4);
	[lambda] s=4 -> 1 : (s'=3);
	
endmodule

rewards "rew0"
	[delta] s=2 : 1;
endrewards

rewards "rew1"
	s=0 : 7;
	[delta] s=2 : 1;
endrewards


rewards "rew2"
	s=0 : 7;
	[delta] s=2 : 1;
	[lambda] s=4 : 100;
endrewards

rewards "rew3"
	s=0 : 7;
	[delta] s=2 : 1;
	[gamma] s=4 : 100;
endrewards
