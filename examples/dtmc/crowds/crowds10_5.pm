dtmc

// probability of forwarding
const double    PF = 0.8;
const double notPF = .2;  // must be 1-PF
// probability that a crowd member is bad
const double  badC = .167;
 // probability that a crowd member is good
const double goodC = 0.833;
// Total number of protocol runs to analyze
const int TotalRuns = 5;
// size of the crowd
const int CrowdSize = 10;

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

	observe5: [0..TotalRuns] init 0;

	observe6: [0..TotalRuns] init 0;

	observe7: [0..TotalRuns] init 0;

	observe8: [0..TotalRuns] init 0;

	observe9: [0..TotalRuns] init 0;

	// the last seen crowd member
	lastSeen: [0..CrowdSize - 1] init 0;

	// get the protocol started
	[] phase=0 & runCount<TotalRuns -> (phase'=1) & (runCount'=runCount+1) & (lastSeen'=0);

	// decide whether crowd member is good or bad according to given probabilities
	[] phase=1 -> goodC : (phase'=2) & (good'=true) + badC : (phase'=2) & (good'=false);

	// if the current member is a good member, update the last seen index (chosen uniformly)
	[] phase=2 & good -> 1/10 : (lastSeen'=0) & (phase'=3) + 1/10 : (lastSeen'=1) & (phase'=3) + 1/10 : (lastSeen'=2) & (phase'=3) + 1/10 : (lastSeen'=3) & (phase'=3) + 1/10 : (lastSeen'=4) & (phase'=3) + 1/10 : (lastSeen'=5) & (phase'=3) + 1/10 : (lastSeen'=6) & (phase'=3) + 1/10 : (lastSeen'=7) & (phase'=3) + 1/10 : (lastSeen'=8) & (phase'=3) + 1/10 : (lastSeen'=9) & (phase'=3);

	// if the current member is a bad member, record the most recently seen index
	[] phase=2 & !good & lastSeen=0 & observe0 < TotalRuns -> (observe0'=observe0+1) & (phase'=4);
	[] phase=2 & !good & lastSeen=1 & observe1 < TotalRuns -> (observe1'=observe1+1) & (phase'=4);
	[] phase=2 & !good & lastSeen=2 & observe2 < TotalRuns -> (observe2'=observe2+1) & (phase'=4);
	[] phase=2 & !good & lastSeen=3 & observe3 < TotalRuns -> (observe3'=observe3+1) & (phase'=4);
	[] phase=2 & !good & lastSeen=4 & observe4 < TotalRuns -> (observe4'=observe4+1) & (phase'=4);
	[] phase=2 & !good & lastSeen=5 & observe5 < TotalRuns -> (observe5'=observe5+1) & (phase'=4);
	[] phase=2 & !good & lastSeen=6 & observe6 < TotalRuns -> (observe6'=observe6+1) & (phase'=4);
	[] phase=2 & !good & lastSeen=7 & observe7 < TotalRuns -> (observe7'=observe7+1) & (phase'=4);
	[] phase=2 & !good & lastSeen=8 & observe8 < TotalRuns -> (observe8'=observe8+1) & (phase'=4);
	[] phase=2 & !good & lastSeen=9 & observe9 < TotalRuns -> (observe9'=observe9+1) & (phase'=4);

	// good crowd members forward with probability PF and deliver otherwise
	[] phase=3 -> PF : (phase'=1) + notPF : (phase'=4);

	// deliver the message and start over
	[] phase=4 -> (phase'=0);

endmodule

label "observe0Greater1" = observe0 > 1;
label "observeIGreater1" = observe1 > 1 | observe2 > 1 | observe3 > 1 | observe4 > 1 | observe5 > 1 | observe6 > 1 | observe7 > 1 | observe8 > 1 | observe9 > 1;
label "observeOnlyTrueSender" = observe0 > 1 & observe1 <= 1 & observe2 <= 1 & observe3 <= 1 & observe4 <= 1 & observe5 <= 1 & observe6 <= 1 & observe7 <= 1 & observe8 <= 1 & observe9 <= 1;