#ifndef TIMEPARAM_H
#define TIMEPARAM_H

// class containing information required for VUV and visible timings parameterisation in SBND
/*
VUV:
   	landau + exponential parameterisation of VUV photon arrival times
	The initial sampling of generated parameterisations to allow drawing random numbers from the distribution is slow. To improve efficiency, the parameterisations
	are discretised in distance allowing each generated and sampled parameterisation to be stored in a vector so they to be used whenever subsequently required
	without the need for regenerating. With 1cm step sizes in the discretisation this method is of order 100 times faster than original method, and the discrepancy
	created is on sub-nanosecond scale. 

VIS:
	The shortest path for the reflected is calculated considering refractive indices of each component in LAr giving the earliest arrival time.
	The distribution of the arrival times is taken initially as the distribution of the VUV arrival times to the cathode plane, then these are smeared relative to
	the fastest arrival time with an exponential distribution. This leaves the fastest arrival time unchanged and approximates the overall distribution in the 
	visible light arrival times seen in full simulation. 
*/	

#include <vector>

#include "TF1.h"
#include "TVector3.h"

class timeparam {

private:
	// *************************************************************************************************
	// Discretisation of VUV time parameterisations
	// *************************************************************************************************
	// discretisation step size in cm, set in libraryanalyze_light_histo.h
	double step_size;
	// maximum distance in cm parameterisations are generated for (only generated when required to prevent long initial loading time)
	double d_max = 2500;
	// vector containing generated VUV timing parameterisations
	std::vector<TF1> VUV_timing;
	// vector containing min and max range VUV timing parameterisations are sampled to
	std::vector<double> VUV_max;
	std::vector<double> VUV_min;
	
	// *************************************************************************************************
	// Definition of the parameters for VUV timings parameterisation
	// *************************************************************************************************
	// Parameters of the Landau + Exponential (<= 300 cm) and Landau (> 300 cm) models
	// Landau parameters 
	std::vector<double> vDistances_all = {62.5, 87.5, 112.5, 137.5, 162.5, 187.5, 212.5, 237.5, 262.5, 287.5, 312.5, 337.5, 362.5, 387.5, 412.5, 437.5, 462.5, 487.5, 512.5, 537.5, 562.5, 587.5, 612.5, 637.5, 662.5, 687.5, 712.5, 737.5, 762.5, 787.5, 812.5, 837.5, 862.5, 887.5, 912.5, 937.5, 962.5, 987.5, 1012.5};
	std::vector<double> vNorm_over_entries = {1.46739, 0.880294, 0.571107, 0.344403, 0.261388, 0.20993, 0.170956, 0.152152, 0.136094, 0.120939, 0.109945, 0.10561, 0.0958589, 0.0904341, 0.0839861, 0.0803693, 0.0761224, 0.0716448, 0.0689223, 0.0668696, 0.0635322, 0.0616896, 0.0600554, 0.0577425, 0.0561707, 0.0539962, 0.0517729, 0.050872, 0.0488119, 0.0469635, 0.0466058, 0.0453283, 0.04366, 0.0425648, 0.0411353, 0.0396493, 0.0388351, 0.0383864, 0.0385728};
	std::vector<double> vMpv = {6.05668, 9.05062, 12.3438, 17.1553, 21.9225, 26.9042, 33.1595, 37.1543, 43.6424, 49.9016, 55.7976, 59.7093, 66.7296, 72.5434, 78.6925, 84.1388, 90.0175, 97.1053, 101.911, 107.799, 113.748, 119.527, 124.167, 130.438, 134.988, 141.886, 147.848, 153.056, 160.653, 168.234, 171.821, 178.68, 186.341, 193.222, 202.61, 212.357, 222.014, 232.012, 235.169};
	std::vector<double> vWidth = {0.977654, 1.57296, 2.42236, 4.15371, 5.76189, 7.49866, 9.87714, 10.9349, 13.4434, 15.5028, 19.8005, 20.6927, 22.9557, 24.5475, 26.2879, 27.7879, 29.296, 31.1889, 32.5755, 33.5919, 35.4932, 36.7544, 37.8536, 39.5658, 40.8131, 42.6414, 44.6617, 45.9231, 48.2682, 50.799, 51.9537, 54.1583, 57.8934, 61.4115, 65.4056, 70.7218, 74.6922, 79.6439, 81.8717};
	// Exponential parameters
	std::vector<double> vDistances = {55, 65, 75, 85, 95, 105, 115, 125, 135, 145, 155, 165, 175, 185, 195, 205, 215, 225, 235, 245, 255, 265, 275, 285, 295};
	std::vector<double> vSlope = {-0.0885849, -0.0688936, -0.0519349, -0.0481695, -0.0408145, -0.0359584, -0.0341716, -0.0302902, -0.0282722, -0.026348, -0.0240983, -0.023172, -0.0220861, -0.0207058, -0.0197934, -0.0193794, -0.0182472, -0.0175204, -0.0173492, -0.0163832, -0.0164717, -0.0160823,-0.0153199, -0.0149124, -0.0142298};
	// Line fit to the profiles of the ratio between the normalization parameters of the exponential and the landau functions vs distance (<= 300 cm)
	// Fits made to profiles in three different offset angle bins [0, 30deg], [30deg, 60deg] and [60deg, 90 deg]
	const double Expo_over_Landau_norm[3][2] = { {-4.44152e-02, 1.25321e-03}, {-2.74406e-02, 1.27240e-03}, {3.48944e-02, 1.27080e-03} };
	// VUV group velocity
	const double vuv_vgroup_mean = 10.13;//cm/ns
	const double vuv_vgroup_max = 15.;//cm/ns
	// Distance from "Landau + Expo" -> "Single Landau" model
	const double inflexion_point_distance = 300.; //cm

