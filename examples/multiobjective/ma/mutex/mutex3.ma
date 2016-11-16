// Translation of the MAPA Specification of a mutex system into PRISM code
// http://wwwhome.cs.utwente.nl/~timmer/scoop/papers/qest13/index.html

ma

const int N; // The size of the data (should be at most 6)

formula someEnter  = s1=1 | s2=1 | s3=1;
formula someWait   = s1=2 | s2=2 | s3=2;
formula someLow    = s1=3 | s2=3 | s3=3;
formula someHigh   = s1=4 | s2=4 | s3=4;
formula someTie    = s1=5 | s2=5 | s3=5;
formula someAdmit  = s1=6 | s2=6 | s3=6;
formula otherHigh  =        s2=4 | s3=4;

formula someLowTie = someLow | someTie;
formula someLowHighTie = someLow | someHigh | someTie;
formula someAdmitHighTie = someAdmit | someHigh | someTie;
formula someEnterWait = someEnter | someWait;


module process1
	
	// The internal state of the process
	// 0: uninterested
	// 1: enter
	// 2: wait
	// 3: low
	// 4: high
	// 5: tie
	// 6: admit
	s1 : [0..6];
	
	// the phase of the protocol
	phase1 : [1..12];
	
	// The considered data
	data1 : [1..N];
	
	// The result of a coin flip
	h1 : bool;
	
	//[] phase1=1 -> 1 : true;
	[] phase1=1 -> 1 : (phase1'=2);
	
	[] phase1=2 & N>=1 -> 1 : (data1'=1) & (phase1'=3);
	[] phase1=2 & N>=2 -> 1 : (data1'=2) & (phase1'=3);
	[] phase1=2 & N>=3 -> 1 : (data1'=3) & (phase1'=3);
	[] phase1=2 & N>=4 -> 1 : (data1'=4) & (phase1'=3);
	[] phase1=2 & N>=5 -> 1 : (data1'=5) & (phase1'=3);
	[] phase1=2 & N>=6 -> 1 : (data1'=6) & (phase1'=3);
	
	[] phase1=3 & (someLowHighTie & !someAdmit) -> 1 : (s1'=2) & (phase1'=4);
	[] phase1=3 & (!someLowHighTie | someAdmit) ->  0.5 : (phase1'=5) & (h1'=false) + 0.5 : (phase1'=5) & (h1'=true);
	
	[] phase1=4 & (!someLowHighTie | someAdmit) -> 1 : (s1'=1) & (phase1'=3);
	
	[] phase1=5 & h1=false -> 1 : (s1'=3) & (phase1'=6);
	[] phase1=5 & h1=true -> 1 : (s1'=4) & (phase1'=7) & (h1'=false);
	
	[] phase1=6 & !someAdmitHighTie -> 1 : (s1'=5) & (phase1'=8);
	
	[] phase1=7 & (someAdmit | otherHigh) -> 1 : (s1'=5) & (phase1'=9);
	[] phase1=7 & (!someAdmit & !otherHigh) -> 1 :  (phase1'=10);
	
	[] phase1=8 -> 0.5 : (phase1'=5) & (h1'=false) + 0.5 : (phase1'=5) & (h1'=true);
	
	[] phase1=9 & !someAdmit & !otherHigh -> 0.5 : (phase1'=5) & (h1'=false) + 0.5 : (phase1'=5) & (h1'=true);
	
	<> phase1=10 -> data1 : (phase1'=11) & (data1'=1); 
	
	[] phase1=11 & (someLowTie | otherHigh) & !someEnter -> 1 : (s1'=0) & (phase1'=1);
	[] phase1=11 & !someLowTie & !otherHigh -> 1 : (s1'=6) & (phase1'=12);
	
	[] phase1=12 & !someEnterWait -> 1 : (s1'=0) & (phase1'=1); 
	
endmodule
 
module process2 = process1 [ s1=s2, phase1=phase2, data1=data2, h1=h2, s2=s1] endmodule
module process3 = process1 [ s1=s3, phase1=phase3, data1=data3, h1=h3, s3=s1] endmodule

label "crit1" = phase1=10;
label "crit2" = phase2=10;
label "crit3" = phase3=10;


rewards "timeInCrit1"
	phase1=10 : 1;
endrewards

rewards "timeInCrit2"
	 phase2=10 : 1;
endrewards

rewards "timeInCrit3"
	phase3=10 : 1;
endrewards

