
ma

const int N; // num packages

const double inRate = 4;
const double processingRate = 4;

module streamingclient
	
	s : [0..3]; // current state:
	// 0: decide whether to start	
	// 1: buffering
	// 2: running
	// 3: success
	
	n : [0..N]; // number of received packages
	k : [0..N]; // number of processed packages 	
	
	[buffer]  s=0 & n<N & k=n -> 1 : (s'=1);
	[buffer]  s=0 & n<N & k<n -> 0.99: (s'=1) + 0.01 : (s'=2) & (k'=k+1);
	[start] s=0 & k<n -> 1 : (s'=2) & (k'=k+1);
	
	<> s=1 -> inRate : (n'=n+1) & (s'=0);
	
	<> s=2 & n<N & k<n -> inRate : (n'=n+1) + processingRate : (k'=k+1);
	<> s=2 & n<N & k=n -> inRate : (n'=n+1) + processingRate : (s'=0); 
	<> s=2 & n=N & k<n -> processingRate : (k'=k+1);
	<> s=2 & n=N & k=N -> processingRate : (s'=3);
	
	<> s=3 -> 1 : true;
endmodule


label "underrun" = (s=0 & k>0);
label "running" = (s=2);
label "done" = (s=3);

rewards "buffering"
	s=1 : 1;
endrewards

rewards "numrestarts"
	[start] k > 0 : 1;
endrewards
