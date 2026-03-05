#pragma once 

#include <string>
#include <vector>
#include <optional>
#include <unordered_map> 
#include <filesystem>

namespace fs = std::filesystem;

struct OrderExpansionConfig {

    bool enabled = false;
    int max_order = 3;
    
}; 

struct GridConfig{

    int k_points = 1000;
    double dt_fs;
    double t_start_fs;
    double t_end_fs;


};

struct CoulombConfig{
    bool enabled;
    std::string screening = "none";
};

struct InteractionConfig{

    Coulomb coulomb;

   
};



struct FieldConfig {
    
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
    
    std::string format;

    bool save_k_resolved = false;
    bool save_macroscopic = true;

    std::vector<std::string> observables;
};



struct EngineConfig{

    std::string schema_version;
    std::string model = "sbe";

    OrderExpansionConfig order_expansion;
    GridConfig grid;
    
    std::optional<InteractionConfig>  interactions;

    std::vector<FieldConfig> fields;

    OutputConfig output;

};