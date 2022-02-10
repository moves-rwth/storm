#!/usr/bin/env bash

# Settings
auto_format_file_extensions=( .h .cpp )
auto_format_src_dir=./src

# Set-up directories
script_dir=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
storm_root="$script_dir/../.."

# Sanity checks
for expected_file in .clang-format .clang-format-ignore
do
	if [ ! -f $storm_root/$expected_file ]; then
    	echo "ERROR: There does not seem to be a file '$storm_root/$expected_file'. Have you moved this script?"
		exit 1
	fi
done
if ! command -v clang-format &> /dev/null
then
    echo "Unable to find clang-format executable. Is it installed?"
    exit 1
fi

# go to correct directory
cd $storm_root

# Build an expression for the find command
# 1. look in the correct directory
auto_format_find_expression="$auto_format_src_dir ( ("
# 2. the file should have one of the specified file extensions
for extension in "${auto_format_file_extensions[@]}"
do
	auto_format_find_expression+=" -name *$extension -or"
done
# 3. the file path should not match one of the excluding patterns in .clang-format-ignore
auto_format_find_expression+=" -false ) -and -not ("
while read exclude_pattern || [[ -n "$exclude_pattern" ]]
do
	if [[ -z "$exclude_pattern" || "$exclude_pattern" == \#* ]]
	then 
		continue #ignore white spaces and lines starting with #
	fi 
	auto_format_find_expression+=" -path $exclude_pattern -or"
done < .clang-format-ignore
auto_format_find_expression+=" -false ) ) -print"

# disable bash expansion of *
set -f 

# find files and invoke clang-format with in-place option
find $auto_format_find_expression | xargs clang-format -i -style=file
