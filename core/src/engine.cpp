#include "engine.hpp"
#include "SimulationResult.hpp"
#include "config_loader.hpp"
#include "Simulator.hpp"
#include "table_writer.hpp"
#include "nlohmann/json.hpp"

#include <fstream>
#include <stdexcept>

namespace fs = std::filesystem;

using json = nlohmann::json;

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

    SimulationResult result = bloch_simulator.run();



    write_results(result);
    write_meta();
}

void Engine::write_meta()
{
    const fs::path meta_path = sim_output_dir_ / "meta.json";

    json meta;

    meta["schema_version"] = config_.schema_version;
    meta["status"] = "grid_field_and_result_debug";

    meta["model"] = {
        {"type", config_.simulation_config.model_config.type},
        {"lattice_constant_A", config_.simulation_config.model_config.lattice_constant_A},
        {"E_gap_eV", config_.simulation_config.model_config.E_gap_eV},
        {"deltaE_c_eV", config_.simulation_config.model_config.deltaE_c_eV},
        {"deltaE_v_eV", config_.simulation_config.model_config.deltaE_v_eV},
        {"T_1_fs", config_.simulation_config.model_config.T1_fs},
        {"T_2_fs", config_.simulation_config.model_config.T2_fs},
        {"mu_e_A", config_.simulation_config.model_config.mu_e_A},
        {"coulomb_constant", config_.simulation_config.model_config.coulomb_constant}
    };

    meta["grid"] = {
        {"k_points", config_.grid_config.k_points},
        {"dt_fs", config_.grid_config.dt_fs},
        {"t_start_fs", config_.grid_config.t_start_fs},
        {"t_end_fs", config_.grid_config.t_end_fs}
    };

    meta["order_expansion"] = {
        {"enabled", config_.simulation_config.order_expansion_config.enabled},
        {"max_order", config_.simulation_config.order_expansion_config.max_order}
    };

    if (config_.optical_config.has_value()) {
        const auto& optical = config_.optical_config.value();

        meta["optical"] = {
            {"id", optical.id},
            {"pulse_type", optical.pulse_type},
            {"amplitude", optical.amplitude}
        };

        if (optical.frequency.has_value()) {
            meta["optical"]["frequency"] = optical.frequency.value();
        }

        if (optical.duration_fs.has_value()) {
            meta["optical"]["duration_fs"] = optical.duration_fs.value();
        }

        if (optical.t0_fs.has_value()) {
            meta["optical"]["t0_fs"] = optical.t0_fs.value();
        }
    }

    if (config_.dc_config.has_value()) {
        const auto& dc = config_.dc_config.value();

        meta["dc"] = {
            {"id", dc.id},
            {"pulse_type", dc.pulse_type},
            {"amplitude", dc.amplitude}
        };

        if (dc.t_on_fs.has_value()) {
            meta["dc"]["t_on_fs"] = dc.t_on_fs.value();
        }

        if (dc.t_off_fs.has_value()) {
            meta["dc"]["t_off_fs"] = dc.t_off_fs.value();
        }
    }

    meta["outputs"] = {
        "time_grid.dat",
        "k_grid.dat",
        "bands.dat",
        "optical_field.dat",
        "dc_field.dat",
        "P.dat",
        "n.dat",
        "J.dat"

    };

    std::ofstream out(meta_path, std::ios::out | std::ios::trunc);

    if (!out) {
        throw std::runtime_error(
            "Engine::write_meta: failed to open meta.json for writing: "
            + meta_path.string()
        );
    }

    out << meta.dump(2) << '\n';
}

void Engine::write_results(const SimulationResult& result)
{
    write_table_tsv(
        {
            {"time_s", result.time_s}
        },
        sim_output_dir_ / "time_grid.dat"
    );

    write_table_tsv(
        {
            {"k", result.k_grid}
        },
        sim_output_dir_ / "k_grid.dat"
    );

    write_table_tsv(
        {
            {"k", result.k_grid},
            {"E_v", result.valence_band},
            {"E_c", result.conduction_band}
        },
        sim_output_dir_ / "bands.dat"
    ); 
    
    write_table_tsv(
        {
            {"time_s", result.time_s},
            {"E_opt_V_per_m", result.fields.optical_t}

        }, 
        sim_output_dir_/"optical_field.dat"
    );

    write_table_tsv(
        {
            {"time_s", result.time_s},
            {"E_dc_V_per_m", result.fields.dc_t}
        },
        sim_output_dir_/"dc_field.dat"
    );

    write_table_tsv(
        {
            {"time_s", result.time_s},
            {"P_real", result.P_real},
            {"P_imag", result.P_imag}

        },
        sim_output_dir_/"P.dat"

    );

      write_table_tsv(
        {
            {"time_s", result.time_s},
            {"n_real", result.n_real},
            {"n_imag", result.n_imag}

        },
        sim_output_dir_/"n.dat"

    );

      write_table_tsv(
        {
            {"time_s", result.time_s},
            {"J_real", result.J_real},
            {"J_imag", result.J_imag}

        },
        sim_output_dir_/"J.dat"

    );

}