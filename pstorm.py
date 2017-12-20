import sys
import os 
import subprocess 
prop = ' --prop \"Pmax=? [F<1 \\"goal\\"]\" --ma:technique unifplus'
storm= '/home/timo/ustorm/build/bin/storm'


if len(sys.argv)<2:
    print("no input file found\n")
    exit()

for a in sys.argv[1:]:
    file = " --prism " + a
    cmd = storm + file + prop
    os.system(cmd)
