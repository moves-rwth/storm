import os
import os.path
import subprocess
import re
import time
import math

STORM_PATH= "/Users/mvolk/develop/storm/build/src/storm-dft"
EXAMPLE_DIR= "/Users/mvolk/develop/storm/examples/dft/"


benchmarks = [ 
    ("and", False, 3),
    ("and_param", True, "(4*x^2+2*x+1)/((x) * (2*x+1))"),
    ("cm2", False, 0.256272),
    #("cm4", False, 0),
    ("cps", False, "inf"),
    #("fdep", False, 0),
    ("mdcs", False, 2.85414),
    ("mdcs2", False, 2.85414),
    ("mp", False, 1.66667),
    ("or", False, 1),
    ("pand", False, "inf"),
    ("pand_param", True, "-1"),
    ("spare", False, 3.53846),
    ("spare2", False, 1.86957),
    ("spare3", False, 1.27273),
    ("spare4", False, 4.8459),
    ("spare5", False, 2.16667),
    ("spare6", False, 1.4),
    ("tripple_and1", False, 4.16667),
    ("tripple_and2", False, 3.66667),
    ("tripple_and2_c", False, 3.6667),
    ("tripple_and_c", False, 4.16667),
    ("tripple_or", False, 0.5),
    ("tripple_or2", False, 0.666667),
    ("tripple_or2_c", False, 0.66667),
    ("tripple_or_c", False, 0.5),
    ("tripple_pand", False, "inf"),
    ("tripple_pand2", False, "inf"),
    ("tripple_pand2_c", False, "inf"),
    ("tripple_pand_c", False, "inf"),
    ("voting", False, 1.66667),
    ("voting2", False, 0.588235)
]

def run_storm_dft(filename, parametric, quiet):
    # Run storm-dft on filename and return result
    prop = "ET=? [F \"failed\"]"
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
    count = 0
    correct = 0
    start = time.time()
    for (benchmark, parametric, result_original) in benchmarks:
        # Run benchmark and check result
        count += 1;
        print("Running '{}'".format(benchmark))
        result = run_storm_dft(benchmark, parametric, True)
        if not parametric:
            # Float
            result = float(result)
            if not math.isclose(result, float(result_original), rel_tol=1e-05):
                print("!!! File '{}': result: {}, Expected: {}".format(benchmark, result, result_original))
            else:
                correct += 1
        else:
            # Parametric
            if result != result_original:
                print("!!! File {}: result: {}, Expected: {}".format(benchmark, result, result_original))
            else:
                correct += 1
    end = time.time()
    print("Correct results for {} of {} DFTs in {}s".format(correct, count, end-start))
