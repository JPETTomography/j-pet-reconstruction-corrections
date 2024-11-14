#!/usr/bin/env bash

dataPath="merged_timeCorrected" #without .Cdf

outputPath="random_corrected_data" #without type

projectionPath="dtwProjections.root"

root -l ./correct_on_Cdf_with_no_corrections.cpp\(\"$dataPath\",\"$outputPath\",\"$projectionPath\"\) -q
