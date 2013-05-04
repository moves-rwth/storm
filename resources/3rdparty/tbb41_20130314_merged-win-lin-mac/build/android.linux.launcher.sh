#!/bin/sh
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
# android.linux.launcher.sh [-v] [-u] [-l <library>] <executable> <arg1> <arg2> <argN>
#         where: -l <library> specfies the library name to be assigned to LD_PRELOAD
#         where: -v enables verbose output when running the test (where supported)
#         where: -u is ignored on Android
#
# Libs and executable necessary for testing should be present in the current directory before running.
# ANDROID_SERIAL must be set to the connected Android target device name for file transfer and test runs.
# ANDROID_TEST_DIRECTORY may be set to the directory used for testing on the Android target device; otherwise, 
#                        the default directory used is "/data/local/tmp/$(basename $PWD)".
# Note: Do not remove the redirections to '/dev/null' in the script, otherwise the nightly test system will fail.

do_cleanup()
{
    adb pull $targetdir/events.txt events.txt > /dev/null 2>&1
    # Remove target directory on the device
    adb shell "rm -r ${targetdir}; mkdir -p ${targetdir}" > /dev/null 2>&1
}

do_trap_cleanup()
{
    do_cleanup
    exit -1
}

# Process the optional arguments if present
if [ "x$1" = "x-v" ]; then {
    verb="$1"
    shift 1
}; fi

if [ "x$1" = "x-u" ]; then {
    shift 1
}; fi

if [ "x$1" = "x-l" ]; then {
    ldpreload="$2"
    shift 2
}; fi

# Collect the executable name
exename=$(basename $1)
shift

# Prepare the target directory on the device
currentdir=$(basename $PWD)
targetdir=${ANDROID_TEST_DIRECTORY:-/data/local/tmp/$currentdir}
do_cleanup
trap do_trap_cleanup INT  # if someone hits control-c, cleanup the device

# Collect the list of files to transfer to the target device, starting with executable itself.
fnamelist="$exename"

# Add the C++ standard library from the NDK, which is required for all tests on Android.
if [ ! -z "${LIB_GNU_STL_ANDROID}" ]; then
	fnamelist="$fnamelist ${LIB_GNU_STL_ANDROID}/libgnustl_shared.so"
else
	fnamelist="$fnamelist libgnustl_shared.so"
fi

# Find the TBB libraries and add them to the list.
# Add TBB libraries from the current directory that contains libtbb* files

files="$(/bin/ls libtbb* 2> /dev/null)"
if [ ! -z "$files" ]; then fnamelist="$fnamelist $files"; fi

mallocfiles="$(/bin/ls libtbbmalloc* 2> /dev/null)"
if [ ! -z "$mallocfiles" ]; then {
    #TODO: any better workaround instead calling echo 
    #(without echo there is error: /system/bin/sh: libtbbmalloc_proxy.so: not found)
    ldpreload="$ldpreload $(echo $mallocfiles)"
}; fi

if [ ! -z "$ldpreload" ]; then ldpreload="export LD_PRELOAD=$ldpreload;"; fi

# Add any libraries built for specific tests.
exeroot=${exename%\.*}
files="$(/bin/ls ${exeroot}*.so ${exeroot}*.so.* 2> /dev/null)"
if [ ! -z "$files" ]; then {
    fnamelist="$fnamelist $files" 
}; fi

# TODO: Add extra libraries from the Intel(R) Compiler for certain tests
# found=$(echo $exename | egrep 'test_malloc_atexit\|test_malloc_lib_unload' 2> /dev/null)
# if [ ! -z $found ] ; then
#     fnamelist="$fnamelist ${compiler_path_lib}/libimf.so  \
#                           ${compiler_path_lib}/libsvml.so \
#                           ${compiler_path_lib}/libintlc.so.5"
# fi

# Transfer collected executable and library files to the target device.
transfers_ok=1
for fullname in $fnamelist; do {
    if [ -r $fullname ]; then {
        # Transfer the executable and libraries to top-level target directory
        adb push $fullname ${targetdir}/$(basename $fullname) > /dev/null 2>&1
    }; else {
        echo "Error: required file ${currentdir}/${fullname} for test $exename not available for transfer."
        transfers_ok=0
    }; fi
}; done

if [ "${transfers_ok}" = "0" ]; then {
    do_cleanup
    exit -1
}; fi

# Transfer input files used by example codes by scanning the executable argument list.
for fullname in "$@"; do {
    if [ -r $fullname ]; then {
        directory=$(dirname $fullname)
        filename=$(basename $fullname)
        # strip leading "." from fullname if present
        if [ "$directory" = "\." ]; then {
            directory=""
            fullname=$filename
        }; fi
        # Create the target directory to hold input file if necessary
        if [ ! -z $directory ]; then {
            adb shell "mkdir $directory" > /dev/null 2>&1
        }; fi
        # Transfer the input file to corresponding directory on target device
        adb push $fullname ${targetdir}/$fullname > /dev/null 2>&1
    }; fi
}; done

# The return_code file is the best way found to return the status of the test execution when using adb shell.
(adb shell "cd $targetdir; $ldpreload export LD_LIBRARY_PATH=.; ./$exename $verb $*; echo \$? > return_code") | sed -e "s/\\r$//"

# Capture the return code string and remove the trailing \r from the return_code file contents
exitcode=`(adb shell "cat $targetdir/return_code 2> /dev/null") | sed -e "s/\\r$//"`

do_cleanup

# Return the exit code of the test.
exit $exitcode
