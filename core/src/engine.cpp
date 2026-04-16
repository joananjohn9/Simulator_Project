#include "engine.hpp"
#include "config_loader.hpp"

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
    const fs::path meta_path = sim_output_dir_ / "meta.json";
    std::ofstream meta(meta_path, std::ios::out | std::ios::trunc);

    if (!meta) {
        throw std::runtime_error(
            "Failed to open meta.json for writing: " + meta_path.string()
        );
    }

    meta << "{\n"
        << "  \"schema_version\": \"" << config_.schema_version << "\",\n"
        << "  \"model_type\": \""
        << config_.simulation_config.model_config.type << "\",\n"
        << "  \"lattice_constant_A\": "
        << config_.simulation_config.model_config.lattice_constant_A << ",\n"
        << "  \"E_gap_eV\": "
        << config_.simulation_config.model_config.E_gap_eV << ",\n"
        << "  \"deltaE_c_eV\": "
        << config_.simulation_config.model_config.deltaE_c_eV << ",\n"
        << "  \"deltaE_v_eV\": "
        << config_.simulation_config.model_config.deltaE_v_eV << ",\n"
        << "  \"k_points\": " << config_.grid_config.k_points << ",\n"
        << "  \"dt_fs\": " << config_.grid_config.dt_fs << ",\n"
        << "  \"t_start_fs\": " << config_.grid_config.t_start_fs << ",\n"
        << "  \"t_end_fs\": " << config_.grid_config.t_end_fs << ",\n"
        << "  \"order_expansion_enabled\": "
        << (config_.simulation_config.order_expansion_config.enabled ? "true" : "false") << ",\n"
        << "  \"max_order\": "
       
        << "  \"n_k_points\": " << config_.grid_config.k_points;

    if (config_.optical_config.has_value()) {
        meta << ",\n"
             << "  \"optical_id\": \"" << config_.optical_config->id << "\"";
    }

    if (config_.dc_config.has_value()) {
        meta << ",\n"
             << "  \"dc_id\": \"" << config_.dc_config->id << "\"";
    }

    meta << ",\n"
         << "  \"status\": \"dummy run\"\n"
         << "}\n";
}