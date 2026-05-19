#pragma once 

#include "config.hpp"
#include "grid.hpp"
#include "SimulationResult.hpp"
#include "SBEState.hpp"


#include <cstddef>
#include <complex>




struct Macroscopic_observables{

    std::complex<double> P;
    std::complex<double> n;
    std::complex<double> J;

};


class Simulator{

    public:

        //constructor 
        explicit Simulator(const EngineConfig& config);
        SimulationResult run();
    
    private:

        // Data members
        const EngineConfig& config_;
        Grid grid_;
        Macroscopic_observables obs;
        

        //Helper-Private Functions
        void validate_required_fields() const;        
        void reserve_macroscopic_outputs(SimulationResult& result) const;
        void append_zero_observables(SimulationResult& result) const;
        void append_observables(SimulationResult& result, const Macroscopic_observables& obs) const;


        SimulationResult initialize_result();      
        SBEState make_initial_state(std::size_t k_points) const; 
        Macroscopic_observables compute_macroscopic_observables(const SBEState& state) const;
           
};