
// Stochastic Job Scheduling, based on []
// Encoding by Junges & Quatmann
// RWTH Aachen University
// Please cite Quatmann et al: Multi-objective Model Checking of Markov Automata
ma

const double x_j1 = 1.0;
const double x_j2 = 2.0;
const double x_j3 = 3.0;
formula is_running = r_j1 + r_j2 + r_j3 > 0;
formula num_finished = f_j1 + f_j2 + f_j3;
module main
	r_j1 : [0..1];
	r_j2 : [0..1];
	r_j3 : [0..1];
	f_j1 : [0..1];
	f_j2 : [0..1];
	f_j3 : [0..1];
	<> (r_j1 = 1)  -> x_j1 : (r_j1' = 0) & (r_j2' = 0) & (r_j3' = 0) & (f_j1' = 1);
	<> (r_j2 = 1)  -> x_j2 : (r_j1' = 0) & (r_j2' = 0) & (r_j3' = 0) & (f_j2' = 1);
	<> (r_j3 = 1)  -> x_j3 : (r_j1' = 0) & (r_j2' = 0) & (r_j3' = 0) & (f_j3' = 1);
	[] (!is_running) & (num_finished = 2) & (f_j1 = 0) -> 1: (r_j1' = 1);
	[] (!is_running) & (num_finished = 2) & (f_j2 = 0) -> 1: (r_j2' = 1);
	[] (!is_running) & (num_finished = 2) & (f_j3 = 0) -> 1: (r_j3' = 1);
	[] (!is_running) & (num_finished <= 1) & (f_j1 = 0) & (f_j2 = 0) -> 1: (r_j1' = 1) & (r_j2' = 1);
	[] (!is_running) & (num_finished <= 1) & (f_j1 = 0) & (f_j3 = 0) -> 1: (r_j1' = 1) & (r_j3' = 1);
	[] (!is_running) & (num_finished <= 1) & (f_j2 = 0) & (f_j3 = 0) -> 1: (r_j2' = 1) & (r_j3' = 1);
endmodule
init
	r_j1 = 0 &
	r_j2 = 0 &
	r_j3 = 0 &
	f_j1 = 0 &
	f_j2 = 0 &
	f_j3 = 0
endinit
label "all_jobs_finished" = num_finished=3;
label "half_of_jobs_finished" = num_finished=2;
label "one_job_finished" = num_finished=1;
label "slowest_before_fastest" = f_j1=1 & f_j3=0;
rewards "avg_waiting_time"
 true : (3-num_finished)/3;
endrewards
