#pragma once 

#include "field_builder.hpp"

#include <vector>


struct SimulationResult{

    std::vector<double> time_s;
    std::vector<double> k_grid;
    std::vector<double> valence_band;
    std::vector<double> conduction_band;

    FieldSamples fields;


};