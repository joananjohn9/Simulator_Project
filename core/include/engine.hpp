#pragma once

#include <filesystem>

#include "config.hpp"

namespace fs = std::filesystem;

class Engine {
public:
    Engine(const fs::path& input_json_path,
           const fs::path& output_dir_path);

    void run();

private:
    fs::path input_json_;
    fs::path output_dir_;
    fs::path sim_output_dir_;

    EngineConfig config_;

    void setup_paths();
    void load_config();
};