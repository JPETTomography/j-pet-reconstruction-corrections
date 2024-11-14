#!/usr/bin/env bash

lutPath="../Modular_rotated.lut"

dataPath="merged_timeCorrected_dtw.Cdf"

totaltime=8.

root -l ./create_dtw_projections.cpp\(\"$lutPath\",\"$dataPath\",$totaltime\) -q