	// *************************************************************************************************
	// Definition of the parameters for vis timings calculation
	// *************************************************************************************************
	// velocity of visible light
	const double vis_vmean = 23.99; // cm/ns
	const double vis_vrms = 0.07508; // cm/ns
	// x-position of cathode foils, matching position of foils in gdml file - this needs changing if using different geometry
	const double cathode_plane_depth = 0;
	// refractive indices in LAr
	double n_LAr_VUV = 2.632;       // effective index due to group vel.
	double n_LAr_vis = 1.23;
	double c_LAr_VUV = 0.12;        		// m/s
	double c_LAr_vis = 0.29979/n_LAr_vis; 	// m/s

	// smearing parameters:
	// smearing parameters are calculated for four fastest path off-set angle alpha bins: [0,10], [10,20], [20,30], [30,40] deg
	// larger angles cannot be achieved with events in the middle of the detector, though could be possible from edged (I shall write a script to acertain maximum possible alpha for SBND geometry)
	// as failsafe, if a larger alpha than 40 deg is found, defaults to [30,40] degree bin ---- effectively [30,90] deg bin
	vector<double> refl_vdistances = {25,40,60,80,100,120,140,160,180,200};
	vector<vector<double>> cut_off_bins = { {220.559,239.781,261.162,280.35,297.07,305.335,316.825,321.326,323.348,321.925},
							{232.93,252.297,272.975,290.81,306.027,316.828,325.465,329.651,334.128,331.066},
							{248.255,267.175,287.91,303.054,315.817,327.364,336.522,337.163,339.698,336.158},
							{258.65,275.184,292.176,302.28,322.826,327.364,336.522,337.163,339.698,336.158}		// last 5 bins have no data, uses values from previous bin
						};
	vector<vector<double>> tau_bins = { {9.65303,7.80435,4.45676,3.2075,2.40353,1.7977,1.337,0.799091,0.289916,0.00731707},
							{11.7346,9.58924,5.45275,3.68209,2.69315,2.08817,1.48642,0.95226,0.411622,0.0406915},
							{13.3461,11.8424,5.87778,4.09909,3.00104,2.22286,1.54286,1.05221,0.457658,0.0435644},
							{14.2143,13.7812,6.58852,3.65,3.2,2.22286,1.54286,1.05221,0.457658,0.0435644}		// last 5 bins have no data, uses values from previous alpha bin
						};

	double pi =  3.141592653589793;


public:
	// constructor
	timeparam(double step);

	// destructor
	~timeparam(){std::cout << "Deleting timing parameterisations.\n";};

	// parameterisation generation function
	// generates TF1 and samples within required range for given distance (index), saving this object for later use
	void generateparam(int index);

    // VUV arrival times calculation function
    // returns an arrival time for each photon randomly drawn from the pre-sampled parameterisations 
    std::vector<double> getVUVTime(double distance, int number_photons);

    // VIS arrival times calculation function
    // returns an arrival time for each photon randomly drawn from the pre-sampled VUV parameterisations and smeared to approximate visible light distribution
    std::vector<double> getVisTime(TVector3 ScintPoint, TVector3 OpDetPoint, int number_photons);

    double getVUVmin(int index);
};

#endif