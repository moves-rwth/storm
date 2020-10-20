// Translation of the MAPA Specification of a polling system into PRISM code
// http://wwwhome.cs.utwente.nl/~timmer/scoop/papers/qest13/index.html

ma

const int N; // number of job types (should be at most 6)
const int Q; // Maximum queue size in each station

// Formulae to control the LIFO queue of the stations.
// The queue is represented by some integer whose base N representation has at most Q digits, each representing one of the job types 0, 1, ..., N-1.
// In addition, we store the current size of the queue which is needed to distinguish an empty queue from a queue holding job of type 0
formula queue1_empty = q1Size=0;
formula queue1_full = q1Size=Q;
formula queue1_pop = floor(q1/N);
formula queue1_head = q1 - (queue1_pop * N); // i.e. q1 modulo N
formula queue1_push = q1*N;
formula queue2_empty = q2Size=0;
formula queue2_full = q2Size=Q;
formula queue2_pop = floor(q2/N);
formula queue2_head = q2 - (queue2_pop * N); // i.e. q2 modulo N
formula queue2_push = q2*N;

const int queue_maxValue = (N^Q)-1;

const double inRate1 = 3; // = (2 * #station) + 1;
const double inRate2 = 5; // = (2 * #station) + 1;

module pollingsys
	// The queues for the stations
	q1 : [0..queue_maxValue];
	q1Size : [0..Q];
	q2 : [0..queue_maxValue];
	q2Size : [0..Q];
 
	// Store the job that is currently processed by the server. j=N means that no job is processed.
	j : [0..N] init N;
	
	// Flag indicating whether a new job arrived
	newJob1 : bool init false;
	newJob2 : bool init false;
	
	//<>  !newJob1 & !newJob2 & !queue1_full &  queue2_full & j=N -> inRate1 : (newJob1'=true);
	//<>  !newJob1 & !newJob2 &  queue1_full & !queue2_full & j=N -> inRate2 : (newJob2'=true);
	<>  !newJob1 & !newJob2 & !queue1_full & !queue2_full & j=N -> inRate1 : (newJob1'=true) + inRate2 : (newJob2'=true);
	<>  !newJob1 & !newJob2 &  queue1_full &  queue2_full & j<N ->  2*(j+1) : (j'=N);
	<>  !newJob1 & !newJob2 & !queue1_full &  queue2_full & j<N -> inRate1 : (newJob1'=true) +  2*(j+1) : (j'=N);
	<>  !newJob1 & !newJob2 &  queue1_full & !queue2_full & j<N -> inRate2 : (newJob2'=true) +  2*(j+1) : (j'=N);
	<>  !newJob1 & !newJob2 & !queue1_full & !queue2_full & j<N -> inRate1 : (newJob1'=true) + inRate2 : (newJob2'=true) +  2*(j+1) : (j'=N);
	
	[] newJob1 & N>=1 -> 1 : (q1Size'=q1Size+1) &  (q1'=queue1_push+0) & (newJob1'=false);
	[] newJob1 & N>=2 -> 1 : (q1Size'=q1Size+1) &  (q1'=queue1_push+1) & (newJob1'=false); 
	[] newJob1 & N>=3 -> 1 : (q1Size'=q1Size+1) &  (q1'=queue1_push+2) & (newJob1'=false); 
	[] newJob1 & N>=4 -> 1 : (q1Size'=q1Size+1) &  (q1'=queue1_push+3) & (newJob1'=false); 
	[] newJob1 & N>=5 -> 1 : (q1Size'=q1Size+1) &  (q1'=queue1_push+4) & (newJob1'=false); 
	[] newJob1 & N>=6 -> 1 : (q1Size'=q1Size+1) &  (q1'=queue1_push+5) & (newJob1'=false); 	
	
	[] newJob2 & N>=1 -> 1 : (q2Size'=q2Size+1) &  (q2'=queue2_push+0) & (newJob2'=false);
	[] newJob2 & N>=2 -> 1 : (q2Size'=q2Size+1) &  (q2'=queue2_push+1) & (newJob2'=false); 
	[] newJob2 & N>=3 -> 1 : (q2Size'=q2Size+1) &  (q2'=queue2_push+2) & (newJob2'=false); 
	[] newJob2 & N>=4 -> 1 : (q2Size'=q2Size+1) &  (q2'=queue2_push+3) & (newJob2'=false); 
	[] newJob2 & N>=5 -> 1 : (q2Size'=q2Size+1) &  (q2'=queue2_push+4) & (newJob2'=false); 
	[] newJob2 & N>=6 -> 1 : (q2Size'=q2Size+1) &  (q2'=queue2_push+5) & (newJob2'=false); 
	
	[copy1] !newJob1 & !newJob2 & !queue1_empty & j=N -> 0.9 :  (j'=queue1_head) & (q1Size'=q1Size-1) & (q1'=queue1_pop)  + 0.1 : (j'=queue1_head);
	[copy2] !newJob1 & !newJob2 & !queue2_empty & j=N -> 0.9 :  (j'=queue2_head) & (q2Size'=q2Size-1) & (q2'=queue2_pop)  + 0.1 : (j'=queue2_head);
	
endmodule
 


label "q1full" = q1Size=Q;
label "q2full" = q2Size=Q;
label "allqueuesfull" = q1Size=Q & q2Size=Q;


// Rewards adapted from   Guck et al.: Modelling and Analysis of Markov Reward Automata

rewards "processedjobs1"
	[copy1] true : 0.1;
endrewards

rewards "processedjobs2"
	[copy2] true : 0.1;
endrewards

rewards "processedjobs"
	[copy1] true : 0.1;
	[copy2] true : 0.1;
endrewards

rewards "queuesize1"
	true : 0.01 * (q1Size);
endrewards

rewards "queuesize2"
	true : 0.01 * (q2Size);
endrewards

rewards "queuesize"
	true : 0.01 * (q1Size + q2Size);
endrewards
