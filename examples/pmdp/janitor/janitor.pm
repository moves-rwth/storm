// GRID WORLD MODEL OF ROBOT AND JANITOR
// Hakan Younes/gxn/dxp 04/05/04
// Variant by Sebastian Junges, RWTH Aachen University
// As described in 
// Junges, Jansen, Dehnert, Topcu, Katoen: 
// Safety Constrained Reinforcement Learning
// Proc. of TACAS’16

mdp

// PARAMETERS: Probability that the janitor moves North, South, East or West
const double jProbN;
const double jProbE;
const double jProbS = 1 - jProbN;
const double jProbW = 1 - jProbE;

// CONSTANTS
const int xSize = 6; // size of the grid
const int ySize = 6;
const int jXmin = 1; // janitor area
const int jXmax = 6;
const int jYmin = 1;
const int jYmax = 6;
const int maxSteps = 15;

formula T = (xR = xSize & yR = ySize);
formula C = (xR = xJ & yR = yJ);

module counter
	count : [0..maxSteps] init 0;
	[forward] (count<maxSteps & !C) -> 1:(count'=count+1);
	[turnLeft] (count<maxSteps & !C) -> 1:(count'=count+1);
	[turnRight] (count<maxSteps & !C) -> 1:(count'=count+1);
	[forward] (count<maxSteps & C) -> 1:(count'=maxSteps);
	[turnLeft] (count<maxSteps & C) -> 1:(count'=maxSteps);
	[turnRight] (count<maxSteps & C) -> 1:(count'=maxSteps);
	[forward] (count=maxSteps) -> 1:true;
	[turnLeft] (count=maxSteps) -> 1:true;
	[turnRight] (count=maxSteps) -> 1:true;
endmodule

module robot
	xR : [1..xSize] init 1;
	yR : [1..ySize] init 1;
	d: [0..3] init 0;

	  [forward] (d=0 & yR < ySize & !T) -> 1:(yR'=yR+1);
   	[forward] (d=2 & yR > 1 & !T) -> 1:(yR'=yR-1);
 		[forward] (d=1 & xR < xSize & !T) -> 1:(xR'=xR+1);
 		[forward] (d=3 & xR > 1 & !T) -> 1:(xR'=xR-1);
  	//[backward] (d=0 & yR > 1 & !T) -> 1:(yR'=yR-1);
  	//[backward] (d=2 & yR < ySize & !T) -> 1:(yR'=yR+1);
  	//[backward] (d=1 & xR > 1 & !T) -> 1:(xR'=xR-1);
  	//[backward] (d=3 & xR < xSize & !T) -> 1:(xR'=xR+1);
  	[turnLeft] (d>0 & !T) -> 1:(d'=d-1);
  	[turnLeft] (d=0 & !T) -> 1:(d'=3);
  	[turnRight] (d=3 & !T) -> 1:(d'=0);
  	[turnRight] (d<3 & !T) -> 1:(d'=d+1);
	// [turnLeft] (d=1) -> 1:(d’=0);
	// [turnRight] (d=0) -> 1:(d’=1);
		[done] T -> 1:true;
endmodule

// The Janitor moves probabilistically within the grid.
module janitor
	xJ : [jXmin..jXmax] init jXmax; // x position of janitor
	yJ : [jYmin..jYmax] init jYmin; // y position of janitor

	[forward] (yJ=jYmax & xJ=jXmax) -> 0.5:(yJ'=yJ-1) + 0.5:(xJ'=xJ-1);
	[forward] (yJ=jYmax & xJ=jXmin) -> 0.5:(yJ'=yJ-1) + 0.5:(xJ'=xJ+1);
	[forward] (yJ=jYmax & xJ<jXmax & xJ > jXmin) -> 0.3333:(yJ'=yJ-1) + 0.6667*jProbW:(xJ'=xJ-1) + 0.6667*jProbE:(xJ'=xJ+1);
	[forward] (yJ=jYmin & xJ=jXmax) -> 0.5:(yJ'=yJ+1) + 0.5:(xJ'=xJ-1);
	[forward] (yJ=jYmin & xJ=jXmin) -> 0.5:(yJ'=yJ+1) + 0.5:(xJ'=xJ+1);
	[forward] (yJ=jYmin & xJ<jXmax & xJ>jXmin) -> 0.3333:(yJ'=yJ+1) + 0.6667*jProbW:(xJ'=xJ-1) + 0.6667*jProbE:(xJ'=xJ+1);
	[forward] (yJ<jYmax & yJ>jYmin & xJ=jXmax) -> 0.6667*jProbN:(yJ'=yJ+1) + 0.6667*jProbS:(yJ'=yJ-1) + 0.3333:(xJ'=xJ-1);
	[forward] (yJ<jYmax & yJ>jYmin & xJ=jXmin) -> 0.6667*jProbN:(yJ'=yJ+1) + 0.6667*jProbS:(yJ'=yJ-1) + 0.3333:(xJ'=xJ+1);
	[forward] (yJ<jYmax & yJ>jYmin & xJ<jXmax & xJ>jXmin) -> 0.5*jProbS:(yJ'=yJ-1) + 0.5*jProbN:(yJ'=yJ+1) + 0.5*jProbW:(xJ'=xJ-1) + 0.5*jProbE:(xJ'=xJ+1);


	[turnLeft] (yJ=jYmax & xJ=jXmax) -> 0.5:(yJ'=yJ-1) + 0.5:(xJ'=xJ-1);
	[turnLeft] (yJ=jYmax & xJ=jXmin) -> 0.5:(yJ'=yJ-1) + 0.5:(xJ'=xJ+1);
	[turnLeft] (yJ=jYmax & xJ<jXmax & xJ > jXmin) -> 0.3333:(yJ'=yJ-1) + 0.6667*jProbW:(xJ'=xJ-1) + 0.6667*jProbE:(xJ'=xJ+1);
	[turnLeft] (yJ=jYmin & xJ=jXmax) -> 0.5:(yJ'=yJ+1) + 0.5:(xJ'=xJ-1);
	[turnLeft] (yJ=jYmin & xJ=jXmin) -> 0.5:(yJ'=yJ+1) + 0.5:(xJ'=xJ+1);
	[turnLeft] (yJ=jYmin & xJ<jXmax & xJ>jXmin) -> 0.3333:(yJ'=yJ+1) + 0.6667*jProbW:(xJ'=xJ-1) + 0.6667*jProbE:(xJ'=xJ+1);
	[turnLeft] (yJ<jYmax & yJ>jYmin & xJ=jXmax) -> 0.6667*jProbN:(yJ'=yJ+1) + 0.6667*jProbS:(yJ'=yJ-1) + 0.3333:(xJ'=xJ-1);
	[turnLeft] (yJ<jYmax & yJ>jYmin & xJ=jXmin) -> 0.6667*jProbN:(yJ'=yJ+1) + 0.6667*jProbS:(yJ'=yJ-1) + 0.3333:(xJ'=xJ+1);
	[turnLeft] (yJ<jYmax & yJ>jYmin & xJ<jXmax & xJ>jXmin) -> 0.5*jProbS:(yJ'=yJ-1) + 0.5*jProbN:(yJ'=yJ+1) + 0.5*jProbW:(xJ'=xJ-1) + 0.5*jProbE:(xJ'=xJ+1);

	[turnRight] (yJ=jYmax & xJ=jXmax) -> 0.5:(yJ'=yJ-1) + 0.5:(xJ'=xJ-1);
	[turnRight] (yJ=jYmax & xJ=jXmin) -> 0.5:(yJ'=yJ-1) + 0.5:(xJ'=xJ+1);
	[turnRight] (yJ=jYmax & xJ<jXmax & xJ > jXmin) -> 0.3333:(yJ'=yJ-1) + 0.6667*jProbW:(xJ'=xJ-1) + 0.6667*jProbE:(xJ'=xJ+1);
	[turnRight] (yJ=jYmin & xJ=jXmax) -> 0.5:(yJ'=yJ+1) + 0.5:(xJ'=xJ-1);
	[turnRight] (yJ=jYmin & xJ=jXmin) -> 0.5:(yJ'=yJ+1) + 0.5:(xJ'=xJ+1);
	[turnRight] (yJ=jYmin & xJ<jXmax & xJ>jXmin) -> 0.3333:(yJ'=yJ+1) + 0.6667*jProbW:(xJ'=xJ-1) + 0.6667*jProbE:(xJ'=xJ+1);
	[turnRight] (yJ<jYmax & yJ>jYmin & xJ=jXmax) -> 0.6667*jProbN:(yJ'=yJ+1) + 0.6667*jProbS:(yJ'=yJ-1) + 0.3333:(xJ'=xJ-1);
	[turnRight] (yJ<jYmax & yJ>jYmin & xJ=jXmin) -> 0.6667*jProbN:(yJ'=yJ+1) + 0.6667*jProbS:(yJ'=yJ-1) + 0.3333:(xJ'=xJ+1);
	[turnRight] (yJ<jYmax & yJ>jYmin & xJ<jXmax & xJ>jXmin) -> 0.5*jProbS:(yJ'=yJ-1) + 0.5*jProbN:(yJ'=yJ+1) + 0.5*jProbW:(xJ'=xJ-1) + 0.5*jProbE:(xJ'=xJ+1);

endmodule

rewards "mvbased"
	[forward] true: 5;
	//[backward] true: 10;
	[turnLeft] true: 25;
	[turnRight] true: 28;
endrewards

rewards "mvbased_upper"
	[forward] true: 30;
	//[backward] true: 35;
	[turnLeft] true: 40;
	[turnRight] true: 40;
endrewards

rewards "mvbased_lower"
	[forward] true: 4;
	//[backward] true: 4;
	[turnLeft] true: 8;
	[turnRight] true: 8;
endrewards

label "Target" = T & (count<maxSteps) & !C;
label "Crash" = C;
