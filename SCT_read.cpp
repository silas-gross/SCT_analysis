#include "SCT_read.h"

std::vector<TH1F*> SCT_read::TempLoss(std::vector<std::vector<float>> data, std::string board_label, const long init_time)
{
	TH1F* Tloss = new TH1F((board_label + "_tloss").c_str(), ("Expected Temperature increase for chip " + board_label + "; time (10 s); Temperature increase (K)").c_str(), 1000000, 0, 1000000);
	TH1F* TGain = new TH1F((board_label + "_act_tloss").c_str(), ("Measured Temperature increase for chip " + board_label + "; time (10 s); Teperature increase (K)").c_str(), 1000000, 0, 1000000);
	TH1F* TDiff = new TH1F((board_label + "_diffs_of_loss").c_str(), ("Diffences of Temperature Increase approach for chip " + board_label + "; time (10 s); Teperature increase (K)").c_str(), 1000000, 0, 1000000);
	float* tinit = &data.at(0).at(10);
	const float Ts = 3.0, hs=10.88, A=0.000075; 
	for (int i = 0; i < data.size(); i++)
	{
		if (data.at(i).at(10) == 0) continue;
		float tdiff = data.at(i).at(10) - *tinit;
		float tmdiff = (data.at(i).at(1)*data.at(i).at(2) + data.at(i).at(3)*data.at(i).at(4))/ (A*hs) + Ts;
		long time_spot = data.at(i).at(0) - inittime;
		time_spot = time_spot / 10;
		float diffs = abs(tdiff - tmdiff);
		Tloss->SetBinContent(time_spot, tmdiff);
		TGain->SetBinContent(time_spot, tdiff);
		TDiff->SetBinContent(time_spot, tdiff);
	}
	std::vector<TH1F*> Temps(3);
	Temps.at(0) = Tloss;
	Temps.at(1) = TGain;
	Temps.at(2) = TDiff;
	Tloss->Delete();
	TGain->Delete();
	tinit->delptr();
	return Temps;
}
void SCT_read::scan_dat_file(fstream* inf, TFile* f, int boardnumber, std::string time_code, bool aon, bool lastof, int rn, bool rad)
{
	std::string readout = "";
	int wc = 0, line = 0, tline = 0;
	bool hasread = false;
	std::vector <std::string> columns;
	std::vector <TH1F*> hists;
	std::vector <std::vector <float> > vals;
	std::vector <float> lvec;

	bool esccondition = inf->is_open();
	//std::cout << "Ready to begin looping, Start Signal is " <<time_code << std::endl;
	while (esccondition) {
		while (std::getline(*inf, readout))
		{
			tline++;
			//std::cout << tline << std::endl;
			if (readout.find(time_code.c_str()) == std::string::npos &&wc == 0 && hasread == 0) std::cout << '\r' << "Looking for value at line " << tline << std::flush;
			if (readout.find(time_code.c_str()) != std::string::npos) {
				wc = 1;
				hasread = true;
				std::cout << "\n Start signal " << time_code << " sent" << "\n Line is " << tline << "\n Line reads " << readout << std::endl;
			}
			if (readout.find("Start") != std::string::npos && readout.find(time_code.c_str()) == std::string::npos) wc = 0;

			if (wc == 1)
			{

				line++;
				columns.clear();
				std::istringstream temp(readout);
				std::string temp2;
				while (std::getline(temp, temp2, ' ')) columns.push_back(temp2);
				for (int i = 0; i < columns.size(); i++) {
					try {
						lvec.push_back(std::stof(columns.at(i).c_str()));
					}
					catch (const std::exception& e) {
						continue;

					}
				}
				if (lvec.size() <14 || columns.size() == 0) {
					continue;
				}
				vals.push_back(lvec);
				lvec.clear();

			}
			if (wc == 0 && hasread == true || inf->eof())esccondition = false;
		}
	}
	std::cout << " \n Having escaped the carven of the terrible while loop, our hero travels forth" << std::endl;
	hists.push_back(new TH1F(("vd" + std::to_string(rn)).c_str(), "Voltage in Digital Channel; Time (s); Voltage (V)", vals.size(), 0, 10 * vals.size()));

	for (int i = 0; i < vals.size() - 1; i++) {
		try {
			vals.at(i).at(0) = vals.at(i).at(0) - vals.at(0).at(0);
			hists.at(0)->SetBinContent(i, vals.at(i).at(1));
		}

		catch (std::exception& e) {
			continue;
		}
	}
	if (boardnumber == 48) std::cout << "Sample Current " << vals.at(10).at(2) << std::endl;
	hists.push_back(new TH1F(("id" + std::to_string(rn)).c_str(), "Current in Digital Channel; Time (s); Current (mA)", vals.size(), 0, 10 * vals.size()));
	for (int i = 0; i < vals.size() - 1; i++) {
		try { hists.at(1)->SetBinContent(i, 1000 * vals.at(i).at(2)); }
		catch (...) {
			continue;
		}
	}
	hists.push_back(new TH1F(("pd" + std::to_string(rn)).c_str(), "Power in Digital Channel; Time (s); Power(mW)", vals.size(), 0, 10 * vals.size()));
	for (int i = 0; i < vals.size(); i++) {
		try {
			hists.at(2)->SetBinContent(i, vals.at(i).at(1)*vals.at(i).at(2) * 1000);
		}
		catch (std::exception& e) { continue; }
	}
	if (aon = true) {
		hists.push_back(new TH1F(("va" + std::to_string(rn)).c_str(), "Voltage in Analog Channel; Time (s); Voltage (V)", vals.size(), 0, 10 * vals.size()));

		for (int i = 0; i < vals.size() - 1; i++) {
			try {
				vals.at(i).at(0) = vals.at(i).at(0) - vals.at(0).at(0);
				hists.at(3)->SetBinContent(i, vals.at(i).at(3));
			}

			catch (std::exception& e) {
				continue;
			}
		}
		hists.push_back(new TH1F(("ia" + std::to_string(rn)).c_str(), "Current in Analog Channel; Time (s); Current (mA)", vals.size(), 0, 10 * vals.size()));
		for (int i = 0; i < vals.size() - 1; i++) {
			try { hists.at(4)->SetBinContent(i, 1000 * vals.at(i).at(4)); }
			catch (...) {
				continue;
			}
		}
		hists.push_back(new TH1F(("pa" + std::to_string(rn)).c_str(), "Power in Analog Channel; Time (s); Power(mW)", vals.size(), 0, 10 * vals.size()));
		for (int i = 0; i < vals.size(); i++) {
			try {
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
	tempgraphs.push_back(new TGraph(V, T));
	for (int i = 0; i < hists.size(); i++) hists.at(i)->Write();
	//***NOTE: PD & PA have older variable names from attempting to plot Powers instead of currents, Too lazy to change ***
	PAall.push_back(hists.at(4)); //analog current
	PDall.push_back(hists.at(1)); //Digital current
	if (rad == 0) {
		std::vector<std::vector<float>> av = time_averging(vals);

		try {

			for (int i = 0; i < av.size(); i++) {
				for (int j = 0; j < av.at(i).size(); j++) avg_hists.at(i)->Fill(av.at(i).at(j));
			}
		}
		catch (std::exception& e) { std::cout << "Unable to perform time averaging on run " << rn << "\n Returned error " << e.what() << std::endl; }
	}
	if (rad == 1) {
		std::vector<TH1F*> Tempdiffs = SCT_read::Temp_loss(vals, , vals.at(0).at(0));
		if (lastof == true) Tempdiffs.Write();
	}
	av.~vector();
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
	//avg_hists.~vector();
	//get rid of memory leak possiblity
}
std::vector<std::vector<float>> SCT_read::time_averging(std::vector<std::vector<float>> data)
{
	std::vector<float> ta_vd, ta_va, ta_ad, ta_aa;
	float vd = 0, va = 0, ad = 0, aa = 0, n = 0;
	std::cout << "Input vector has size " << data.size() << " with " << data.at(0).size() << " columns" << std::endl;
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
	std::vector <std::vector<float>> avgs(4);
	avgs.at(0) = ta_vd;
	avgs.at(1) = ta_ad;
	avgs.at(2) = ta_va;
	avgs.at(3) = ta_aa;
	std::cout << "Readout vectors have size " << ta_vd.size() << std::endl;
	return avgs;
}