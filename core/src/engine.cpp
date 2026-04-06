#include "engine.hpp"

#include <filesystem>
#include <fstream>
#include <stdexcept>

namespace fs = std::filesystem;

Engine::Engine(const std::string& input_json,
               const std::string& output_dir)
    : input_json_(input_json),
      output_dir_(output_dir),
      sim_output_dir_(output_dir)
{
    setup_paths();
    load_config();
    run();
}

// ------- Set up Path --------
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
}

// --------- Load configuration --------
void Engine::load_config()
{
    config_ = load_engine_config(input_json_.string());
}

// ------- RUN --------
void Engine::run()
{
    const fs::path meta_path = sim_output_dir_ / "meta.json";
    std::ofstream meta(meta_path, std::ios::out | std::ios::trunc);

    if (!meta) {
        throw std::runtime_error(
            "Failed to open meta.json for writing: " + meta_path.string()
        );
    }

    meta << "{\n"
         << "  \"schema_version\": \"" << config_.schema_version << "\",\n"
         << "  \"model\": \"" << config_.model << "\",\n"
         << "  \"k_points\": " << config_.grid.k_points << ",\n"
         << "  \"dt_fs\": " << config_.grid.dt_fs;

    if (config_.optical.has_value()) {
        meta << ",\n"
             << "  \"optical_id\": \"" << config_.optical->id << "\"";
    }

    if (config_.dc.has_value()) {
        meta << ",\n"
             << "  \"dc_id\": \"" << config_.dc->id << "\"";
    }

    meta << "\n}\n";
}