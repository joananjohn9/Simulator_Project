#pragma once 

#include "config.hpp"
#include "grid.hpp"
#include "SimulationResult.hpp"
#include "SBEState.hpp"

#include <vector>
#include <cstddef>


class Simulator{

    public:

        //constructor 
        explicit Simulator(const EngineConfig& config);
        SimulationResult run();
    
    private:

        // Data members
        const EngineConfig& config_;
        Grid grid_;
        

        //Helper-Private Functions
        void validate_required_fields() const;
        SBEState make_initial_state(std::size_t k_points) const; 
        void reserve_macroscopic_outputs(SimulationResult& result) const;
        void append_zero_observables(SimulationResult& result) const;
        

          
           

};