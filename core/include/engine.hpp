#pragma once

#include <filesystem>

#include "config_loader.hpp"

namespace fs = std::filesystem;

class Engine {
public:
    // Constructor: takes input JSON path and output directory
    Engine(const fs::path& input_json_path,
           const fs::path& output_dir_path);

    // Executes the simulation and writes outputs
    void run();

private:
    // ----- Paths -----
    fs::path input_json_;   // path to input.json
    fs::path output_dir_;   // actual simulation output directory

    // ----- Configuration -----
    EngineConfig config_;

    // ----- Internal helpers -----
    void setup_paths();
    void load_config();
};