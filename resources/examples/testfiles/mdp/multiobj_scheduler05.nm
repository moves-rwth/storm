mdp

label "tasks_complete" = (task6=3);

const int K=5;

module scheduler

	task1 : [0..3];
	task2 : [0..3];
	task3 : [0..3];
	task4 : [0..3];
	task5 : [0..3];
	task6 : [0..3];

	[p1_add] task1=0 -> (task1'=1);
	[p2_add] task1=0 -> (task1'=2);
	[p1_mult] task2=0 -> (task2'=1);
	[p2_mult] task2=0 -> (task2'=2);
	[p1_mult] task3=0&task1=3 -> (task3'=1);
	[p2_mult] task3=0&task1=3 -> (task3'=2);
	[p1_add] task4=0&task1=3&task2=3 -> (task4'=1);
	[p2_add] task4=0&task1=3&task2=3 -> (task4'=2);
	[p1_mult] task5=0&task3=3 -> (task5'=1);
	[p2_mult] task5=0&task3=3 -> (task5'=2);
	[p1_add] task6=0&task4=3&task5=3 -> (task6'=1);
	[p2_add] task6=0&task4=3&task5=3 -> (task6'=2);
	[p1_done] task1=1 -> (task1'=3);
	[p1_done] task2=1 -> (task2'=3);
	[p1_done] task3=1 -> (task3'=3);
	[p1_done] task4=1 -> (task4'=3);
	[p1_done] task5=1 -> (task5'=3);
	[p1_done] task6=1 -> (task6'=3);
	[p2_done] task1=2 -> (task1'=3);
	[p2_done] task2=2 -> (task2'=3);
	[p2_done] task3=2 -> (task3'=3);
	[p2_done] task4=2 -> (task4'=3);
	[p2_done] task5=2 -> (task5'=3);
	[p2_done] task6=2 -> (task6'=3);
	[time] true -> 1.0 : true;

endmodule

module P1

	p1 : [0..3];
	c1 : [0..2];
	x1 : [0..4*K+1];

	[p1_add] (p1=0) -> (p1'=1) & (x1'=0);
	[] (p1=1)&(x1=1*K)&(c1=0) -> 1/3 : (p1'=3) & (x1'=0) & (c1'=0) + 2/3 : (c1'=1) & (x1'=0);
	[] (p1=1)&(x1=1*K)&(c1=1) -> 1/2 : (p1'=3) & (x1'=0) & (c1'=0) + 1/2 : (c1'=2) & (x1'=0);
	[p1_done] (p1=1)&(x1=1*K)&(c1=2) -> (p1'=0) & (x1'=0) & (c1'=0);
	[p1_mult] (p1=0) -> (p1'=2) & (x1'=0);
	[] (p1=2)&(x1=2*K)&(c1=0) -> 1/3 : (p1'=3) & (x1'=0) & (c1'=0) + 2/3 : (c1'=1) & (x1'=0);
	[] (p1=2)&(x1=1*K)&(c1=1) -> 1/2 : (p1'=3) & (x1'=0) & (c1'=0) + 1/2 : (c1'=2) & (x1'=0);
	[p1_done] (p1=2)&(x1=1*K)&(c1=2) -> (p1'=0) & (x1'=0) & (c1'=0);
	[p1_done] (p1=3) -> (p1'=0);
	[time] (p1=1=>x1+1<=1*K)&((p1=2&c1=0)=>x1+1<=2*K)&((p1=2&c1>0)=>x1+1<=1*K)&(p1=3=>x1+1<=0) -> 1.0 : (x1'=min(x1+1,4*K+1));

endmodule

module P2

	p2 : [0..3];
	c2 : [0..2];
	x2 : [0..6*K+1];

	[p2_add] (p2=0) -> (p2'=1) & (x2'=0);
	[] (p2=1)&(x2=4*K)&(c2=0) -> 1/3 : (p2'=3) & (x2'=0) & (c2'=0) + 2/3 : (c2'=1) & (x2'=0);
	[] (p2=1)&(x2=1)&(c2=1) -> 1/2 : (p2'=3) & (x2'=0) & (c2'=0) + 1/2 : (c2'=2) & (x2'=0);
	[p2_done] (p2=1)&(x2=1)&(c2=2) -> (p2'=0) & (x2'=0) & (c2'=0);
	[p2_mult] (p2=0) -> (p2'=2) & (x2'=0);
	[] (p2=2)&(x2=6*K)&(c2=0) -> 1/3 : (p2'=3) & (x2'=0) & (c2'=0) + 2/3 : (c2'=1) & (x2'=0);
	[] (p2=2)&(x2=1)&(c2=1) -> 1/2 : (p2'=3) & (x2'=0) & (c2'=0) + 1/2 : (c2'=2) & (x2'=0);
	[p2_done] (p2=2)&(x2=1)&(c2=2) -> (p2'=0) & (x2'=0) & (c2'=0);
	[p2_done] (p2=3) -> (p2'=0);
	[time] ((p2=1&c2=0)=>x2+1<=4*K)&((p2=1&c2>0)=>x2+1<=1)&((p2=2&c2=0)=>x2+1<=6*K)&((p2=2&c2>0)=>x2+1<=1)&(p2=3=>x2+1<=0) -> 1.0 : (x2'=min(x2+1,6*K+1));

endmodule

rewards "time" 

	[time] true : 1/K;

endrewards

rewards "energy" 

	[time] p1=0 : 10/(1000*K);
	[time] p1>0 : 90/(1000*K);
	[time] p2=0 : 20/(1000*K);
	[time] p2>0 : 30/(1000*K);

endrewards
