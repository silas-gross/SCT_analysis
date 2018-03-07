#pragma once
#include <stdio.h>
#include "C:/root_v5.34.36/include/TH1F.h"
#include "C:/root_v5.34.36/include/TGraph.h"
#include "C:/root_v5.34.36/include/TFile.h"
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
class SCT_read {
public: 
	void SCT_read::scan_dat_file(std::fstream* inf, TFile* f, int boardnumber, std::string time_code, bool aon, bool lastof, int rn);
	void SCT_read::scan_irrad_dat_file(std::fstream* inf, TFile* f, int boardnumber);
private:
	std::vector<TH1F*> SCT_read::TempLoss(std::vector<std::vector<float>> data, std::string board_label, const long init_time);
	std::vector<std::vector<float>> SCT_read::time_averging(std::vector<std::vector<float>> data);

}

