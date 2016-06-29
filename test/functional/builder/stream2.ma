
ma

const int N = 2; // num packages

const double inRate = 4;
const double processingRate = 5;

module streamingclient
	
	s : [0..3]; // current state:
	// 0: decide whether to start	
	// 1: buffering
	// 2: running
	// 3: done
	
	n : [0..N]; // number of received packages
	k : [0..N]; // number of processed packages 	
	
	[buffer]  s=0 & n<N -> 1 : (s'=1);
	[start] s=0 & k<n -> 1 : (s'=2) & (k'=k+1);
	
	<> s=1 -> inRate : (n'=n+1) & (s'=0);
	
	<> s=2 & n<N & k<n -> inRate : (n'=n+1) + processingRate : (k'=k+1);
	<> s=2 & n<N & k=n -> inRate : (n'=n+1) + processingRate : (s'=0); 
	<> s=2 & n=N & k<n -> processingRate : (k'=k+1);
	<> s=2 & n=N & k=N -> processingRate : (s'=3);
	
	<> s=3 -> 1 : true;
endmodule

// All packages received and buffer empty
label "done" = (s=3);

rewards "buffering"
	s=1 : 1;
endrewards

rewards "initialbuffering"
	s=1 & k = 0: 1;
endrewards

rewards "intermediatebuffering"
	s=1 & k > 0: 1;
endrewards

rewards "numRestarts"
	[start] k > 0 : 1;
endrewards
