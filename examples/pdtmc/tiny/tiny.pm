dtmc

module tiny
  s : [0 .. 2] init 0;

  [] s = 0 -> 1/2 : (s'=1) + 1/2 : (s'=2);
  [] s = 1 -> 1 : (s'=2);
  [] s = 2 -> 1/2 : (s'=2) + 1/2 : (s'=1);

endmodule

