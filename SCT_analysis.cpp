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
#include "C:/root_v5.34.36/include/TCanvas.h"
#include "C:/root_v5.34.36/include/TLegend.h"
#include "C:/root_v5.34.36/include/TGraph.h"
#include "C:/root_v5.34.36/include/TVectorF.h"
std::vector <TH1F*> PAall, PDall;
std::vector <TGraph*> tempgraphs;
void scan_dat_file(fstream* inf, TFile* f, int boardnumber, std::string time_code, bool aon)
{
	std::string readout="";
	int wc = 0, line = 0, tline = 0;
	bool hasread = false;
	std::vector <std::string> columns;
	std::vector <TH1F*> hists;
	std::vector <std::vector <float> > vals;
	std::vector <float> lvec;
	//fstream err;
	//err.open("error_file.txt");
	//vals.resize(11);
	//std::vector <float> vd, t, id, va, ia, vdraw, vdreg, varaw, vareg, mgnd, T;
	bool esccondition = inf->is_open();
	//std::cout << "Ready to begin looping, Start Signal is " <<time_code << std::endl;
	while (esccondition) {
		while (std::getline(*inf, readout))
		{
			tline++;
			//std::cout << tline << std::endl;
			if(readout.compare(time_code) != 0) std::cout <<'\r' << "Looking for value at line " << tline << std::flush;
			if (readout.find(time_code)!=std::string::npos) {
				wc = 1;
				hasread = true;
				//std::cout << "Now in data taking mode" <<"\n Start signal " <<time_code <<" sent" <<"\n This is same as " <<readout <<std::endl;
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
	hists.push_back(new TH1F("vd", "Voltage in Digital Channel; Time (s); Voltage (V)", vals.size(), 0, 10*vals.size()));
	//std::cout << vals.at(0).size() << std::endl;
	
	for (int i = 0; i < vals.size()-1; i++) {
		try {
			vals.at(i).at(0) = vals.at(i).at(0) - vals.at(0).at(0);
			std::cout << " \r Time of envent is at " << vals.at(i).at(0) << "seconds from start" << std::flush;
			hists.at(0)->SetBinContent(i, vals.at(i).at(1));
			//std::cout << vals.at(0).at(i) << std::endl;
		}

		catch (std::exception& e) { //std::cout << " \n Error in element " <<i  <<"of type " <<e.what()<< std::endl; 
		continue; 
		}
	}
	hists.push_back(new TH1F("id", "Current in Digital Channel; Time (s); Current (mA)", vals.size(), 0, 10*vals.size()));
	for (int i = 0; i < vals.size() - 1; i++) {
		try { hists.at(1)->SetBinContent(i, 1000 * vals.at(i).at(2)); }
		catch (...) {
			continue;
		}
	}
	hists.push_back(new TH1F("pd", "Power in Digital Channel; Time (s); Power(mW)", vals.size(), 0, 10*vals.size()));
	for (int i = 0; i < vals.size(); i++) {
		try {
			//vals.at(i).at(0) = vals.at(i).at(0) - vals.at(0).at(0);
			hists.at(2)->SetBinContent(i, vals.at(i).at(1)*vals.at(i).at(2) * 1000);
		}
		catch (std::exception& e) { continue; }
	}
	if (aon = true) {
		hists.push_back(new TH1F("va", "Voltage in Analog Channel; Time (s); Voltage (V)", vals.size(), 0, 10 * vals.size()));
		//std::cout << vals.at(0).size() << std::endl;

		for (int i = 0; i < vals.size() - 1; i++) {
			try {
				vals.at(i).at(0) = vals.at(i).at(0) - vals.at(0).at(0);
				std::cout << " \r Time of envent is at " << vals.at(i).at(0) << "seconds from start" << std::flush;
				hists.at(3)->SetBinContent(i, vals.at(i).at(3));
				//std::cout << vals.at(0).at(i) << std::endl;
			}

			catch (std::exception& e) {
				//std::cout << " \n Error in element " << i << "of type " << e.what() << std::endl;
				continue;
			}
		}
		hists.push_back(new TH1F("ia", "Current in Analog Channel; Time (s); Current (mA)", vals.size(), 0, 10 * vals.size()));
		for (int i = 0; i < vals.size() - 1; i++) {
			try { hists.at(4)->SetBinContent(i, 1000 * vals.at(i).at(4)); }
			catch (...) {
				continue;
			}
		}
		hists.push_back(new TH1F("pa", "Power in Analog Channel; Time (s); Power(mW)", vals.size(), 0, 10 * vals.size()));
		for (int i = 0; i < vals.size(); i++) {
			try {
				//vals.at(i).at(0) = vals.at(i).at(0) - vals.at(0).at(0);
				hists.at(5)->SetBinContent(i, vals.at(i).at(3)*vals.at(i).at(4) * 1000);
			}
			catch (std::exception& e) { continue; }
		}
	}
	int index = vals.size();
	TVectorF T(index),  C(index), V(index);
	int j = 0;
	for (int i = 0; i < vals.size(); i++) {
		try {
			if (vals.at(i).at(10) <= 0) { //checks for faulty Temperatures and discards them
				continue;
			}
			T[j]=vals.at(i).at(10); //use of j to avoid holes in the vector while parsing
			C[j] = vals.at(i).at(2);
			V[j] = vals.at(i).at(1);
			j++;
			
		}
		catch (std::exception& e) { std::cout <<'r'<< e.what() << std::flush; 
		continue;  //should not throw an exception, but getting out of range errors sill on vals
		}
	}
	tempgraphs.push_back(new TGraph(C, T)); //creates current graphs
	std::cout << "\n Temp is: " << T[index / 2] << " C on the chip" << std::endl;
	tempgraphs.push_back(new TGraph(V, T));
	for (int i = 0; i < hists.size(); i++) hists.at(i)->Write();
	//***NOTE: PD & PA have older variable names from attempting to plot Powers instead of currents, Too lazy to change ***
	PAall.push_back(hists.at(4)); //analog current 
	PDall.push_back(hists.at(1)); //Digital current
	std::cout << "\n" <<tempgraphs.size() <<" Temperature Graphs recorded" << std::endl; //sanity check
//	f->Write();
	f->Close();
	T.Delete();
	C.Delete();
	V.Delete();
	columns.~vector();
	vals.~vector(); 
	//get rid of memory leak possiblity 
}


	

	int main()
{
	fstream log_file;
	 //get the dat file
	std::vector <TFile*> files;
	std::string fnames[8], t[8]; //time needs to be the time of the run
	int bn[4];
	fnames[0] = "119";
	fnames[1] = "125";
	fnames[2] = "107";
	fnames[3] = "23";
	t[3] = "Jan 30 14:02"; //the actual time code is not working with str::find, have to pay closer attention to the limit cases??
	t[2] = "Jan 30 11";
	t[1] = "Jan 29 17";
	t[0] = "Jan 29 16:27";
	t[6] = "Jan 31 15:36";
	t[5] = "Jan 31 15:26";
	t[7] = "Jan 31 15:11";
	t[4] = "Jan 31 15:03";
	for (int i = 0; i < 4; i++) {
		bn[i] = std::stoi(fnames[i]);
		fnames[4 + i] = fnames[i] + "_run_2";
	}
	for (int i = 0; i < 8; i++)
	{
		log_file.open("C:/Users/Silas Grossberndt/Documents/ABC_Boards/TIDLogTesting.dat"); //to reset the read code each time
		std::cout << "Starting with board number " << fnames[i] << std::endl; 
	//	bn[i] = std::stoi(fnames[i]);
	//	t[i] = "#** Start session " + t[i] + "**";
		std::string ftnames = "boardnumber_" + fnames[i]+".root";
		files.push_back(new TFile(ftnames.c_str(), "RECREATE"));
		if (i<4) scan_dat_file(&log_file, files.at(i), bn[i%4], t[i], true);
		if (i >=4) scan_dat_file(&log_file, files.at(i), bn[i % 4], t[i], true);
		log_file.close();
	}
	TFile* f = new TFile("current.root", "Recreate");
	for (int i = 0; i < 8; i++) {
		std::string ahname, dhname, titles;
		ahname = fnames[i] + "pa";

		dhname = fnames[i] + "pd";
		PAall.at(i)->SetName(ahname.c_str());
		PDall.at(i)->SetTitle(titles.c_str());
		PDall.at(i)->SetName(dhname.c_str());
		PDall.at(i)->GetYaxis()->SetRangeUser(0, 60);
		PAall.at(i)->GetYaxis()->SetRangeUser(0, 100);
		PDall.at(i)->SetLineColor(i);
		PAall.at(i)->SetLineColor(i);
		PDall.at(i)->SetStats(false);
		PAall.at(i)->SetStats(false);
		PAall.at(i)->Write();
		PDall.at(i)->Write();
	}
	TCanvas* c1 = new TCanvas();
	TLegend* l = new TLegend(0.7, 0.95, 0.95, 0.7);
	TCanvas* c2 = new TCanvas();
	c1->cd();
	//c1->SetLogy();
	//c2->SetLogy();
	c1->SetLogx();
	c2->SetLogx();
	for (int i = 0; i < PDall.size(); i++) {
		//PDall.at(i)->Draw("same ][");
		int b = bn[i % 4];
		std::string titles = "Current on chip " + std::to_string(b);
		PDall.at(i)->SetTitle(titles.c_str());
		l->AddEntry(PDall.at(i) );
		PDall.at(i)->SetTitle("TID Digital Current Measurement");
		PDall.at(i)->Draw("same ][");
	}
	l->Draw("same");
	c2->cd();
	TLegend* l1 = new TLegend(0.7, 0.95, 0.95, 0.7);
	for (int i = 4; i < PAall.size(); i++)
	{
		//PAall.at(i)->Draw("same ][");
		int b = bn[i % 4];
		std::string titles = "Current on Chip " + std::to_string(b);
		PAall.at(i)->SetTitle(titles.c_str());
		l1->AddEntry(PAall.at(i));
		PAall.at(i)->SetTitle("TID Analog Current Measurement");
		PAall.at(i)->Draw("same ][");
	}
	l1->Draw("same");
	c1->Print("Digital_powers.png");
	c2->Print("Analog_powers.png");
	c1->Write();
	c2->Write();
	TCanvas* c3 = new TCanvas();
	TCanvas *c4 = new TCanvas();
	c3->cd();
	ofstream erl;
	erl.open("erl.txt");
	TLegend *ct=new TLegend(0.7, 0.95, 0.9, 0.7), *vt=new TLegend(0.7, 0.95, 0.9, 0.7);
	erl << tempgraphs.size() << std::endl;
	for (int i = 0; i < tempgraphs.size(); i++) {
		tempgraphs.at(i)->GetYaxis()->SetRangeUser(20, 22);
		if (i % 2 == 0) {
			c3->cd();

			tempgraphs.at(i)->SetName((std::to_string(bn[(i / 2) % 4]) + "c").c_str());
			tempgraphs.at(i)->SetTitle( ("Chip " + std::to_string(bn[(i / 2) % 4])).c_str());
			tempgraphs.at(i)->SetLineColor((i+1)/2);
			tempgraphs.at(i)->SetMarkerColor((i+1)/2);
			ct->AddEntry(tempgraphs.at(i));
			if (i == 2) {
				for (int j = 0; j < 200; j++) {
					float test = PDall.at(i)->GetBinContent(j);
					erl <<test <<" to deg "<< tempgraphs.at(i)->Eval(test) << std::endl;
				}
			}
			tempgraphs.at(i)->GetXaxis()->SetTitle("Current (mA)");
			tempgraphs.at(i)->GetXaxis()->SetRangeUser(0.03, 0.05);
			tempgraphs.at(i)->GetYaxis()->SetTitle("Temperature (C)");
			tempgraphs.at(i)->SetTitle("Current versus Temperature");
			if (i == 0) tempgraphs.at(i)->Draw("AC*");
			tempgraphs.at(i)->Draw("same *");
			tempgraphs.at(i)->Write();
		}
		else { 
			c4->cd();
			tempgraphs.at(i)->SetName((std::to_string(bn[(i / 2) % 4]) + "v").c_str());
			tempgraphs.at(i)->SetTitle(("Chip " + std::to_string(bn[(i / 2) % 4])).c_str());
			tempgraphs.at(i)->SetLineColor((i+1) / 2);
			tempgraphs.at(i)->SetMarkerColor((i+1) / 2);
			tempgraphs.at(i)->GetXaxis()->SetRangeUser(1.5, 1.6);
			vt->AddEntry(tempgraphs.at(i));
			tempgraphs.at(i)->GetXaxis()->SetTitle("Voltage (mV)");
			tempgraphs.at(i)->GetYaxis()->SetTitle("Temperature (C)");
			tempgraphs.at(i)->SetTitle("Voltage versus Temperature");
			if (i==1) tempgraphs.at(i)->Draw("A*");
			tempgraphs.at(i)->Draw("same *");
			tempgraphs.at(i)->Write();
		}

	}
	c3->cd();
	ct->Draw("same");
	c4->cd();
	vt->Draw("same");
	c3->Write();
	c4->Write();
	//c3->Print("TV.pdf[");
	c3->Print("TC.png");
	c4->Print("TV.png");
	//c4->Print("TV.pdf]");
	f->Write();
	//exit behavior poorly defined--this is causing issues with the exit behavior of board 23, I am trying to fix this by specifing the timing


	return 0;
}

