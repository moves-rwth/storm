//Randomised Consensus Protocol

mdp
const double p1; // in [0.2 , 0.8]
const double p2; // in [0.2 , 0.8]


const int N=2;
const int K=8;
const int range = 2*(K+1)*N;
const int counter_init = (K+1)*N;
const int left = N;
const int right = 2*(K+1)*N - N;

// shared coin
global counter : [0..range] init counter_init;

module process1
	
	// program counter
	pc1 : [0..3];
	// 0 - flip
	// 1 - write 
	// 2 - check
	// 3 - finished
	
	// local coin
	coin1 : [0..1];	

	// flip coin
	[] (pc1=0)  -> p1 : (coin1'=0) & (pc1'=1) + 1 - p1 : (coin1'=1) & (pc1'=1);
	// write tails -1  (reset coin to add regularity)
	[] (pc1=1) & (coin1=0) & (counter>0) -> (counter'=counter-1) & (pc1'=2) & (coin1'=0);
	// write heads +1 (reset coin to add regularity)
	[] (pc1=1) & (coin1=1) & (counter<range) -> (counter'=counter+1) & (pc1'=2) & (coin1'=0);
	// check
	// decide tails
	[] (pc1=2) & (counter<=left) -> (pc1'=3) & (coin1'=0);
	// decide heads
	[] (pc1=2) & (counter>=right) -> (pc1'=3) & (coin1'=1);
	// flip again
	[] (pc1=2) & (counter>left) & (counter<right) -> (pc1'=0);
	// loop (all loop together when done)
	[done] (pc1=3) -> (pc1'=3);

endmodule

module process2 = process1[pc1=pc2,coin1=coin2,p1=p2] endmodule
label "finished" = pc1=3 &pc2=3 ;
label "all_coins_equal_1" = coin1=1 &coin2=1 ;
rewards "steps"
	true : 1;
endrewards



