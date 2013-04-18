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


function get_library_directory(){
    gcc_version_full=`gcc --version | grep "gcc"| egrep -o " [0-9]+\.[0-9]+\.[0-9]+.*" | sed -e s/^\ //`
    if [ $? -eq 0 ]; then
        gcc_version=`echo "$gcc_version_full" | egrep -o "^[0-9]+\.[0-9]+\.[0-9]+"`
    fi
    case "${gcc_version}" in
	4.[4-9]* )
	    lib_dir="gcc4.4";;
	* )
	    lib_dir="gcc4.1";;
    esac
    echo $lib_dir
}

export TBBROOT=SUBSTITUTE_INSTALL_DIR_HERE

if [[ "$1" != "ia32" && "$1" != "intel64" && "$1" != "android" ]]; then
   echo "ERROR: Unknown switch '$1'. Accepted values: ia32, intel64, android"
   return 1;
fi

if [ "$1" != "android" ]; then
    library_directory=$(get_library_directory)
else
    library_directory=""
fi

if [ -z "${MIC_LD_LIBRARY_PATH}" ] ; then
   MIC_LD_LIBRARY_PATH="$TBBROOT/lib/mic"; export MIC_LD_LIBRARY_PATH
else
   MIC_LD_LIBRARY_PATH="$TBBROOT/lib/mic:${MIC_LD_LIBRARY_PATH}"; export MIC_LD_LIBRARY_PATH
fi

if [ -z "${LD_LIBRARY_PATH}" ] ; then
   LD_LIBRARY_PATH="$TBBROOT/lib/$1/$library_directory"; export LD_LIBRARY_PATH
else
   LD_LIBRARY_PATH="$TBBROOT/lib/$1/$library_directory:${LD_LIBRARY_PATH}"; export LD_LIBRARY_PATH
fi

if [ -z "${LIBRARY_PATH}" ] ; then
   LIBRARY_PATH="$TBBROOT/lib/$1/$library_directory"; export LIBRARY_PATH
else
   LIBRARY_PATH="$TBBROOT/lib/$1/$library_directory:${LIBRARY_PATH}"; export LIBRARY_PATH
fi

if [ -z "${CPATH}" ] ; then
    CPATH="${TBBROOT}/include"; export CPATH
else
    CPATH="${TBBROOT}/include:$CPATH"; export CPATH
fi

