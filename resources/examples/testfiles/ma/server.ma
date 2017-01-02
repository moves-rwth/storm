
ma

const double rateProcessing = 2;
const double rateA = 1;
const double rateB = 1;

module server
	
	s : [0..5]; // current state:
	// 0: wait for request	
	// 1: received request from A
	// 2: received request from B	
	// 3: starting to process request of B
	// 4: processing request
	// 5: error
	
	
	
	<> s=0 -> rateA : (s'=1) + rateB : (s'=2);
	[alpha] s=1 -> 1 : (s'=4);
	[alpha] s=2 -> 1 : (s'=3);
	[beta]  s=2 -> 0.5 : (s'=0) + 0.5 : (s'=3);
	[] s=3 -> 1 : (s'=4);
	<> s=4 -> rateProcessing : (s'=0) + (rateA+rateB) : (s'=5);
	<> s=5 -> 1 : true;
	
endmodule


label "error" = (s=5);
label "processB" = (s=3);


