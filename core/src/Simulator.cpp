#include "Simulator.hpp"
#include "grid.cpp"

#include <cmath>
#include <stdexcept>
#include <vector>

Simulator::Simulator(const EngineConfig& config)
    : config_(config)

{
    Grid::Grid(config_);

}


