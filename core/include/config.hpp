#pragma once 

#include <string>
#include <vector>
#include <optional>



struct ModelConfig{

    std::schema_version = "v.0.1";
    std::string type = "sbe_1d";
    double lattice_constant_A = 0.0;
    double E_gap_eV = 0.0;

};

struct OrderExpansionConfig {

    bool enabled = false;
    int max_order = 0;
    
}; 

struct SimulationConfig{

    ModelConfig model_config;
    OrderExpansionConfig order_expansion_config;

};

struct GridConfig{

    int k_points = 0;
    double dt_fs = 0.0;
    double t_start_fs = 0.0;
    double t_end_fs = 0.0 ;


};

struct CoulombConfig{
    bool enabled = false;
    std::string screening = "none";
};

struct InteractionConfig{

    CoulombConfig coulomb;

   
};



struct FieldConfig {
    
    std::string id;
    std::string kind;
    std::string pulse_type; 

    double amplitude = 0.0;

    //For Optical fields
    std::optional<double> frequency;
    std::optional<double> duration_fs;
    std::optional<double> t0_fs;


    //For DC fields

    std::optional<double> t_on_fs;
    std::optional<double> t_off_fs;

};


struct OutputConfig{
    
    std::string format = "binary";

    bool save_k_resolved = false;
    bool save_macroscopic = true;

    std::vector<std::string> observables;
};



struct EngineConfig{

    SimulationConfig simulation_config;

    OrderExpansionConfig order_expansion_config;
    GridConfig grid_config;
    
    std::optional<InteractionConfig>  interactions_config;

    std::optional<FieldConfig> optical_config;
    std::optional<FieldConfig> dc_config;

    OutputConfig output_config;

};