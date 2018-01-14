// CROWDS [Reiter,Rubin]
// Vitaly Shmatikov, 2002

// Note:
// Change everything marked CWDSIZ when changing the size of the crowd
// Change everything marked CWDMAX when increasing max size of the crowd

dtmc

// Probability of forwarding
const double PF = 0.8;

// Probability that a crowd member is bad
const double  badC = 0.091;
// const double  badC = 0.167;

const int CrowdSize; // CWDSIZ: actual number of good crowd members
const int MaxGood=20; // CWDMAX: maximum number of good crowd members

// Process definitions
module crowds

	// Auxiliary variables
	launch:   bool init true;       // Start modeling?
	new:      bool init false;      // Initialize a new protocol instance?
	start:    bool init false;      // Start the protocol?
	run:      bool init false;      // Run the protocol?
	lastSeen: [0..MaxGood] init MaxGood;   // Last crowd member to touch msg
	good:     bool init false;      // Crowd member is good?
	bad:      bool init false;      //              ... bad?
	recordLast: bool init false;    // Record last seen crowd member?
	badObserve: bool init false;    // Bad members observes who sent msg?
	deliver:  bool init false;      // Deliver message to destination?
	done:     bool init false;      // Protocol instance finished?
	
	[] launch -> (new'=true) & (launch'=false);
	// Set up a new protocol instance
	[newrun] new -> (new'=false) & (start'=true);
	
	// SENDER
	// Start the protocol
	[] start -> (lastSeen'=0) & (run'=true) & (deliver'=false) & (start'=false);
	
	// CROWD MEMBERS
	// Good or bad crowd member?
	[] !good & !bad & !deliver & run ->
	             1-badC : (good'=true) & (recordLast'=true) & (run'=false) +
	               badC : (bad'=true)  & (badObserve'=true) & (run'=false);

	// GOOD MEMBERS
	// Forward with probability PF, else deliver
	[] good & !deliver & run -> PF : (good'=false) + 1-PF : (deliver'=true);
	// Record the last crowd member who touched the msg;
	// all good members may appear with equal probability
	//    Note: This is backward.  In the real protocol, each honest
	//          forwarder randomly chooses the next forwarder.
	//          Here, the identity of an honest forwarder is randomly
	//          chosen *after* it has forwarded the message.
	[] recordLast & CrowdSize=2 ->
	        1/2 : (lastSeen'=0) & (recordLast'=false) & (run'=true) +
	        1/2 : (lastSeen'=1) & (recordLast'=false) & (run'=true);
	[] recordLast & CrowdSize=4 ->
	        1/4 : (lastSeen'=0) & (recordLast'=false) & (run'=true) +
	        1/4 : (lastSeen'=1) & (recordLast'=false) & (run'=true) +
	        1/4 : (lastSeen'=2) & (recordLast'=false) & (run'=true) +
	        1/4 : (lastSeen'=3) & (recordLast'=false) & (run'=true);
	[] recordLast & CrowdSize=5 ->
	        1/5 : (lastSeen'=0) & (recordLast'=false) & (run'=true) +
	        1/5 : (lastSeen'=1) & (recordLast'=false) & (run'=true) +
	        1/5 : (lastSeen'=2) & (recordLast'=false) & (run'=true) +
	        1/5 : (lastSeen'=3) & (recordLast'=false) & (run'=true) +
	        1/5 : (lastSeen'=4) & (recordLast'=false) & (run'=true);
	[] recordLast & CrowdSize=10 ->
	        1/10 : (lastSeen'=0) & (recordLast'=false) & (run'=true) +
	        1/10 : (lastSeen'=1) & (recordLast'=false) & (run'=true) +
	        1/10 : (lastSeen'=2) & (recordLast'=false) & (run'=true) +
	        1/10 : (lastSeen'=3) & (recordLast'=false) & (run'=true) +
	        1/10 : (lastSeen'=4) & (recordLast'=false) & (run'=true) +
	        1/10 : (lastSeen'=5) & (recordLast'=false) & (run'=true) +
	        1/10 : (lastSeen'=6) & (recordLast'=false) & (run'=true) +
	        1/10 : (lastSeen'=7) & (recordLast'=false) & (run'=true) +
	        1/10 : (lastSeen'=8) & (recordLast'=false) & (run'=true) +
	        1/10 : (lastSeen'=9) & (recordLast'=false) & (run'=true);
	[] recordLast & CrowdSize=15 ->
	        1/15 : (lastSeen'=0)  & (recordLast'=false) & (run'=true) +
	        1/15 : (lastSeen'=1)  & (recordLast'=false) & (run'=true) +
	        1/15 : (lastSeen'=2)  & (recordLast'=false) & (run'=true) +
	        1/15 : (lastSeen'=3)  & (recordLast'=false) & (run'=true) +
	        1/15 : (lastSeen'=4)  & (recordLast'=false) & (run'=true) +
	        1/15 : (lastSeen'=5)  & (recordLast'=false) & (run'=true) +
	        1/15 : (lastSeen'=6)  & (recordLast'=false) & (run'=true) +
	        1/15 : (lastSeen'=7)  & (recordLast'=false) & (run'=true) +
	        1/15 : (lastSeen'=8)  & (recordLast'=false) & (run'=true) +
	        1/15 : (lastSeen'=9)  & (recordLast'=false) & (run'=true) +
	        1/15 : (lastSeen'=10) & (recordLast'=false) & (run'=true) +
	        1/15 : (lastSeen'=11) & (recordLast'=false) & (run'=true) +
	        1/15 : (lastSeen'=12) & (recordLast'=false) & (run'=true) +
	        1/15 : (lastSeen'=13) & (recordLast'=false) & (run'=true) +
	        1/15 : (lastSeen'=14) & (recordLast'=false) & (run'=true);
	[] recordLast & CrowdSize=20 ->
	        1/20 : (lastSeen'=0)  & (recordLast'=false) & (run'=true) +
	        1/20 : (lastSeen'=1)  & (recordLast'=false) & (run'=true) +
	        1/20 : (lastSeen'=2)  & (recordLast'=false) & (run'=true) +
	        1/20 : (lastSeen'=3)  & (recordLast'=false) & (run'=true) +
	        1/20 : (lastSeen'=4)  & (recordLast'=false) & (run'=true) +
	        1/20 : (lastSeen'=5)  & (recordLast'=false) & (run'=true) +
	        1/20 : (lastSeen'=6)  & (recordLast'=false) & (run'=true) +
	        1/20 : (lastSeen'=7)  & (recordLast'=false) & (run'=true) +
	        1/20 : (lastSeen'=8)  & (recordLast'=false) & (run'=true) +
	        1/20 : (lastSeen'=9)  & (recordLast'=false) & (run'=true) +
	        1/20 : (lastSeen'=10) & (recordLast'=false) & (run'=true) +
	        1/20 : (lastSeen'=11) & (recordLast'=false) & (run'=true) +
	        1/20 : (lastSeen'=12) & (recordLast'=false) & (run'=true) +
	        1/20 : (lastSeen'=13) & (recordLast'=false) & (run'=true) +
	        1/20 : (lastSeen'=14) & (recordLast'=false) & (run'=true) +
	        1/20 : (lastSeen'=15) & (recordLast'=false) & (run'=true) +
	        1/20 : (lastSeen'=16) & (recordLast'=false) & (run'=true) +
	        1/20 : (lastSeen'=17) & (recordLast'=false) & (run'=true) +
	        1/20 : (lastSeen'=18) & (recordLast'=false) & (run'=true) +
	        1/20 : (lastSeen'=19) & (recordLast'=false) & (run'=true);
	
	// BAD MEMBERS
	// Remember from whom the message was received and deliver
	// CWDMAX: 1 rule per each good crowd member
	[obs0]  lastSeen=0  & badObserve -> (deliver'=true) & (run'=true) & (badObserve'=false);
	[obs1]  lastSeen=1  & badObserve -> (deliver'=true) & (run'=true) & (badObserve'=false);
	[obs2]  lastSeen=2  & badObserve -> (deliver'=true) & (run'=true) & (badObserve'=false);
	[obs3]  lastSeen=3  & badObserve -> (deliver'=true) & (run'=true) & (badObserve'=false);
	[obs4]  lastSeen=4  & badObserve -> (deliver'=true) & (run'=true) & (badObserve'=false);
	[obs5]  lastSeen=5  & badObserve -> (deliver'=true) & (run'=true) & (badObserve'=false);
	[obs6]  lastSeen=6  & badObserve -> (deliver'=true) & (run'=true) & (badObserve'=false);
	[obs7]  lastSeen=7  & badObserve -> (deliver'=true) & (run'=true) & (badObserve'=false);
	[obs8]  lastSeen=8  & badObserve -> (deliver'=true) & (run'=true) & (badObserve'=false);
	[obs9]  lastSeen=9  & badObserve -> (deliver'=true) & (run'=true) & (badObserve'=false);
	[obs10] lastSeen=10 & badObserve -> (deliver'=true) & (run'=true) & (badObserve'=false);
	[obs11] lastSeen=11 & badObserve -> (deliver'=true) & (run'=true) & (badObserve'=false);
	[obs12] lastSeen=12 & badObserve -> (deliver'=true) & (run'=true) & (badObserve'=false);
	[obs13] lastSeen=13 & badObserve -> (deliver'=true) & (run'=true) & (badObserve'=false);
	[obs14] lastSeen=14 & badObserve -> (deliver'=true) & (run'=true) & (badObserve'=false);
	[obs15] lastSeen=15 & badObserve -> (deliver'=true) & (run'=true) & (badObserve'=false);
	[obs16] lastSeen=16 & badObserve -> (deliver'=true) & (run'=true) & (badObserve'=false);
	[obs17] lastSeen=17 & badObserve -> (deliver'=true) & (run'=true) & (badObserve'=false);
	[obs18] lastSeen=18 & badObserve -> (deliver'=true) & (run'=true) & (badObserve'=false);
	[obs19] lastSeen=19 & badObserve -> (deliver'=true) & (run'=true) & (badObserve'=false);

	// RECIPIENT
	// Delivery to destination
	[] deliver & run -> (done'=true) & (deliver'=false) & (run'=false) & (good'=false) & (bad'=false);
	// Start a new instance
	[] done -> (new'=true) & (done'=false) & (run'=false) & (lastSeen'=MaxGood);
	
endmodule

rewards "num_runs"
	[newrun] true : 1;
endrewards

rewards "observe0"
	[obs0]  true : 1;
endrewards

rewards "observe1"
	[obs1]  true : 1;
endrewards

rewards "observe2"
	[obs2]  true : 1;
endrewards

rewards "observe3"
	[obs3]  true : 1;
endrewards

rewards "observe4"
	[obs4]  true : 1;
endrewards

rewards "observe5"
	[obs5]  true : 1;
endrewards

rewards "observe6"
	[obs6]  true : 1;
endrewards

rewards "observe7"
	[obs7]  true : 1;
endrewards

rewards "observe8"
	[obs8]  true : 1;
endrewards

rewards "observe9"
	[obs9]  true : 1;
endrewards

rewards "observe10"
	[obs10] true : 1;
endrewards

rewards "observe11"
	[obs11] true : 1;
endrewards

rewards "observe12"
	[obs12] true : 1;
endrewards

rewards "observe13"
	[obs13] true : 1;
endrewards

rewards "observe14"
	[obs14] true : 1;
endrewards

rewards "observe15"
	[obs15] true : 1;
endrewards

rewards "observe16"
	[obs16] true : 1;
endrewards

rewards "observe17"
	[obs17] true : 1;
endrewards

rewards "observe18"
	[obs18] true : 1;
endrewards

rewards "observe19"
	[obs19] true : 1;
endrewards

rewards "observeI"
	[obs1] true : 1;
	[obs2] true : 1;
	[obs3] true : 1;
	[obs4] true : 1;
	[obs5] true : 1;
	[obs6] true : 1;
	[obs7] true : 1;
	[obs8] true : 1;
	[obs9] true : 1;
	[obs10] true : 1;
	[obs11] true : 1;
	[obs12] true : 1;
	[obs13] true : 1;
	[obs14] true : 1;
	[obs15] true : 1;
	[obs16] true : 1;
	[obs17] true : 1;
	[obs18] true : 1;
	[obs19] true : 1;
endrewards


