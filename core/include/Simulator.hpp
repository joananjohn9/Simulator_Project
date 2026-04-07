#include "config.hpp"
#include "SimulationResult.hpp"
#include <vector>

class Simulator{

    public:
        explicit Simulator(const EngineConfig& config);
        Simulation run();
    
    private:
        const EngineConfig& config_;

        std::vector<double> build_time_grid() const;
        std::vector<double> build_place_holder_polarization(
            const std::vector<double>& time_fs
        ) const;

}