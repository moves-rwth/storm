import argparse
import subprocess
import os

def get_dependencies(file):
    # Call otool -L file to obtain the dependencies.
    proc = subprocess.Popen(["otool", "-L", args.binary], stdout=subprocess.PIPE)
    result = {}
    for line_bytes in proc.stdout:
        line = line_bytes.decode("utf-8").strip()
        lib = line.split()[0]
        if (lib.startswith("@")):
            lib = lib.split("/", 1)[1]
        (base, file) = os.path.split(lib)
        print(base + " // " + file)
    
    return result

def parse_arguments():
    parser = argparse.ArgumentParser(description='Package the storm binary on Mac OS.')
    parser.add_argument('binary', help='the binary to package')
    args = parser.parse_args()
    return args

dep_exeptions = set()
dep_exeptions.add("libSystem.B.dylib")
dep_exeptions.add("libc++.1.dylib")

def rec_gather_all_dependencies(file, deps = set()):
    current_deps = get_dependencies(file)
    new_deps = current_deps.keys() - dep_exeptions - deps
    deps = deps | current_deps.keys()
    
    for d in new_deps:
        rec_gather_all_dependencies(file, deps)
    
    return deps

def print_deps(deps):
    print("Found the following dependencies:")
    for d in deps:
        print(d)

if __name__ == "__main__":
    args = parse_arguments()
    deps = rec_gather_all_dependencies(args.binary)
    print_deps(deps)
