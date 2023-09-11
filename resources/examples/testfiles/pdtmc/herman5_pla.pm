// herman's self stabilising algorithm [Her90]
// gxn/dxp 13/07/02

// the procotol is synchronous with no nondeterminism (a DTMC)
dtmc

// coin
const double p;

module tmp
    p0 : bool init true;
endmodule

// module for process 1
module process1

	// Boolean variable for process 1
	x1 : [0..1];
    x1new : [0..1];
    i1 : bool init false;
    c1 : bool init false;

    [initial] (!i1) -> 0.5 : (x1'=0) & (i1'=true) + 0.5 : (x1'=1) & (i1'=true);

	[] (i1) & (!c1) & (c0) & (x1=x0)  -> p : (x1new'=0) & (c1'=true) + 1-p : (x1new'=1) & (c1'=true);
	[] (i1) & (!c1) & (c0) & (x1!=x0) -> (x1new'=x0) & (c1'=true);
	
    [step] (c1) -> 1 : (c1'=false) & (x1'=x1new);
endmodule

// add further processes through renaming
module process2 = process1 [ x1=x2, x0=x1, i1=i2, c1=c2, c0=c1, x1new=x2new ] endmodule
module process3 = process1 [ x1=x3, x0=x2, i1=i3, c1=c3, c0=c2, x1new=x3new ] endmodule
module process4 = process1 [ x1=x4, x0=x3, i1=i4, c1=c4, c0=c3, x1new=x4new ] endmodule
module process5 = process1 [ x1=x0, x0=x4, i1=i5, c1=c0, c0=p0, x1new=x0new ] endmodule

formula initialized = i1 & i2 & i3 & i4 & i5;

// cost - 1 in each state (expected number of steps)
rewards "steps"
	[step] initialized : 1;
endrewards

// formula, for use in properties: number of tokens
// (i.e. number of processes that have the same value as the process to their left)
formula num_tokens = (x1=x0?1:0)
                    +(x2=x1?1:0)
                    +(x3=x2?1:0)
                    +(x4=x3?1:0)
                    +(x0=x4?1:0);

// label - stable configurations (1 token)
label "stable" = num_tokens=1 & initialized & !c0 & !c1 & !c2 & !c3 & !c4;

