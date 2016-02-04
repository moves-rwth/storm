import os
import os.path
import subprocess
import re
import time
import math
import argparse

STORM_PATH= "/Users/mvolk/develop/storm/build/src/storm-dft"
EXAMPLE_DIR= "/Users/mvolk/develop/storm/examples/dft/"


benchmarks = [ 
    ("and", False, [3, 1]),
    ("and_param", True, ["(4*x^2+2*x+1)/((x) * (2*x+1))", "1"]),
    ("cm2", False, [0.256272, 1]),
    #("cm4", False, [0, 1]),
    ("cps", False, ["inf", 0.333333]),
    #("fdep", False, [0, 1]),
    ("mdcs", False, [2.85414, 1]),
    ("mdcs2", False, [2.85414, 1]),
    ("mp", False, [1.66667, 1]),
    ("or", False, [1, 1]),
    ("pand", False, ["inf", 0.666667]),
    ("pand_param", True, ["-1", "(x)/(y+x)"]),
    ("spare", False, [3.53846, 1]),
    ("spare2", False, [1.86957, 1]),
    ("spare3", False, [1.27273, 1]),
    ("spare4", False, [4.8459, 1]),
    ("spare5", False, [2.66667, 1]), # We discard the result 2.16667 from DFTCalc
    ("spare6", False, [1.4, 1]),
    ("spare7", False, [3.67333, 1]),
    ("tripple_and1", False, [4.16667, 1]),
    ("tripple_and2", False, [3.66667, 1]),
    ("tripple_and2_c", False, [3.6667, 1]),
    ("tripple_and_c", False, [4.16667, 1]),
    ("tripple_or", False, [0.5, 1]),
    ("tripple_or2", False, [0.666667, 1]),
    ("tripple_or2_c", False, [0.66667, 1]),
    ("tripple_or_c", False, [0.5, 1]),
    ("tripple_pand", False, ["inf", 0.0416667]),
    ("tripple_pand2", False, ["inf", 0.166667]),
    ("tripple_pand2_c", False, ["inf", 0.166667]),
    ("tripple_pand_c", False, ["inf", 0.0416667]),
    ("voting", False, [1.66667, 1]),
    ("voting2", False, [0.588235, 1])
]

def run_storm_dft(filename, prop, parametric, quiet):
    # Run storm-dft on filename and return result
    dft_file = os.path.join(EXAMPLE_DIR, filename + ".dft")
    args = [STORM_PATH,
            dft_file,
            '--prop', prop]
    if parametric:
        args.append('--parametric')

    output = run_tool(args, quiet)
    # Get result
    match = re.search(r'Result: \[(.*)\]', output)
    if not match:
        print("No valid result found in: " + output)
        return
    
    result = match.group(1)
    return result

def run_tool(args, quiet=False):
    """
    Executes a process,
    :returns: the `stdout`
    """
    pipe = subprocess.Popen(args, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
    result = "";
    for line in iter(pipe.stdout.readline, ""):
        if not line and pipe.poll() is not None:
            break
        output = line.decode(encoding='UTF-8').rstrip()
        if output != "":
            if  not quiet:
                print("\t * " + output)
            result = output
    return result

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='Benchmarking DFTs via Storm')
    parser.add_argument('--debuglevel', type=int, default=0, help='the debug level (0=silent, 1=print benchmarks, 2=print output from storm')
    args = parser.parse_args()
    count = 0
    correct = 0
    properties = ["ET=? [F \"failed\"]", "P=? [F \"failed\"]"]
    start = time.time()
    for index, prop in enumerate(properties):
        for (benchmark, parametric, result_original) in benchmarks:
            expected_result = result_original[index]
            # Run benchmark and check result
            count += 1;
            if args.debuglevel > 0:
                print("Running '{}' with property '{}'".format(benchmark, prop))
            result = run_storm_dft(benchmark, prop, parametric, args.debuglevel<2)
            if not parametric:
                # Float
                result = float(result)
                if not math.isclose(result, float(expected_result), rel_tol=1e-05):
                    print("Wrong result on example '{}' with property '{}': result: {}, Expected: {}".format(benchmark, prop, result, expected_result))
                else:
                    correct += 1
            else:
                # Parametric
                if result != expected_result:
                    print("Wrong result on example '{}' with property '{}': result: {}, Expected: {}".format(benchmark, prop, result, expected_result))
                else:
                    correct += 1
    end = time.time()
    print("Correct results for {} of {} DFT checks in {}s".format(correct, count, end-start))
