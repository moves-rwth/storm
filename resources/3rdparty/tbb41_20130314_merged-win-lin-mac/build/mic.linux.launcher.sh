#!/bin/bash
#
# Copyright 2005-2013 Intel Corporation.  All Rights Reserved.
#
# This file is part of Threading Building Blocks.
#
# Threading Building Blocks is free software; you can redistribute it
# and/or modify it under the terms of the GNU General Public License
# version 2 as published by the Free Software Foundation.
#
# Threading Building Blocks is distributed in the hope that it will be
# useful, but WITHOUT ANY WARRANTY; without even the implied warranty
# of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with Threading Building Blocks; if not, write to the Free Software
# Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
#
# As a special exception, you may use this file as part of a free software
# library without restriction.  Specifically, if other files instantiate
# templates or use macros or inline functions from this file, or you compile
# this file and link it with other files to produce an executable, this
# file does not by itself cause the resulting executable to be covered by
# the GNU General Public License.  This exception does not however
# invalidate any other reasons why the executable file might be covered by
# the GNU General Public License.

# Usage:
# mic.linux.launcher.sh [-v] [-s] [-r <repeats>] [-u] [-l <library>] <executable> <arg1> <arg2> <argN>
#         where: -v enables verbose output
#         where: -s enables stress testing unless ctrl-c is pressed
#         where: -r <repeats> specifies number of times to repeat execution
#         where: -u is ignored
#         where: -l <library> specifies the library name to be assigned to LD_PRELOAD
#
# Libs and executable necessary for testing should be present in the current directory before running.
# Note: Do not remove the redirections to '/dev/null' in the script, otherwise the nightly test system will fail.
#
trap 'echo Error at line $LINENO while executing "$BASH_COMMAND"' ERR #
trap 'echo -e "\n*** Interrupted ***" && exit 1' SIGINT SIGQUIT #
# Process the optional arguments if present
if [ "x$1" = "x-v" ]; then shift 1; else SUPPRESS='>/dev/null'; fi #
if [ "x$1" = "x-s" ]; then shift 1; echo Doing stress testing. Press Ctrl-C to terminate
    run_prefix+='rep() { while :; do $*; done; }; rep '; fi #
if [ "x$1" = "x-r" ]; then #
    run_prefix+="rep() { for i in \$(seq 1 $2); do echo \$i of $2:; \$*; done; }; rep " #
    shift 2; fi #
[ "x$1" = "x-u" ] && shift 1 #
if [ "x$1" = "x-l" ]; then { #
    ldd_list+="$2 "#
    run_prefix+=" LD_PRELOAD=$2" #
    shift 2 #
}; fi #
# Collect the executable name
fexename="$1" #
exename=`basename $1` #
shift #
#
RSH="sudo ssh mic0" #
RCP="sudo scp" #
#
# Prepare the target directory on the device
currentdir=`basename $PWD` #
targetdir=${TEST_DIRECTORY:-/mic0fs/$USER/$currentdir} #
#
# Remove leftover target directory on the device
eval "$RSH \"rm -r $targetdir; mkdir -p $targetdir\" $SUPPRESS 2>&1 || exit \$?" #
eval "$RCP $fexename mic0:$targetdir/ $SUPPRESS || exit \$?" #
#
# Collect the list of files to transfer to the target device, starting with executable itself.
ldd_list+="libtbbmalloc*.so* `$RSH ldd $targetdir/$exename | grep = | cut -d= -f1 2>/dev/null`" #
fnamelist="" #
#
# Find the libraries and add them to the list.
# For example, go through MIC_LD_LIBRARY_PATH and add TBB libraries from the first 
# directory that contains tbb files
mic_dir_list=`echo .:$MIC_LD_LIBRARY_PATH | tr : " "` #
for name in $ldd_list; do # adds the first matched name in specified dirs
    fnamelist+="`find $mic_dir_list -name $name -a -readable -print -quit 2>/dev/null` " #
done #
#
# Add any libraries built for specific tests.
exeroot=${exename%\.*} #
fnamelist+=`ls ${exeroot}*.so ${exeroot}*.so.* 2>/dev/null`||: #
#
# Transfer collected executable and library files to the target device.
eval "$RCP $fnamelist mic0:$targetdir/ $SUPPRESS || exit \$?" #
#
# Transfer input files used by example codes by scanning the executable argument list.
for fullname in "$@"; do if [ -r $fullname ]; then { #
    directory=$(dirname $fullname) #
    filename=$(basename $fullname) #
    # strip leading "." from fullname if present
    [ "$directory" = "." ] && directory="" && fullname="$filename" #
    # Create the target directory to hold input file if necessary
    [ ! -z "$directory" ] && $RSH "mkdir -p $targetdir/$directory" $SUPPRESS 2>&1 #
    # Transfer the input file to corresponding directory on target device
    eval "$RCP $fullname mic0:$targetdir/$fullname $SUPPRESS 2>&1 || exit \$?" #
}; fi; done #
#
args=$* #
# Run the test on the target device
kill_interrupt() { #
echo -e "\n*** Killing remote $exename ***" && $RSH "killall $exename" #
} # kill target process
trap 'kill_interrupt' SIGINT SIGQUIT # trap keyboard interrupt (control-c)
trap - ERR #
$RSH "cd $targetdir; export LD_LIBRARY_PATH=.:\$LD_LIBRARY_PATH; $run_prefix ./$exename $args" #
# Return the exit code of the test.
exit $? #
