#include <cstdlib>
#include <vector>
#include <iostream>
#include <map>
#include <string>

#include "TFile.h"
#include "TTree.h"
#include "TCanvas.h"
#include "TFrame.h"
#include "TH1F.h"
#include "TBenchmark.h"
#include "TRandom.h"
#include "TSystem.h"
#include "TString.h"

#include "TVector3.h"

using namespace std;

int merge_header(string, int);

void prepare_castor_files_dtw(string listName, int total_time)
{
    long int total = merge_header(listName, total_time);
    
    std::ifstream Cdf_file("merged_dtw.Cdf",std::ios::binary);
    std::ofstream new_Cdf_file("merged_timeCorrected_dtw.Cdf",std::ios::binary);
    
    unsigned int var1, var2, t;
    long int count = 0;
    
    while(true)
    {
        Cdf_file.read((char*)&t, sizeof(unsigned int));
        Cdf_file.read((char*)&var1, sizeof(unsigned int));
        Cdf_file.read((char*)&var2, sizeof(unsigned int));
        
        if(!Cdf_file)
        {
            break;
        }
        count++;
        unsigned int t_new = (unsigned int)( (float)count/(float)total * (float)total_time*1000. );
        new_Cdf_file.write((char*)&t_new, sizeof(unsigned int));
        new_Cdf_file.write((char*)&var1, sizeof(unsigned int));
        new_Cdf_file.write((char*)&var2, sizeof(unsigned int));
    }
    
    new_Cdf_file.close();
    Cdf_file.close();
    cout << total << "\t" << count << endl;
    if(total == count)
    {
        cout << "New datafile created" << endl;
    }
    else
    {
        cout << "ERROR: Number of events in merged datafile did not match sum of events before merging" << endl;
        cout << "ERROR: New datafile created with wrong amount of events" << endl;
    }
}

int merge_header(string listName, int total_time)
{
    std::ifstream list_file(listName);
    string fileName;
    long int total = 0;
    while(getline(list_file, fileName))
    {
        std::ifstream Cdh_file(fileName);
        string dummy;
        getline(Cdh_file, dummy);
        getline(Cdh_file, dummy);
        getline(Cdh_file, dummy, ' ');
        getline(Cdh_file, dummy, ' ');
        getline(Cdh_file, dummy, ' ');
        getline(Cdh_file, dummy);
        stringstream val;
        val << dummy;
        long int events;
        val >> events;
        total += events;
        Cdh_file.close();
    }
    
    list_file.clear();
    list_file.seekg(0, list_file.beg);
    
    getline(list_file, fileName);
    string line;
    
    std::ifstream Cdh_file(fileName);
    std::ofstream file("merged_timeCorrected_dtw.Cdh");
    
    getline(Cdh_file, line);
    file << line << endl;
    
    getline(Cdh_file, line);
    file << "Data filename: merged_timeCorrected_dtw.Cdf" << endl;
    
    getline(Cdh_file, line);
    file << "Number of events: " << total << endl;
    
    for(int i = 0; i < 3; i++)
    {
        getline(Cdh_file, line);
        file << line << endl;
    }
    
    getline(Cdh_file, line);
    file << "Duration (s): " << total_time << endl;
    
    Cdh_file.close();
    file.close();
    list_file.close();
    
    return total;
}
