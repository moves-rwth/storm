dtmc

// probability of forwarding
const double    PF = 4/5;
const double notPF = 1/5;  // must be 1-PF
// probability that a crowd member is bad
const double  badC = 167/1000;
 // probability that a crowd member is good
const double goodC = 833/1000;
// Total number of protocol runs to analyze
const int TotalRuns = 5;
// size of the crowd
const int CrowdSize = 5;

module crowds
	// protocol phase
	phase: [0..4] init 0;

	// crowd member good (or bad)
	good: bool init false;

	// number of protocol runs
	runCount: [0..TotalRuns] init 0;

	// observe_i is the number of times the attacker observed crowd member i
	observe0: [0..TotalRuns] init 0;

	observe1: [0..TotalRuns] init 0;

	observe2: [0..TotalRuns] init 0;

	observe3: [0..TotalRuns] init 0;

	observe4: [0..TotalRuns] init 0;

	// the last seen crowd member
	lastSeen: [0..CrowdSize - 1] init 0;

	// get the protocol started
	[] phase=0 & runCount<TotalRuns -> 1: (phase'=1) & (runCount'=runCount+1) & (lastSeen'=0);

	// decide whether crowd member is good or bad according to given probabilities
	[] phase=1 -> goodC : (phase'=2) & (good'=true) + badC : (phase'=2) & (good'=false);

	// if the current member is a good member, update the last seen index (chosen uniformly)
	[] phase=2 & good -> 1/5 : (lastSeen'=0) & (phase'=3) + 1/5 : (lastSeen'=1) & (phase'=3) + 1/5 : (lastSeen'=2) & (phase'=3) + 1/5 : (lastSeen'=3) & (phase'=3) + 1/5 : (lastSeen'=4) & (phase'=3);

	// if the current member is a bad member, record the most recently seen index
	[] phase=2 & !good & lastSeen=0 & observe0 < TotalRuns -> 1: (observe0'=observe0+1) & (phase'=4);
	[] phase=2 & !good & lastSeen=1 & observe1 < TotalRuns -> 1: (observe1'=observe1+1) & (phase'=4);
	[] phase=2 & !good & lastSeen=2 & observe2 < TotalRuns -> 1: (observe2'=observe2+1) & (phase'=4);
	[] phase=2 & !good & lastSeen=3 & observe3 < TotalRuns -> 1: (observe3'=observe3+1) & (phase'=4);
	[] phase=2 & !good & lastSeen=4 & observe4 < TotalRuns -> 1: (observe4'=observe4+1) & (phase'=4);

	// good crowd members forward with probability PF and deliver otherwise
	[] phase=3 -> PF : (phase'=1) + notPF : (phase'=4);

	// deliver the message and start over
	[] phase=4 -> 1: (phase'=0);

endmodule

label "observe0Greater1" = observe0>1;
label "observe1Greater1" = observe1>1;
label "observe2Greater1" = observe2>1;
label "observe3Greater1" = observe3>1;
label "observe4Greater1" = observe4>1;
label "observeIGreater1" = observe1>1|observe2>1|observe3>1|observe4>1;
label "observeOnlyTrueSender" = observe0>1&observe1<=1 & observe2<=1 & observe3<=1 & observe4<=1;
