#!/usr/bin/env bash

echo "Preparing list of header files..."
ls -v ../exampleData/STW/dabc*Cdh > header_list.txt
echo "List of header files created: header_list.txt"

echo "Merging datafiles..."
cat ../exampleData/STW/dabc*Cdf > merged.Cdf
echo "Datafiles merged to: merged.Cdf"

totaltime=8

root -l ./prepare_castor_files.cpp\(\"./header_list.txt\",$totaltime\) -q

echo "Removing temporary files..."
rm header_list.txt
rm merged.Cdf
echo "List of header files and original merged datafile removed"
