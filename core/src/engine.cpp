#include "engine.hpp"

#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <iostream>

namespace fs = std::filesystem;

Engine::Engine(const std::string& input_json,
               const std::string& output_dir)
    : input_json_(input_json),
      output_dir_(output_dir)
{
    const fs::path input_json_path{input_json_};
    const fs::path output_dir_path{output_dir_};

    // --- Contract A (v0.1): input.json must exist and be a regular file
    if (!fs::exists(input_json_path)) {
        throw std::runtime_error(
            "Input JSON does not exist at: " + input_json_path.string()
        );
    }
    if (!fs::is_regular_file(input_json_path)) {
        throw std::runtime_error(
            "Input JSON is not a regular file: " + input_json_path.string()
        );
    }

    // --- output_dir must be a directory (create if missing)
    if (fs::exists(output_dir_path) && !fs::is_directory(output_dir_path)) {
        throw std::runtime_error(
            "Output path exists but is not a directory: " + output_dir_path.string()
        );
    }

    if (!fs::exists(output_dir_path)) {
        std::cout << "Output directory does not exist at: "
                  << output_dir_path << "\n"
                  << "Creating output directory...\n";
    }

    fs::create_directories(output_dir_path);

    // --- Create sim_output inside output_dir
    sim_output_dir_ = output_dir_path / "sim_output";
    fs::create_directories(sim_output_dir_);
}

void Engine::run()
{
    const fs::path meta_path = sim_output_dir_ / "meta.json";
    std::ofstream meta(meta_path, std::ios::out | std::ios::trunc);
    if (!meta) {
        throw std::runtime_error("Failed to open meta.json for writing: " + meta_path.string());
    }

    meta << "{\n"
         << "  \"schema_version\": \"0.1\",\n"
         << "  \"status\": \"dummy run\"\n"
         << "}\n";
}
