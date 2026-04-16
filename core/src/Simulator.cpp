#include "Simulator.hpp"
#include "grid.cpp"

#include <cmath>
#include <stdexcept>
#include <vector>

Simulator::Simulator(const EngineConfig& config)
    : config_(config)

{
    Grid grid(config_);

    
    std::cout << "time points: " << grid_data.time_fs().size() << '\n';
    std::cout << "k points: " << grid_data.k_grid().size() << '\n';

    if (!grid_data.time_fs().empty()) {
        std::cout << "t_start = " << grid_data.time_fs().front() << '\n';
        std::cout << "t_end   = " << grid_data.time_fs().back() << '\n';
    }

    if (!grid_data.k_grid().empty()) {
        std::cout << "k_first = " << grid_data.k_grid().front() << '\n';
        std::cout << "k_mid   = " << grid_data.k_grid()[grid_data.k_grid().size()/2] << '\n';
        std::cout << "k_last  = " << grid_data.k_grid().back() << '\n';
    }

    if (!grid_data.conduction_band().empty()) {
        std::cout << "Ec_first = " << grid_data.conduction_band().front() << '\n';
        std::cout << "Ec_mid   = " << grid_data.conduction_band()[grid_data.conduction_band().size()/2] << '\n';
        std::cout << "Ec_last  = " << grid_data.conduction_band().back() << '\n';
    }

    if (!grid_data.valence_band().empty()) {
        std::cout << "Ev_first = " << grid_data.valence_band().front() << '\n';
        std::cout << "Ev_mid   = " << grid_data.valence_band()[grid_data.valence_band().size()/2] << '\n';
        std::cout << "Ev_last  = " << grid_data.valence_band().back() << '\n';
    }

}

