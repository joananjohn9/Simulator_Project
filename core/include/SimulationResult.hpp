#pragma once 

#include "field_builder.hpp"

#include <vector>


struct SimulationResult{

    // Grid values
    std::vector<double> time_s;
    std::vector<double> k_grid;
    std::vector<double> valence_band;
    std::vector<double> conduction_band;

    // Field Values
    FieldSamples fields;

    // Simulation Result values
    std::vector<double> P_real;
    std::vector<double> P_imag;

    std::vector<double> n_real;
    std::vector<double> n_imag;

    std::vector<double> J_real;
    std::vector<double> J_imag;
    



};