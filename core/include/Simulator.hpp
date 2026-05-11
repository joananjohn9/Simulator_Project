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
          
           

};