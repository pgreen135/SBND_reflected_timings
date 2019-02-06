// SBND
// generates reflected timings distribution using parameterisation input x,y,z and optical channel
// centre coordinates: (x ,0, 250)

#include "timeparam.h"
#include "timeparamservices.h"

void example_driver (double x_pos, double y_pos, double z_pos, int OpChannel, int nphotons) {

	// initialise time parameterisation class
	timeparam arrival_times(1);

	// load detector positions
	vector<vector<double>> myfile_data;
	cout << "Loading Photon Detector positions...\n";
    ifstream myfile;
    myfile.open("sbnd_optical_channel_positions.txt"); 
    if(myfile.is_open()) {cout << "File opened successfully" << endl; }
    while(!myfile.eof()) {
        double num_pmt, x_pmt, y_pmt, z_pmt;
        if(myfile >> num_pmt >> x_pmt >> y_pmt >> z_pmt) {
            vector<double> line_data({num_pmt, x_pmt, y_pmt, z_pmt});
            myfile_data.push_back(line_data);
        }
            else{break; }
    }
    myfile.close();
    cout << "Positions Loaded!\n\n";

    // calculate reflected timing distribution
    TVector3 ScintPoint(x_pos,y_pos,z_pos);
	TVector3 OpDetPoint(myfile_data.at(OpChannel).at(1),myfile_data.at(OpChannel).at(2),myfile_data.at(OpChannel).at(3));

	vector<double> transport_times = arrival_times.getVisTime(ScintPoint, OpDetPoint, nphotons);

	// create histogram of distribution
	double bin_size = 2.5; 		//	size of bins in ns
	int number_bins = 200;		//	number bins
	TH1F *mch = new TH1F("mch","", number_bins, 0, number_bins*bin_size);
	for (auto it = transport_times.begin(); it != transport_times.end(); ++it) {
		mch->Fill(*it);
	}
	mch->Scale(1./mch->Integral());

	// plot distribution
	TCanvas *c1 = new TCanvas("c1","",200,10,1080,1080);
    c1->SetGrid();
    gStyle->SetOptStat(0);

    mch->SetLineColor(4);
    mch->SetLineWidth(2);
    
    mch->GetXaxis()->SetTitle("transport time (ns)");

    stringstream ss_title;
    ss_title << "Visible light: scintillation point = (" << x_pos << ", " << y_pos << ", " << z_pos << "), optical channel = " << OpChannel;
    string str_title = ss_title.str();
    const char* title = str_title.c_str();
    mch->SetTitle(title);

   	mch->Draw("HIST");
}