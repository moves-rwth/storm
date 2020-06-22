// Model taken from Daws04
// This version by Ernst Moritz Hahn (emh@cs.uni-sb.de)

dtmc

const double pK;
const double pL;
const int n;

module main
  s: [-2..n+1];

  [b] (s=-1) -> (s'=-2);
  [a] (s=0) -> 1-pL : (s'=-1) + pL : (s'=1);
  [a] (s>0) & (s<n+1) -> 1-pK : (s'=0) + pK : (s'=s+1);

endmodule

init
  s = 0
endinit

rewards
 [a] true : 1;
 [b] true : n-1;
endrewards