#include "Simulator.hpp"
#include "field_builder.hpp"
#include "grid.hpp"


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
}