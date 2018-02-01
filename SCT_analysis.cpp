// SCT_analysis.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <fstream>
//include "C:/root_v5.34.36/include/TH1.h"
#include "C:/root_v5.34.36/include/TFile.h"
#include <string>
//#include "C:/root_v5.34.36/include/TTree.h"
#include "C:/root_v5.34.36/include/TH1F.h"
#include <vector>
#include <iostream>
#include <sstream>

void scan_dat_file(fstream* inf, TFile* f, int boardnumber, std::string time_code)
{
	std::string readout;
	int wc = 0, line = 0;
	std::vector <std::string> columns;
	std::vector <TH1F*> hists;
	std::vector <std::vector <float> > vals;
	//vals.resize(11);
	//std::vector <float> vd, t, id, va, ia, vdraw, vdreg, varaw, vareg, mgnd, T;
	bool esccondition = inf->is_open();
	while (esccondition) {
		while (std::getline(*inf, readout))
		{
			if (readout == time_code) {
				wc = 1;
			}
			if (readout == "") wc = 0;
			if (wc = 1 && (readout != time_code && readout != "#** t(s),v0Mon(V),i0Mon(mA),v1Mon(V),i1Mon(mA),VDDA_RAW(mV),VDDA_REG(mV),VDDD_RAW(mV),VDDD_REG(mV),MOD_GND(mV),Temperature(C),IrradStatus(On = 1),DoseRate(kRad/hr),TotalDose(MRad) **" && readout != "#** -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- **"))
			{
				line++;
				//std::cout << line << std::endl;
				//for (int i = 0; i < 11; i++) vals.at(i).resize(line);
				//std::cout << vals.size() << std::endl;
				//std::string sline = line.itos();
				columns.clear();
				std::istringstream temp(readout);
				std::string temp2;
				while (std::getline(temp, temp2, ' ')) columns.push_back(temp2);
				std::vector <float>lvec;
				for (int i = 0; i < columns.size(); i++) {
					//std::cout << columns.at(i) << std::endl;
					try {
							lvec.push_back(std::stof(columns.at(i).c_str()));
							//std::cout << std::stof(columns.at(i).c_str()) << std::endl;
						}
					catch (const std::exception& e) {
						continue;
					}
					 //std::cout << "error on line " <<line <<std::endl; }
				}
				vals.push_back(lvec);
				std::cout << vals.size() << std::endl;
				
			}
			else continue;
			if (line == 1835)esccondition=false;
		
		}
	}
	std::cout << "having Escaped the carven of the terrible for loop, our hero travels forth" << std::endl;
	hists.push_back(new TH1F("vd", "Voltage in Digital Channel; Time (s); Voltage (V)", vals.size(), 0, vals.size()));
	std::cout << vals.at(0).size();
	try {
		for (int i = 0; i < vals.size(); i++) {
			vals.at(0).at(i) = vals.at(i).at(0) - vals.at(0).at(0);
			//std::cout << "\n how about now?";
			hists.at(0)->SetBinContent(vals.at(0).at(i), vals.at(1).at(i));
			//std::cout << vals.at(0).at(i) << std::endl;
		}
	}
	catch (std::exception& e) { std::cout << e.what() << std::endl; }
	hists.push_back(new TH1F("id", "Current in Digital Channel; Time (s); Current (mA)", vals.at(0).size(), 0, vals.at(0).size()));
	for (int i = 0; i < vals.size()-1; i++) hists.at(1)->SetBinContent(vals.at(0).at(i), 1000 * vals.at(2).at(i));
	hists.push_back(new TH1F("pd", "Power in Digital Channel; Time (s); Power(mW)", vals.at(0).size(), 0, vals.at(0).size()));
	for (int i = 0; i < vals.size(); i++) {
		//vals.at(i).at(0) = vals.at(i).at(0) - vals.at(0).at(0);
		hists.at(2)->SetBinContent(vals.at(0).at(i), vals.at(1).at(i)*vals.at(2).at(i) * 1000);
	}
	for (int i = 0; i < hists.size(); i++) hists.at(i)->Write();
	std::cout << "Done" << std::endl;
	f->Write();
	f->Close();
}


	

	int main()
{
	fstream log_file;
	log_file.open("C:/Users/Silas Grossberndt/Documents/ABC_Boards/TIDLogTesting.dat"); //get the dat file
	std::vector <TFile*> files;
	std::string fnames[4], t[4]; //time needs to be the time of the run
	int bn[4];
	fnames[0] = "119";
	fnames[1] = "125";
	fnames[2] = "107";
	fnames[3] = "23";
	t[3] = "Tue Jan 30 14:02:24 (78850930)";
	t[2] = "Tue Jan 30 11:29:49 2018 (78841776)";
	t[1] = "Mon Jan 29 17:14:16 2018 (78776042)";
	t[0] = "Mon Jan 29 16:27:51 2018 (78772357)";
	for (int i = 0; i < 4; i++)
	{
		bn[i] = std::stoi(fnames[i]);
		t[i] = "#** Start session " + t[i] + "**";
		fnames[i] = "boardnumber_" + fnames[i]+".root";
		files.push_back(new TFile(fnames[i].c_str(), "RECREATE"));
		scan_dat_file(&log_file, files.at(i), bn[i], t[i]);
	}
	


	return 0;
}

