#pragma once 

#include "config.hpp"
#include "grid.hpp"
#include "SimulationResult.hpp"
#include <vector>

class Simulator{

    public:

        //constructor 
        explicit Simulator(const EngineConfig& config);
        SimulationResult run();
    
    private:
        const EngineConfig& config_;
        Grid grid_;
        
        void validate_required_fields() const;
          
           

};