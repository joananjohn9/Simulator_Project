#include "engine.hpp"
#include "config_loader.hpp"
#include "Simulator.hpp"

#include <fstream>
#include <stdexcept>

namespace fs = std::filesystem;

Engine::Engine(const fs::path& input_json_path,
               const fs::path& output_dir_path)
    : input_json_(input_json_path),
      output_dir_(output_dir_path),
      sim_output_dir_(output_dir_path / "sim_output")
{
    setup_paths();
    load_config();
    
    
}

void Engine::setup_paths()
{
    if (!fs::exists(input_json_)) {
        throw std::runtime_error(
            "Input JSON file does not exist: " + input_json_.string()
        );
    }

    if (!fs::is_regular_file(input_json_)) {
        throw std::runtime_error(
            "Input JSON path is not a regular file: " + input_json_.string()
        );
    }

    if (!fs::exists(output_dir_)) {
        fs::create_directories(output_dir_);
    }

    if (!fs::is_directory(output_dir_)) {
        throw std::runtime_error(
            "Output path is not a directory: " + output_dir_.string()
        );
    }

    fs::create_directories(sim_output_dir_);
}

void Engine::load_config()
{
    config_ = load_engine_config(input_json_.string());
}



void Engine::run()
{
   
    
    
    Simulator bloch_simulator(config_);
    
}