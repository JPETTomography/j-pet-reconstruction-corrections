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

std::vector<string> read_header(string);
bool check_header_RF(std::vector<string>);
bool check_and_create_Cdh(string, string);
void correct_on_Cdf_with_no_corrections(string, string, TString);

void correct_on_Cdf_with_no_corrections(string file_name, string new_file_name, TString matrix_name)
{
    if(!check_and_create_Cdh(file_name, new_file_name))
    {
        return;
    }
    
    TFile *random_factors = new TFile(matrix_name);
    
    string cdf_name = file_name + ".Cdf";
    string new_cdf_name = new_file_name + ".Cdf";
    
    std::ifstream Cdf_file(cdf_name,std::ios::binary);
    std::ofstream new_Cdf_file(new_cdf_name,std::ios::binary);
    unsigned int var1, var2, t;
    float tof;
    
    while(true)
    {
        Cdf_file.read((char*)&t, sizeof(unsigned int));
        Cdf_file.read((char*)&tof, sizeof(float));
        Cdf_file.read((char*)&var1, sizeof(unsigned int));
        Cdf_file.read((char*)&var2, sizeof(unsigned int));
        
        if(!Cdf_file)
        {
            break;
        }
        
        int axial_1 = (int)var1/(312*8);
        int axial_2 = (int)var2/(312*8);
        TString matrixName;
        matrixName.Form("smooth_%i_%i",axial_1,axial_2);
        TH2D *matrix = (TH2D*)random_factors->Get(matrixName);
        
        int scint_1 = (int)var1%312;
        int scint_2 = (int)var2%312;
        float rf = (float)( matrix->GetBinContent(scint_1+1, scint_2+1)/64. );
        
            new_Cdf_file.write((char*)&t, sizeof(unsigned int));
            new_Cdf_file.write((char*)&rf, sizeof(float));
            new_Cdf_file.write((char*)&tof, sizeof(float));
            new_Cdf_file.write((char*)&var1, sizeof(unsigned int));
            new_Cdf_file.write((char*)&var2, sizeof(unsigned int));
    }
    
    Cdf_file.close();
    new_Cdf_file.close();
    cout << "New datafile created" << endl;
}

bool check_and_create_Cdh(string file_name, string new_file_name)
{
    string header_name = file_name + ".Cdh";
    string new_header_name = new_file_name + ".Cdh";
    
    std::vector<string> header = read_header(header_name);
    
    if(check_header_RF(header))
    {
        cout << "Random correction factors already included" << endl;
        return false;
    }
    else
    {
        std::ofstream file(new_header_name);
	
        file << header[0] << endl;
        file << "Data filename: " << new_file_name << ".Cdf" << endl;
        for(int i = 2; i < header.size(); i++)
        {
            file << header[i] << endl;
        }
        file << "Random correction flag: 1";
        file.close();
        cout << "New header created" << endl;
        return true;
    }
}

std::vector<string> read_header(string header_name)
{
    std::ifstream file(header_name);
    string line;
    std::vector<string> header;
    
    while(getline(file, line))
    {
        header.push_back(line);
    }
    
    file.close();
    return header;
}

bool check_header_RF(std::vector<string> header)
{
    bool flag = false;
    string random("Random correction flag: 1");
    
    for(int i = 0; i < header.size(); i++)
    {
        if(!header[i].compare(random))
        {
            flag = true;
        }
    }
    return flag;
}
