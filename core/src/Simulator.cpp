#include "Simulator.hpp"

#include <cmath>
#include <stdexcept>
#include <vector>

Simulator::Simulator(const EngineConfig& config)
    : config_(config)

{
    build_time_grid();

}

std::vector<double> Simulator :: build_time_grid() const
{
    double t_start = config_.grid.t_start_fs;
    double t_end = config_.grid.t_end_fs;
    double dt = config_.grid.dt_fs;

    if (dt<=0.0){

        throw runtime_error("dt_fs must be positive");
    }

    if (t_end < t_start){
        throw runtime_error("t_end_fs must be greater than t_start_fs");
    }

    const std::size_t n_points = 
        static_cast <std::size_t>(std::floor(t_end - t_start)/dt + 0.5) + 1;

    
    std::vector<double> time_fs;
    time_fs.reserve(n_points);

    for(std::size_t i=0; i < n_points; i++){

        time_fs.push_back(t_start + static_cast<double> i*dt); 
    

    }

    return time_fs;


}

std::vector<double> Simulator:: build_k_grid() const {

    double k_points = config_.grid.k_points;

    for(int )

}