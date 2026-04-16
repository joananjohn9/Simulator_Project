#include "grid_data.hpp"

#include <cmath>
#include <stdexcept>

GridData::GridData(const EngineConfig& config)
{
    build_time_grid(config.grid_config);
    build_k_grid(config.grid_config, config.simulation_config.model_config);
    build_bands(config.simulation_config.model_config);
}

const std::vector<double>& GridData::time_fs() const
{
    return time_fs_;
}

const std::vector<double>& GridData::k_grid() const
{
    return k_grid_;
}

const std::vector<double>& GridData::valence_band() const
{
    return valence_band_;
}

const std::vector<double>& GridData::conduction_band() const
{
    return conduction_band_;
}

const double& GridData::dk() const{
    return dk_;
}

const double& GridData::two_dk() const {
    return two_dk_;
}

void GridData::build_time_grid(const GridConfig& grid_config)
{
    const double t_start = grid_config.t_start_fs;
    const double t_end = grid_config.t_end_fs;
    const double dt = grid_config.dt_fs;

    if (t_end <= t_start) {
        throw std::runtime_error(
            "The end time of the simulation should be greater than the start time. Please check config.yaml file."
        );
    }

    if (dt <= 0.0) {
        throw std::runtime_error(
            "The time step (dt) of the simulation should be greater than zero. Please check config.yaml file."
        );
    }

    const std::size_t n_points =
        static_cast<std::size_t>(std::floor((t_end - t_start) / dt + 0.5)) + 1;

    time_fs_.clear();
    time_fs_.reserve(n_points);

    for (std::size_t i = 0; i < n_points; ++i) {
        time_fs_.push_back(t_start + static_cast<double>(i) * dt);
    }
}

void GridData::build_k_grid(const GridConfig& grid_config,
                            const ModelConfig& model_config)
{
    const int k_points = grid_config.k_points;
    const double a = model_config.lattice_constant_A;

    const double pi = std::acos(-1.0);

    // Initializing dk and two_dk 
    dk_ = 2.0 *pi/k_points;
    two_dk_ = 2.0*dk_;

    if (k_points <= 0) {
        throw std::runtime_error(
            "The value of k_points should be greater than 0. Please check config.yaml file."
        );
    }

    // Ensure k_points is odd for a centered symmetric grid.
    if (k_points % 2 == 0) {
        throw std::runtime_error(
            "The value of k_points should be odd for a symmetric grid. Please check config.yaml file."
        );
    }

    if (a <= 0.0) {
        throw std::runtime_error(
            "The lattice constant should be greater than 0. Please check config.yaml file."
        );
    }

    k_grid_.clear();
    k_grid_.reserve(static_cast<std::size_t>(k_points));

    const double dk = (2.0 * pi / a) / static_cast<double>(k_points);
    const int half = k_points / 2;

    for (int i = 0; i < k_points; ++i) {
        const int centered_index = i - half;
        k_grid_.push_back(static_cast<double>(centered_index) * dk);
    }
}

void GridData::build_bands(const ModelConfig& model_config)
{
    const double E_gap = model_config.E_gap_eV;
    const double deltaE_c = model_config.deltaE_c_eV;
    const double deltaE_v = model_config.deltaE_v_eV;
    const double a = model_config.lattice_constant_A;

    if (E_gap < 0.0) {
        throw std::runtime_error(
            "The band gap should be greater than or equal to 0. Please check config.yaml file."
        );
    }

    if (deltaE_c < 0.0) {
        throw std::runtime_error(
            "deltaE_c_eV should be greater than or equal to 0. Please check config.yaml file."
        );
    }

    if (deltaE_v < 0.0) {
        throw std::runtime_error(
            "deltaE_v_eV should be greater than or equal to 0. Please check config.yaml file."
        );
    }

    if (a <= 0.0) {
        throw std::runtime_error(
            "The lattice constant should be greater than 0. Please check config.yaml file."
        );
    }

    if (k_grid_.empty()) {
        throw std::runtime_error(
            "k_grid must be initialized before building the bands."
        );
    }

    valence_band_.clear();
    conduction_band_.clear();

    valence_band_.reserve(k_grid_.size());
    conduction_band_.reserve(k_grid_.size());

    for (double k : k_grid_){

        const double ka = k*a;


        // Tight-binding bands with bandwidths deltaE_c and deltaE_v.
        // At k = 0:
        //   Ec = +E_gap/2
        //   Ev = -E_gap/2
        // At zone edge:
        //   Ec = +E_gap/2 + deltaE_c
        //   Ev = -E_gap/2 - deltaE_v
        const double Ec = 0.5 * E_gap + 0.5 * deltaE_c * (1.0 - std::cos(ka));
        const double Ev = -0.5 * E_gap - 0.5 * deltaE_v * (1.0 - std::cos(ka));

        conduction_band_.push_back(Ec);
        valence_band_.push_back(Ev);

    }
}