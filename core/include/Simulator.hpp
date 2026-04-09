#include "config.hpp"
#include "SimulationResult.hpp"
#include <vector>

class Simulator{

    public:

        //constructor 
        explicit Simulator(const EngineConfig& config);
        SimulationResult run();
    
    private:
        const EngineConfig& config_;
        std::vector<double> build_time_grid() const;
        std::vector<double> build_k_grid() const;   
           

}