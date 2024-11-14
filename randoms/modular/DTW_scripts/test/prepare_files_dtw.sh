#!/usr/bin/env bash

echo "Preparing list of header files..."
ls -v ../exampleData/DTW/dabc*Cdh > header_list_dtw.txt
echo "List of header files created: header_list_dtw.txt"

echo "Merging datafiles..."
cat ../exampleData/DTW/dabc*Cdf > merged_dtw.Cdf
echo "Datafiles merged to: merged_dtw.Cdf"

totaltime=8

root -l ./prepare_castor_files_dtw.cpp\(\"./header_list_dtw.txt\",$totaltime\) -q

echo "Removing temporary files..."
rm header_list_dtw.txt
rm merged_dtw.Cdf
echo "List of header files and original merged datafile removed"
