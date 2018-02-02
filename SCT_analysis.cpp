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
	std::string readout="";
	int wc = 0, line = 0, tline = 0;
	bool hasread = false;
	std::vector <std::string> columns;
	std::vector <TH1F*> hists;
	std::vector <std::vector <float> > vals;
	std::vector <float> lvec;
	fstream err;
	err.open("error_file.txt");
	//vals.resize(11);
	//std::vector <float> vd, t, id, va, ia, vdraw, vdreg, varaw, vareg, mgnd, T;
	bool esccondition = inf->is_open();
	std::cout << "Ready to begin looping, Start Signal is " <<time_code << std::endl;
	while (esccondition) {
		while (std::getline(*inf, readout))
		{
			tline++;
			//std::cout << tline << std::endl;
			if(readout.compare(time_code) != 0) std::cout <<'\r' << "Looking for value at line " << tline << std::flush;
			if (readout.find(time_code)!=std::string::npos) {
				wc = 1;
				hasread = true;
				std::cout << "Now in data taking mode" <<"\n Start signal " <<time_code <<" sent" <<"\n This is same as " <<readout <<std::endl;
			}
			if (readout.find("Start")!=std::string::npos && readout.find(time_code)==std::string::npos) wc = 0;
			//while (wc == 0) std::cout << "Still looking for start signal" << std::endl;
			if (wc == 1 )//&& readout.compare(time_code)==0 && readout.compare( "#** t(s),v0Mon(V),i0Mon(mA),v1Mon(V),i1Mon(mA),VDDA_RAW(mV),VDDA_REG(mV),VDDD_RAW(mV),VDDD_REG(mV),MOD_GND(mV),Temperature(C),IrradStatus(On = 1),DoseRate(kRad/hr),TotalDose(MRad) **")==0 && readout.compare("#** -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- **")==0)
			{
				
				//if (readout == "") continue;
				line++;
				//std::cout << line << std::endl;
				//for (int i = 0; i < 11; i++) vals.at(i).resize(line);
				//std::cout << vals.size() << std::endl;
				//std::string sline = line.itos();
				columns.clear();
				std::istringstream temp(readout);
				std::string temp2;
				while (std::getline(temp, temp2, ' ')) columns.push_back(temp2);
				//std::vector <float>lvec;
				for (int i = 0; i < columns.size(); i++) {
					//std::cout << columns.at(i) << std::endl;
					try {
							lvec.push_back(std::stof(columns.at(i).c_str()));
							//std::cout << lvec.at(i) << std::endl;
						}
					catch (const std::exception& e) {
						continue;
						//std::cout << columns.at(i) << std::endl;
					}
					 //std::cout << "error on line " <<line <<std::endl; }
				}
				if (lvec.size() == 0 || columns.size() == 0) {
				//	std::cout << "vectors unfilled on line " << line <<"\n line reads " <<readout << std::endl;
					continue;
				}
				vals.push_back(lvec);
				 /*try { float a = lvec.at(0); }
				catch (std::exception& e) {
					if (line == 200) { std::cout << e.what() << " on line " << line << "\n which reads " << readout << std::endl; }
				}//having this line in causes the columns request to fail....why???????*/
				lvec.clear();
				//std::cout << vals.size() << std::endl;
				
			}
			if(wc==0) continue;

			//std::cout << vals.at(2).size() << std::endl; //having this line in causes the columns request to fail....why???????
		
		}
		if (wc == 0 && hasread == true || inf->eof())esccondition = false;
	}
	std::cout << " \n Having escaped the carven of the terrible while loop, our hero travels forth" << std::endl;
	hists.push_back(new TH1F("vd", "Voltage in Digital Channel; Time (s); Voltage (V)", vals.size(), 0, vals.size()));
	//std::cout << vals.at(0).size() << std::endl;
	
	for (int i = 0; i < vals.size()-1; i++) {
		try {
			vals.at(i).at(0) = vals.at(i).at(0) - vals.at(0).at(0);
			std::cout << " \r Time of envent is at " << vals.at(i).at(0) << "seconds from start" << std::flush;
			hists.at(0)->SetBinContent(i, vals.at(i).at(1));
			//std::cout << vals.at(0).at(i) << std::endl;
		}

		catch (std::exception& e) { std::cout << " \n Error in element " <<i  <<"of type " <<e.what()<< std::endl; 
		continue; 
		}
	}
	hists.push_back(new TH1F("id", "Current in Digital Channel; Time (s); Current (mA)", vals.size(), 0, vals.size()));
	for (int i = 0; i < vals.size() - 1; i++) {
		try { hists.at(1)->SetBinContent(i, 1000 * vals.at(i).at(2)); }
		catch (...) {
			continue;
		}
	}
	hists.push_back(new TH1F("pd", "Power in Digital Channel; Time (s); Power(mW)", vals.size(), 0, vals.size()));
	for (int i = 0; i < vals.size(); i++) {
		try {
			//vals.at(i).at(0) = vals.at(i).at(0) - vals.at(0).at(0);
			hists.at(2)->SetBinContent(i, vals.at(i).at(1)*vals.at(i).at(2) * 1000);
		}
		catch (std::exception& e) { continue; }
	}
	for (int i = 0; i < hists.size(); i++) hists.at(i)->Write();
	std::cout << "Done" << std::endl;
	f->Write();
	f->Close();
}


	

	int main()
{
	fstream log_file;
	 //get the dat file
	std::vector <TFile*> files;
	std::string fnames[4], t[4]; //time needs to be the time of the run
	int bn[4];
	fnames[0] = "119";
	fnames[1] = "125";
	fnames[2] = "107";
	fnames[3] = "23";
	t[3] = "Jan 30 14:02"; //the actual time code is not working with str::find, have to pay closer attention to the limit cases??
	t[2] = "Jan 30 11";
	t[1] = "Jan 29 17";
	t[0] = "Jan 29 16:27";
	for (int i = 0; i < 4; i++)
	{
		log_file.open("C:/Users/Silas Grossberndt/Documents/ABC_Boards/TIDLogTesting.dat"); //to reset the read code each time
		std::cout << "Starting with board number " << fnames[i] << std::endl; 
		bn[i] = std::stoi(fnames[i]);
	//	t[i] = "#** Start session " + t[i] + "**";
		fnames[i] = "boardnumber_" + fnames[i]+".root";
		files.push_back(new TFile(fnames[i].c_str(), "RECREATE"));
		scan_dat_file(&log_file, files.at(i), bn[i], t[i]);
		log_file.close();
	}
	//exit behavior poorly defined--this is causing issues with the exit behavior of board 23, I am trying to fix this by specifing the timing


	return 0;
}

