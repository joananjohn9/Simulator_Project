#pragma once 

#include <vector>


struct SimulationResult{

    std::vector<double> time_fs;
    std::vector<double> k_grid;
    std::vector<double> valence_band;
    std::vector<double> conduction_band;


};