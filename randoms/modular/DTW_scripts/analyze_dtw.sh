#!/usr/bin/env bash

lutPath="Modular_rotated.lut"

dataPath="merged_timeCorrected_dtw.Cdf"

totaltime=544.

root -l ./create_dtw_projections.cpp\(\"$lutPath\",\"$dataPath\",$totaltime\) -q
