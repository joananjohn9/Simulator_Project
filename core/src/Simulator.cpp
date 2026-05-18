#include "Simulator.hpp"
#include "SBEState.hpp"
#include "SimulationResult.hpp"
#include "field_builder.hpp"
#include "grid.hpp"


#include <cstddef>
#include <vector>
#include <stdexcept>




Simulator::Simulator(const EngineConfig& config)
    : config_(config),
    grid_(config)

{

    
    
    

}


SimulationResult Simulator::run(){

    SimulationResult result;

    result.time_s = grid_.time_s();
    result.k_grid = grid_.k_grid();
    result.valence_band = grid_.valence_band();
    result.conduction_band = grid_.conduction_band();   

    validate_required_fields();

    const double dt_s = config_.grid_config.dt_fs * 1e-15;

    result.fields = build_field_samples(
        result.time_s,
        dt_s,
        config_.optical_config.value(),
        config_.dc_config.value()
    );

    SBEState state = make_initial_state(config_.grid_config.k_points);

    reserve_macroscopic_outputs(result);

    for(std::size_t ti=0; ti < result.time_s.size(); ti++)
    {
        append_zero_observables(result);
    }



    
    
    return result;

}// End of run function



void Simulator::validate_required_fields() const {

    if(!config_.optical_config.has_value())
    {
        throw std::runtime_error(
            "Simulator::run : Missing optical field config"
        );
    }

    if(!config_.dc_config.has_value())
    {
        throw std::runtime_error("Simulator:: run Missing dc field config");
    }
}// end of validate required field function 



SBEState Simulator::make_initial_state(std::size_t k_points) const{

    SBEState state;

    state.p_k.assign(k_points,std::complex<double> (0.0,0.0));
    state.n_k.assign(k_points,std::complex<double> (0.0,0.0));

    return state;

}

void Simulator::reserve_macroscopic_outputs(SimulationResult& result) const{

    std::size_t n_t = result.time_s.size();

    result.P_real.reserve(n_t);
    result.P_imag.reserve(n_t);

    result.n_real.reserve(n_t);
    result.n_imag.reserve(n_t);

    result.J_real.reserve(n_t);
    result.J_imag.reserve(n_t);

}


void Simulator::append_zero_observables(SimulationResult& result) const
{
    result.P_real.push_back(0.0);
    result.P_imag.push_back(0.0);

    result.n_real.push_back(0.0);
    result.n_imag.push_back(0.0);

    result.J_real.push_back(0.0);
    result.J_imag.push_back(0.0);
}