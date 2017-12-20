import subprocess
import os
import shutil
import time
import math
import sys

logfileDir = "test_build_configurations_logs"
pathToConfigFile = ""
globalCmakeArguments = ""
globalMakeArguments = ""

if len(sys.argv) == 1 or len(sys.argv) > 5:
	print "Usage: " + sys.argv[0] + "/path/to/storm /path/to/configurations.txt \"optional make arguments\" \"optional cmake arguments\""
	print "Example: " + sys.argv[0] + " ~/storm test_build_configurations.txt \"-j 48 -k\" \"-DZ3_ROOT=/path/to/z3/\""
	sys.exit(0)

pathToStorm = sys.argv[1]
pathToConfigFile = sys.argv[2]

if len(sys.argv) > 3:
	globalMakeArguments = sys.argv[3]
	print globalMakeArguments
if len(sys.argv) > 4:
	globalCmakeArguments = sys.argv[4]
	print globalCmakeArguments

# create directory for log files
if not os.path.exists(logfileDir):
    os.makedirs(logfileDir)

unsuccessfulConfigs = ""
globalStartTime = time.time()

with open(pathToConfigFile) as configfile:
    localStartTime = time.time()
    configId=0
    for localCmakeArguments in configfile:
        
        cmakeArguments = globalCmakeArguments + " " + localCmakeArguments.strip('\n')
        buildDir = os.path.join(pathToStorm, "build{}".format(configId))
        logfilename = os.path.join(logfileDir, "build{}".format(configId) + "_" + time.strftime("%Y-%m-%d-%H-%M-%S") + ".log")
        
        
        print "Building configuration {} with cmake options ".format(configId) + cmakeArguments
        print "\tCreating log file " + logfilename + " ..."
        with open(logfilename, "w") as logfile:
            success=True
            logfile.write("Log for test configuration " + cmakeArguments + "\n")
                          
            print "\tCreating build directory" + buildDir + " ..."
            if os.path.exists(buildDir):
                print "\t\tRemoving existing directory " + buildDir
                shutil.rmtree(buildDir, ignore_errors=True)
            os.makedirs(buildDir)
            logfile.write("Build directory is " + buildDir + "\n")
            print "\t\tdone"
            
            if success:
                print "\tInvoking cmake ..."
                cmakeCommand = "cmake .. {}".format(cmakeArguments)
                logfile.write ("\n\n CALLING CMAKE \n\n" + cmakeCommand + "\n")
                try:
                    cmakeOutput = subprocess.check_output("cd " + buildDir + "; " + cmakeCommand , shell=True,stderr=subprocess.STDOUT)
                    print "\t\tdone"
                    logfile.write(cmakeOutput)
                except subprocess.CalledProcessError as e:
                    success=False
                    print "\t\tfail"
                    print e.output
                    logfile.write(e.output)
                                
            if success:
                print "\tInvoking make ..."
                makeCommand = "make {}".format(globalMakeArguments)
                logfile.write ("\n\n CALLING MAKE \n\n" + makeCommand + "\n")
                try:
                    makeOutput = subprocess.check_output("cd " + buildDir + "; " + makeCommand , shell=True,stderr=subprocess.STDOUT)
                    print "\t\tdone"
                    logfile.write(makeOutput)
                except subprocess.CalledProcessError as e:
                    success=False
                    print "\t\tfail"
                    print e.output
                    logfile.write(e.output)
            
            if success:
                print "\tInvoking make check..."
                makeCheckCommand = "make check"
                logfile.write ("\n\n CALLING MAKE CHECK \n\n" + makeCheckCommand + "\n")
                try:
                    makeCheckOutput = subprocess.check_output("cd " + buildDir + "; " + makeCheckCommand , shell=True,stderr=subprocess.STDOUT)
                    print "\t\tdone"
                    logfile.write(makeCheckOutput)
                except subprocess.CalledProcessError as e:
                    success=False
                    print "\t\tfail"
                    print e.output
                    logfile.write(e.output)
                    
            localEndTime = time.time()
            if success:
                print "\tConfiguration build and tested successfully within {} minutes.".format(int((localEndTime - localStartTime)/60))
            else:
                print "\tAn error occurred for this configuration."
                unsuccessfulConfigs += buildDir + " with arguments " + cmakeArguments + "\n"

            configId += 1
globalEndTime = time.time()
print "All tests completed after {} minutes.".format(int((globalEndTime - globalStartTime)/60))
if unsuccessfulConfigs == "":
    print "All configurations were build and tested successfully."
else:
    print "The following configurations failed: \n" + unsuccessfulConfigs
    
                


