#!/usr/bin/env bash

properDtwFile="../exampleData/proper_output_files/merged_timeCorrected_dtw.Cdf"
properDtwHeader="../exampleData/proper_output_files/merged_timeCorrected_dtw.Cdh"
properStwFile="../exampleData/proper_output_files/merged_timeCorrected.Cdf"
properStwHeader="../exampleData/proper_output_files/merged_timeCorrected.Cdh"
properCorrectedFile="../exampleData/proper_output_files/random_corrected_data.Cdf"
properCorrectedHeader="../exampleData/proper_output_files/random_corrected_data.Cdh"

./prepare_files_dtw.sh

dtwFile="merged_timeCorrected_dtw.Cdf"
dtwHeader="merged_timeCorrected_dtw.Cdh"

if diff $dtwHeader $properDtwHeader > /dev/null 2>&1
  then
    echo "Merger of DTW headers done correctly"
  else
    echo "Error in merger of DTW headers"
    exit 1
fi

if diff $dtwFile $properDtwFile > /dev/null 2>&1
  then
    echo "Merger of DTW datafiles done correctly"
  else
    echo "Error in merger of DTW datafiles"
    exit 1
fi

./prepare_files.sh

stwFile="merged_timeCorrected.Cdf"
stwHeader="merged_timeCorrected.Cdh"

if diff $stwHeader $properStwHeader > /dev/null 2>&1
  then
    echo "Merger of STW headers done correctly"
  else
    echo "Error in merger of STW headers"
    exit 1
fi

if diff $stwFile $properStwFile > /dev/null 2>&1
  then
    echo "Merger of STW datafiles done correctly"
  else
    echo "Error in merger of STW datafiles"
    exit 1
fi

./analyze_dtw.sh
./apply_random_correction.sh

correctedFile="random_corrected_data.Cdf"
correctedHeader="random_corrected_data.Cdh"

if diff $correctedHeader $properCorrectedHeader > /dev/null 2>&1
  then
    echo "Correcting header for random factors done correctly"
  else
    echo "Error in header corrected for random factors"
    exit 1
fi

if diff $correctedFile $properCorrectedFile > /dev/null 2>&1
  then
    echo "Correcting datafile for random factors done correctly"
  else
    echo "Error in datafile corrected for random factors"
    exit 1
fi

echo "Pipeline test to create and add random coincidence correction factors passed successfully"
