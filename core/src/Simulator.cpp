#include "Simulator.hpp"
#include "grid.hpp"

#include <cmath>
#include <stdexcept>
#include <vector>
#include <iostream> 

Simulator::Simulator(const EngineConfig& config)
    : config_(config),
    grid_(config)

{

    
    
    

}


SimulationResult Simulator::run(){

    SimulationResult result;

    result.time_fs = grid_.time_fs();
    result.k_grid = grid_.k_grid();
    result.valence_band = grid_.valence_band();
    result.conduction_band = grid_.conduction_band();   
    
    return result;

}// End of run function
