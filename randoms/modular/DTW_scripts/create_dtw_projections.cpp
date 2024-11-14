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

std::pair<float, float> getSinogramVariables(float, float, float, float);

void create_dtw_projections(TString lutPath, TString inputPath, double totalTime)
{
    std::ifstream lut_file(lutPath,std::ios::binary);
    float x, y, z, dummy;
    std::vector<float> lut[3];
    
    while(true)
    {
        lut_file.read((char*)&x, sizeof(float));
        lut_file.read((char*)&y, sizeof(float));
        lut_file.read((char*)&z, sizeof(float));
        lut_file.read((char*)&dummy, sizeof(float));
        lut_file.read((char*)&dummy, sizeof(float));
        lut_file.read((char*)&dummy, sizeof(float));
        
        if(!lut_file)
        {
            break;
        }
        
        lut[0].push_back(x);
        lut[1].push_back(y);
        lut[2].push_back(z);
    }
    
    lut_file.close();
    
    TH2D *all[25][25];
    TH2D *smear[25][25];
    for(int i = 0; i < 25; i++)
    {
        for(int j = 0; j < 25; j++)
        {
            TString name;
            name.Form("original_%i_%i",i,j);
            all[i][j] = new TH2D(name,";;;",312,0,312,312,0,312);
            TString name2;
            name2.Form("smooth_%i_%i",i,j);
            smear[i][j] = new TH2D(name2,";;;",312,0,312,312,0,312);
        }
    }
    
    std::ifstream Cdf_file(inputPath,std::ios::binary);
    unsigned int var1, var2, t;
    
    while(true)
    {
        Cdf_file.read((char*)&t, sizeof(unsigned int));
        Cdf_file.read((char*)&var1, sizeof(unsigned int));
        Cdf_file.read((char*)&var2, sizeof(unsigned int));
        
        if(!Cdf_file)
        {
            break;
        }
        
        float x1 = lut[0].at(var1);
        float y1 = lut[1].at(var1);
        float z1 = lut[2].at(var1);
        
        float x2 = lut[0].at(var2);
        float y2 = lut[1].at(var2);
        float z2 = lut[2].at(var2);
        
        double r, theta;
        std::tie(r, theta) = getSinogramVariables(x1, y1, x2, y2);
        
        int scint_1 = (int)var1%312;
        int axial_1 = (int)var1/(312*8);
        int block_1 = scint_1/4;
        int id_1 = axial_1 + scint_1*25;
        int id2_1 = axial_1 + block_1*25;
        int scint_2 = (int)var2%312;
        int axial_2 = (int)var2/(312*8);
        int block_2 = scint_2/4;
        int id_2 = axial_2 + scint_2*25;
        int id2_2 = axial_2 + block_2*25;
        
        double cps_normalization = 1./totalTime;
        
        if(axial_1 != axial_2)
        {
            all[axial_1][axial_2]->Fill(scint_1+0.5, scint_2+0.5, cps_normalization);
            all[axial_1][axial_2]->Fill(scint_2+0.5, scint_1+0.5, cps_normalization);
            all[axial_2][axial_1]->Fill(scint_2+0.5, scint_1+0.5, cps_normalization);
            all[axial_2][axial_1]->Fill(scint_1+0.5, scint_2+0.5, cps_normalization);
        }
        else
        {
            all[axial_1][axial_2]->Fill(scint_1+0.5, scint_2+0.5, cps_normalization);
            all[axial_2][axial_1]->Fill(scint_2+0.5, scint_1+0.5, cps_normalization);
        }
    }
    
    Cdf_file.close();
    
    for(int i = 0; i < 25; i++)
    {
        for(int j = 0; j < 25; j++)
        {
            double Ratio[311] = {};
            
            for(int s1 = 0; s1 < 311; s1++)
            {
                double R1 = 0.;
                double R2 = 0.;
                int s2 = s1+1;
                
                for(int fan1 = s1+45+1; fan1 < s1+45+223; fan1++)
                {
                    R1 += all[i][j]->GetBinContent(s1+1, fan1%312+1);
                }
                
                for(int fan2 = s2+45; fan2 < s2+45+223-1; fan2++)
                {
                    R2 += all[i][j]->GetBinContent(s2+1, fan2%312+1);
                }

                if(R1 == 0 || R2 == 0)
                {
                    Ratio[s1] = 1.;
                }
                else
                {
                    Ratio[s1] = R2/R1;
                }             
   
            }
                
            double singles_rate[312] = {};
            singles_rate[0] = 1.;
            for(int iter = 1; iter < 312; iter++)
            {
                singles_rate[iter] = singles_rate[iter-1]*Ratio[iter-1];
            }
            
            double nominator = 0.;
            double denominator = 0.;
            double window = 3.0 * pow(10.,6.) * pow(10.,-15.);
            
            for(int s = 0; s < 312; s++)
            {
                for(int fan = s+45; fan < s+45+223; fan++)
                {
                    int fan_id = fan%312;
                    
                    nominator += all[i][j]->GetBinContent(s+1, fan_id+1);
                    denominator += 2.*window*singles_rate[s]*singles_rate[fan_id];
                }
            }
            
            double alfa2 = nominator/denominator;
            
            for(int s = 0; s < 312; s++)
            {
                for(int fan = s+45; fan < s+45+223; fan++)
                {
                    int fan_id = fan%312;
                    double randoms = 2.*window*alfa2*singles_rate[s]*singles_rate[fan_id];
                    smear[i][j]->SetBinContent(s+1, fan_id+1, randoms);
                }
            }
        }
    }
    
    TString outName;
    outName.Form("dtwProjections.root");
    TFile rootFile(outName,"RECREATE");
    for(int i = 0; i < 25; i++)
    {
        for(int j = 0; j < 25; j++)
        {
            all[i][j]->Write();
            smear[i][j]->Write();
        }
    }
    rootFile.Close();
}

std::pair<float, float> getSinogramVariables(float xa, float ya, float xb, float yb)
{
    float r, theta;
    if(xa == xb)
    {
        r = xa;
        theta = 0;
    }
    else if(ya == yb)
    {
        r = ya;
        theta = TMath::Pi()/2.;
    }
    else
    {
        float a = (ya-yb)/(xa-xb);
        float b = ya - a*xa;
        
        float A = a;
        float B = -1;
        float C = b;
        
        r = C/sqrt(A*A + B*B);
        
        float a2 = -1./a;
        if(a2 > 0)
        {
            theta = atan(a2);
        }
        else
        {
            theta = TMath::Pi() + atan(a2);
        }
    }
    return std::make_pair(r, theta);
}
