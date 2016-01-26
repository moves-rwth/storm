dtmc

module tiny
  s : [0 .. 3] init 0;

  [] s = 0 -> 1/3 : (s'=1) + 1/3 : (s'=2) + 1/3 : (s'=3);
  [] s = 1 -> 1 : (s'=2);
  [] s = 2 -> 1/2 : (s'=2) + 1/2 : (s'=1);
  [] s = 3 -> 1 : (s'=3);

endmodule

rewards
  s=1 : 10;
  s=3 : 5;
endrewards
