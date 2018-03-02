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
std::vector <TH1F*> avg_hists;
std::vector<TH1F*> TempLoss(std::vector<std::vector<float>> data, std::string board_label, const long init_time)
{
	TH1F* Temp = new TH1F("temp", "temp", 100, 0, 20);
	TH1F* Tloss = new TH1F((board_label + "_tloss").c_str(), ("Expected Temperature increase for chip " + board_label + " from initial; Temperature increase (K)").c_str(), 100, 0, 2);
	TH1F* TGain = new TH1F((board_label + "_act_tloss" ).c_str(), ("Measured Temperature increase for chip " + board_label + "from initial; Teperature increase (K)").c_str(), 100, 0, 2);
	TH1F* TDiff = new TH1F((board_label + "_diffs_of_loss").c_str(), ("Diffences of Temperature Increase approach for chip " + board_label + ";  Teperature increase (K)").c_str(), 100, 0, 2);
	float tinit = data.at(1).at(10);
	const float Ts = 20, hs = 10.88, A = 0.0075;
	for (int i = 0; i < data.size(); i++)
	{
		//if (data.at(i).at(10) == 0) data.at(i).at(10) = 100;
		float tdiff = data.at(i).at(10) - tinit;
		float tmdiff = (data.at(i).at(1)*data.at(i).at(2) + data.at(i).at(3)*data.at(i).at(4)) / (A*hs) + Ts - tinit;
		long time_spot = data.at(i).at(0) - init_time;
		std::cout << "Temp diff measured " << tdiff << " K" << " versus what it actually is" <<data.at(i).at(10)<<std::endl;
		time_spot = time_spot / 10;
		float diffs = abs(tdiff - tmdiff);
		Temp->Fill(data.at(i).at(10));
		Tloss->Fill(tmdiff);
		TGain->Fill(tdiff);
		TDiff->Fill(diffs);
	}
	std::vector<TH1F*> Temps(4);
	Temps.at(0) = Tloss;
	Temps.at(1) = TGain;
	Temps.at(2) = TDiff;
	Temps.at(3) = Temp;
	Tloss->Delete();
	TGain->Delete();
	//tinit->clear;
	return Temps;
}
std::vector<std::vector<float>> time_averging(std::vector<std::vector<float>> data)
{
	std::vector<float> ta_vd, ta_va, ta_ad, ta_aa;
	float vd=0, va=0, ad=0, aa=0, n=0;
//	std::cout << "Input vector has size " << data.size() << " with " << data.at(0).size() << " columns" << std::endl;
	for (int i = 0; i < data.size(); i++)
	{
		if (data.at(i).size() < 5)continue;
		vd += data.at(i).at(1);

		ad += data.at(i).at(2);
		try {
			va += data.at(i).at(3);
			aa += data.at(i).at(4);
			n++;
		}
		catch (std::exception& e) {};
		if (i % 6 == true) {
			if (n != 0) {
			ta_vd.push_back(vd / n);

			ta_ad.push_back(ad / n);
			
				ta_va.push_back(va / n);
				ta_aa.push_back(aa / n);

			}
			n = 0;
			vd = 0;
			va = 0;
			aa = 0;
			ad = 0;
		}
	}
	std::vector <std::vector<float>> avgs (4);
	avgs.at(0) = ta_vd;
	avgs.at(1) = ta_ad;
	avgs.at(2) = ta_va;
	avgs.at(3) = ta_aa;
//	std::cout << "Readout vectors have size " << ta_vd.size() << std::endl;
	return avgs;
}
int scan_dat_file(fstream* inf, TFile* f, int boardnumber, std::string time_code, bool aon, bool lastof, int rn, int lastline, std::vector<TH1F*> ttemps)
{
	std::string readout = "";
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
			if (tline < lastline)continue;
			//std::cout << tline << std::endl;
			if (readout.find(time_code.c_str()) == std::string::npos &&wc==0 &&hasread==0) std::cout << '\r' << "Looking for value at line " << tline << std::flush;
			if (readout.find(time_code.c_str()) != std::string::npos) {
				wc = 1;
				hasread = true;
				std::cout << "\n Start signal " << time_code << " sent" << "\n Line is " <<tline <<"\n Line reads " <<readout << std::endl;
				line = tline;
			}
			if (readout.find("Start") != std::string::npos && readout.find(time_code.c_str()) == std::string::npos) wc = 0;
			//while (wc == 0) std::cout << "Still looking for start signal" << std::endl;
			//if (hasread && !wc) esccondition = 0;
			if (wc == 1)//&& readout.compare(time_code)==0 && readout.compare( "#** t(s),v0Mon(V),i0Mon(mA),v1Mon(V),i1Mon(mA),VDDA_RAW(mV),VDDA_REG(mV),VDDD_RAW(mV),VDDD_REG(mV),MOD_GND(mV),Temperature(C),IrradStatus(On = 1),DoseRate(kRad/hr),TotalDose(MRad) **")==0 && readout.compare("#** -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- **")==0)
			{

				line++;
				columns.clear();
				std::istringstream temp(readout);
				std::string temp2;
				while (std::getline(temp, temp2, ' ')) columns.push_back(temp2);
				//std::vector <float>lvec;
				for (int i = 0; i < columns.size(); i++) {
					//std::cout << columns.at(i) << std::endl;
					try {
						lvec.push_back(std::stof(columns.at(i).c_str()));
						}
					catch (const std::exception& e) {
						continue;

					}
				//	std::cout << "found " <<lvec.size() <<" columns" <<std::endl; 
				}
				if (lvec.size() <14 || columns.size() == 0) {
					//	std::cout << "vectors unfilled on line " << line <<"\n line reads " <<readout << std::endl;
					continue;
				}
				vals.push_back(lvec);
				lvec.clear();
				if (boardnumber == 84 && vals.size() >= 44) {
					wc = 0;
					continue;
				}
				//if (boardnumber == 84) std::cout << "\r Reading code is " << wc << std::flush;

			}
			//			if(wc==0) continue;

						//std::cout << vals.at(2).size() << std::endl; //having this line in causes the columns request to fail....why???????


			if (wc == 0 && hasread == true || inf->eof())esccondition = false;
		}
	}
	std::cout << " \n Having escaped the carven of the terrible while loop, our hero travels forth" << std::endl;
	hists.push_back(new TH1F(("vd" + std::to_string(rn)).c_str(), "Voltage in Digital Channel; Time (s); Voltage (V)", vals.size(), 0, 10 * vals.size()));
	//std::cout << vals.at(0).size() << std::endl;

	for (int i = 0; i < vals.size(); i++) {
		try {
			vals.at(i).at(0) = vals.at(i).at(0) - vals.at(0).at(0);
			//std::cout << " \r Time of envent is at " << vals.at(i).at(0) << "seconds from start" << std::flush;
			hists.at(0)->SetBinContent(i, vals.at(i).at(1));
			//std::cout <<"There are "<< vals.at(i).size() <<" variables at play" << std::endl;
		}

		catch (std::exception& e) { //std::cout << " \n Error in element " <<i  <<"of type " <<e.what()<< std::endl;
			continue;
		}
	}
	if (boardnumber == 48) std::cout << "Sample Current " << vals.at(10).at(2) << std::endl;
	hists.push_back(new TH1F(("id" + std::to_string(rn)).c_str(), "Current in Digital Channel; Time (s); Current (mA)", vals.size(), 0, 10 * vals.size()));
	for (int i = 0; i < vals.size(); i++) {
		try { hists.at(1)->SetBinContent(i, 1000 * vals.at(i).at(2)); }
		catch (...) {
			continue;
		}
	}
	hists.push_back(new TH1F(("pd" + std::to_string(rn)).c_str(), "Power in Digital Channel; Time (s); Power(mW)", vals.size(), 0, 10 * vals.size()));
	for (int i = 0; i < vals.size(); i++) {
		try {
			//vals.at(i).at(0) = vals.at(i).at(0) - vals.at(0).at(0);
			hists.at(2)->SetBinContent(i, vals.at(i).at(1)*vals.at(i).at(2) * 1000);
		}
		catch (std::exception& e) { continue; }
	}
	if (aon = true) {
		hists.push_back(new TH1F(("va"+std::to_string(rn)).c_str(), "Voltage in Analog Channel; Time (s); Voltage (V)", vals.size(), 0, 10 * vals.size()));
		//std::cout << vals.at(0).size() << std::endl;

		for (int i = 0; i < vals.size(); i++) {
			try {
				vals.at(i).at(0) = vals.at(i).at(0) - vals.at(0).at(0);
				//std::cout << " \r Time of envent is at " << vals.at(i).at(0) << "seconds from start" << std::flush;
				hists.at(3)->SetBinContent(i, vals.at(i).at(3));
				//std::cout << vals.at(0).at(i) << std::endl;
			}

			catch (std::exception& e) {
				//std::cout << " \n Error in element " << i << "of type " << e.what() << std::endl;
				continue;
			}
		}
		hists.push_back(new TH1F(("ia"+std::to_string(rn)).c_str(), "Current in Analog Channel; Time (s); Current (mA)", vals.size(), 0, 10 * vals.size()));
		for (int i = 0; i < vals.size() - 1; i++) {
			try { hists.at(4)->SetBinContent(i, 1000 * vals.at(i).at(4)); }
			catch (...) {
				continue;
			}
		}
		hists.push_back(new TH1F(("pa"+std::to_string(rn)).c_str(), "Power in Analog Channel; Time (s); Power(mW)", vals.size(), 0, 10 * vals.size()));
		for (int i = 0; i < vals.size(); i++) {
			try {
				//vals.at(i).at(0) = vals.at(i).at(0) - vals.at(0).at(0);
				hists.at(5)->SetBinContent(i, vals.at(i).at(3)*vals.at(i).at(4) * 1000);
			}
			catch (std::exception& e) { continue; }
		}
	}
	int index = vals.size();
	TVectorF T(index), C(index), V(index);
	int j = 0;
	for (int i = 0; i < vals.size(); i++) {
		try {
			if (vals.at(i).at(10) <= 0) { //checks for faulty Temperatures and discards them
				continue;
			}
			T[j] = vals.at(i).at(10); //use of j to avoid holes in the vector while parsing
			C[j] = vals.at(i).at(2);
			V[j] = vals.at(i).at(1);
			j++;

		}
		catch (std::exception& e) {
			std::cout << 'r' << e.what() << std::flush;
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
	//std::cout << "\n" <<tempgraphs.size() <<" Temperature Graphs recorded" << std::endl; //sanity check
//	f->Write();
	//f->Close();
	//if (vals.at(1).size() == 14) {
		std::vector<std::vector<float>> av = time_averging(vals);

		try {

			for (int i = 0; i < av.size(); i++) {
				for (int j = 0; j < av.at(i).size(); j++) avg_hists.at(i)->Fill(av.at(i).at(j));
			}
		}
		catch (std::exception& e) { std::cout << "Unable to perform time averaging on run " << rn << "\n Returned error " << e.what() << std::endl; }
		av.~vector();
		
		try {
				std::vector<TH1F*> temps = TempLoss(vals, std::to_string(boardnumber), vals.at(0).at(0));
				if (rn == 0) {
					for (int k = 0; k < temps.size(); k++) {
						ttemps.at(k)=temps.at(k);
					}
				}
				try {
				if(rn==0) {
					for (int k = 0; k < temps.size(); k++) ttemps.at(k)->Add(temps.at(k));
				}
				}
				catch (std::exception& e) {};
		}
		catch (std::exception& e) { std::cout << "Unable to perform Temp Analysis on run " << rn << "\n Returned error " << e.what() << std::endl; }
	//}
	T.Delete();
	C.Delete();
	V.Delete();
	columns.~vector();
	vals.~vector();
	//hists.~vector();
	//av.~vector();
	//if (lastof) {
		//for (int i = 0; i < avg_hists.size(); i++) avg_hists.at(i)->Write(0,1,0);
	//}
//	avg_hists.~vector();
	//get rid of memory leak possiblity
	return line;
}
int main()
{
	fstream log_file;
	 //get the dat file
	std::vector <TFile*> files;
	std::vector <std::vector <std::string>> t;
	std::vector <std::string> fnames;
	std::cout << "First vars loaded" << std::endl;
	int bn[9];
	int nruns[9] = { 0,0,0,0,0,0,0,0,0 };
	std::cout << "Second set" << std::endl;
	//allows for more generalizability than using array
	//std::string fnames[8], t[8]; //time needs to be the time of the run
	//int bn[4];
	//fnames[0] = "119";
	//fnames[1] = "125";
	//fnames[2] = "107";
	//fnames[3] = "23";
	/*t[3] = "Jan 30 14:02"; //the actual time code is not working with str::find, have to pay closer attention to the limit cases??
	t[2] = "Jan 30 11";
	t[1] = "Jan 29 17";
	t[0] = "Jan 29 16:27";
	t[6] = "Jan 31 15:36";
	t[5] = "Jan 31 15:26";
	t[7] = "Jan 31 15:11";
	t[4] = "Jan 31 15:03";*/
	fstream runtimes;
	std::string line;
	std::vector<TCanvas*> cvec(4);
	for (int i = 0; i < 4; i++) cvec.at(i) = new TCanvas();
	std::vector<TLegend*> legs(4);
	for (int i = 0; i < 4; i++) legs.at(i)= new TLegend(0.85, 0.7, 1, 0.9);
	runtimes.open("runlog.txt");
	int i = 0;
	while( runtimes.is_open() && !runtimes.eof()) {
		while (getline(runtimes, line)) {
				//std::cout << line << std::endl;
				if (line.find("End") != std::string::npos) runtimes.close();
				if (line.find("#") != std::string::npos) {
					std::istringstream templine(line);
					std::string temp;
					while (getline(templine, temp, ' ')) {
						try {
							stof(temp);
							//if (temp == fnames.at(i)) continue;
							fnames.push_back(temp);
							std::vector <std::string> blah (0);
							t.push_back(blah);
							i++;
						}
						catch (std::exception& e) {
							continue;
						}
					}
				}
				if(line.find("#")==std::string::npos && line.find("End")==std::string::npos) {
					if (i > 0) {
						t.at(i-1).push_back(line);
						//std::cout << line << std::endl;

						nruns[i - 1]++;
					}
					else continue;
				}
				//if(line.find("Fil")) runtimes.close();

		}
	}
	for (int i = 0; i < fnames.size(); i++) {
		std::cout << fnames.at(i) << std::endl;
		bn[i] = std::stoi(fnames[i]);
		//fnames[4 + i] = fnames[i] + "_run_2";
	}
	int l = 0;
	for (int i = 0; i < fnames.size(); i++)
	{

		avg_hists.clear();
		std::cout << "Starting with board number " << fnames[i] << std::endl;
	//	bn[i] = std::stoi(fnames[i]);
	//	t[i] = "#** Start session " + t[i] + "**";
		//if (bn[i] != 23) continue;
		std::string ftnames = "boardnumber_" + fnames[i]+".root";
		files.push_back(new TFile(ftnames.c_str(), "RECREATE"));
		std::vector <TH1F*> ttemps(4);
		//std::cout << nruns[i] << std::endl;
		//std::vector<TH1F*>avg_hists(4);
		avg_hists.push_back( new TH1F(("Avd" + fnames[i]).c_str(), "Average Voltages in Digital Channel on Chip ; Voltage averaged over 1 minute intervals (V)", 150, 1.49, 1.56));
		avg_hists.push_back( new TH1F(("Aad" + fnames[i]).c_str(), "Average Curents in Digital Channel on Board ; Current averaged over 1 minute intervals (A)", 150, 0.0315, 0.0355));
		avg_hists.push_back( new TH1F(("Ava" + fnames[i]).c_str(), "Average Voltages in Analog Channel on Board ; Voltage averaged over 1 minute intervals (V)", 200, 0.8, 1.55));
		avg_hists.push_back(new TH1F(("Aaa" + fnames[i]).c_str(), "Average Curents in Analog Channel on Board ; Current averaged over 1 minute intervals (A)", 100, 0.03, 0.06));
		int lastline = 0;
		for (int j = 0; j < nruns[i]; j++)
		{
			log_file.open("C:/Users/Silas Grossberndt/Documents/ABC_Boards/TIDLogTesting.dat"); //to reset the read code each time
			bool lo = (j == nruns[i]); 
			l++;
			//if (l == 40) continue;
			//std::cout << "Looking for data taken from index " << l-1 << std::endl;
			try {

				lastline=scan_dat_file(&log_file, files.at(i), bn[i], t.at(i).at(j), true,lo, j, lastline, ttemps);
				std::cout << "Run number " << j << " complete on chip " << bn[i] << std::endl;
			}
			catch (std::exception& e) { continue; }
			//l++;
			log_file.close();
		}
		for (int j = 0; j < avg_hists.size(); j++) {
			avg_hists.at(j)->Write();
			ttemps.at(j)->Write();
			avg_hists.at(j)->SetStats(0);
			avg_hists.at(j)->SetLineColor(i);
			//avg_hists.at(j)->Sumw2();
			avg_hists.at(j)->Scale(1 / (avg_hists.at(j)->Integral()));
			cvec.at(j)->cd();
			if (i == 0) {
				std::string title = avg_hists.at(j)->GetTitle();
				avg_hists.at(j)->SetTitle((title + " Renormailzed to #int =1").c_str());
			}
			//avg_hists.at(j)->SetLineStyle(1);
			avg_hists.at(j)->GetYaxis()->SetRangeUser(0.00001, 1);
			//cvec.at(j)->SetLogy();
			legs.at(j)->AddEntry(avg_hists.at(j), fnames.at(i).c_str());
			avg_hists.at(j)->Draw("same");
			legs.at(j)->Draw();
		}
		if (files.size() == 1) files.at(0)->Close();
		else files.at(i)->Close();
		std::cout << fnames.at(i) << " is closed" << std::endl;
	}
	for (int i = 0; i < 4; i++) {
		cvec.at(i)->Print(("Canvas_" + std::to_string(i) + ".pdf").c_str());
		cvec.at(i)->SetLogy();
		cvec.at(i)->Print(("Canvas_" + std::to_string(i) + "log_y.pdf").c_str());
	}
/*	TFile* f = new TFile("current.root", "Recreate");
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
	*/

	return 0;
}

