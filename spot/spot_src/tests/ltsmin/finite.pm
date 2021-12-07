active proctype P() {
int a = 0;
int b = 0;
x: if
  :: d_step {a < 3 && b < 3; a = a + 1; } goto x;
  :: d_step {a < 3 && b < 3; b = b + 1; } goto x;
fi;
}
