import argparse
import subprocess
import os
from shutil import copyfile

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

def create_package(args):
    create_package_dirs(args.dir)
    copy_binary_to_package_dir(args.bin, args.binary_basename, args.dir)
    run_dylibbundler(args.bundler_binary, args.dir, args.binary_basename)
    pass

def parse_arguments():
    parser = argparse.ArgumentParser(description='Package the storm binary on Mac OS.')
    parser.add_argument('--bin', dest='bin', help='the binary to package', default='storm')
    parser.add_argument('--dir', dest='dir', help='the root directory of the package (will be created if it does not exist)', default='.')
    parser.add_argument('--dylibbundler', dest='bundler_binary', help='the binary of the dylibbundler', default='dylibbundler')
    args = parser.parse_args()
    args.binary_dir = os.path.split(args.bin)[0]
    args.binary_basename = os.path.split(args.bin)[1]
    return args

def create_package_dirs(root_dir):
    if not os.path.exists(root_dir):
        os.makedirs(root_dir)
    if not os.path.exists(root_dir + "/bin"):
        os.makedirs(root_dir + "/bin")
    if not os.path.exists(root_dir + "/lib"):
        os.makedirs(root_dir + "/bin")
    pass

def copy_binary_to_package_dir(binary, binary_basename, root_dir):
    copyfile(binary, root_dir + "/bin/storm")
    pass

def run_dylibbundler(bundler_binary, root_dir, binary_basename):
    command = [bundler_binary, "-cd", "-od", "-b", "-p", "@executable_path/../lib", "-x", root_dir + "/bin/" + binary_basename, "-d", root_dir + "/lib"]
    print("executing " + str(command))
    #proc = subprocess.Popen(command, stdin=subprocess.PIPE, stdout=subprocess.PIPE)
    pass

def fix_paths(root_dir, binary_basename):
    fix_paths_file(root_dir + "/bin/" + binary_basename)
    for file in os.listdir(root_dir + "/lib"):
        fix_paths_file(root_dir + "/lib/" + file)
        pass
    pass

def fix_paths_file(file):
    print("fixing paths for " + file)
    fixable_deps = get_fixable_deps(file)
    for (path, lib) in fixable_deps:
        change_fixable_dep(file, path, lib)

native_libs = ["libc++.1.dylib", "libSystem.B.dylib"]

def get_fixable_deps(file):
    # Call otool -L file to obtain the dependencies.
    proc = subprocess.Popen(["otool", "-L", file], stdin=subprocess.PIPE, stdout=subprocess.PIPE)
    result = []
    for line_bytes in proc.stdout:
        line = line_bytes.decode("utf-8").strip()
        lib = line.split()[0]
        if lib.startswith("@rpath/"):
            result.append(("@rpath", lib.split("/", 1)[1]))
        elif lib.startswith("/"):
            path_file = os.path.split(lib)
            if path_file[1] not in native_libs:
                result.append((path_file[0], path_file[1]))
    return result

def change_fixable_dep(file, path, lib):
    # Call install_name_tool to change the fixable dependencies
    command = ["install_name_tool", "-change", path + "/" + lib, "@executable_path/../lib/" + lib, file]
    print("executing " + str(command))
    proc = subprocess.Popen(command, stdout=subprocess.PIPE)
    #print ("after call to install_name_tool")
    #proc = subprocess.Popen(["otool", "-L", file], stdout=subprocess.PIPE)
    #for line_bytes in proc.stdout:
    #    line = line_bytes.decode("utf-8").strip()
    #    print(line)
    pass

if __name__ == "__main__":
    args = parse_arguments()

    #create_package(args)
    fix_paths(args.dir, args.binary_basename)

    
