#include "grid.hpp"

#include <cmath>
#include <cstddef>
#include <stdexcept>

namespace {
constexpr double fs_to_s = 1.0e-15;
constexpr double angstrom_to_m = 1.0e-10;
constexpr double eV_to_J = 1.602176634e-19;
} // namespace

Grid::Grid(const EngineConfig& config)
{
    build_time_grid(config.grid_config);
    build_k_grid(config.grid_config, config.simulation_config.model_config);
    build_bands(config.simulation_config.model_config);
}

const std::vector<double>& Grid::time_s() const
{
    return time_s_;
}

const std::vector<double>& Grid::k_grid() const
{
    return k_grid_;
}

const std::vector<double>& Grid::valence_band() const
{
    return valence_band_;
}

const std::vector<double>& Grid::conduction_band() const
{
    return conduction_band_;
}

double Grid::dk() const
{
    return dk_;
}

double Grid::two_dk() const
{
    return two_dk_;
}

void Grid::build_time_grid(const GridConfig& grid_config)
{
    const double t_start_fs = grid_config.t_start_fs;
    const double t_end_fs = grid_config.t_end_fs;
    const double dt_fs = grid_config.dt_fs;

    if (t_end_fs <= t_start_fs) {
        throw std::runtime_error(
            "Grid::build_time_grid: t_end_fs must be greater than t_start_fs"
        );
    }

    if (dt_fs <= 0.0) {
        throw std::runtime_error(
            "Grid::build_time_grid: dt_fs must be greater than zero"
        );
    }

    const double t_start_s = t_start_fs * fs_to_s;
    const double dt_s = dt_fs * fs_to_s;

    const std::size_t n_points =
        static_cast<std::size_t>(
            std::floor((t_end_fs - t_start_fs) / dt_fs + 0.5)
        ) + 1;

    time_s_.clear();
    time_s_.reserve(n_points);

    for (std::size_t i = 0; i < n_points; ++i) {
        time_s_.push_back(t_start_s + static_cast<double>(i) * dt_s);
    }
}

void Grid::build_k_grid(
    const GridConfig& grid_config,
    const ModelConfig& model_config
)
{
    const int k_points = grid_config.k_points;
    const double a_m = model_config.lattice_constant_A * angstrom_to_m;

    const double pi = std::acos(-1.0);

    if (k_points <= 0) {
        throw std::runtime_error(
            "Grid::build_k_grid: k_points must be greater than zero"
        );
    }

    if (k_points % 2 == 0) {
        throw std::runtime_error(
            "Grid::build_k_grid: k_points must be odd for a symmetric grid"
        );
    }

    if (a_m <= 0.0) {
        throw std::runtime_error(
            "Grid::build_k_grid: lattice constant must be greater than zero"
        );
    }

    k_grid_.clear();
    k_grid_.reserve(static_cast<std::size_t>(k_points));

    dk_ = (2.0 * pi / a_m) / static_cast<double>(k_points);
    two_dk_ = 2.0 * dk_;

    const int half = k_points / 2;

    for (int i = 0; i < k_points; ++i) {
        const int centered_index = i - half;
        k_grid_.push_back(static_cast<double>(centered_index) * dk_);
    }
}

void Grid::build_bands(const ModelConfig& model_config)
{
    const double E_gap_J = model_config.E_gap_eV * eV_to_J;
    const double deltaE_c_J = model_config.deltaE_c_eV * eV_to_J;
    const double deltaE_v_J = model_config.deltaE_v_eV * eV_to_J;
    const double a_m = model_config.lattice_constant_A * angstrom_to_m;

    if (E_gap_J < 0.0) {
        throw std::runtime_error(
            "Grid::build_bands: E_gap_eV must be greater than or equal to zero"
        );
    }

    if (deltaE_c_J < 0.0) {
        throw std::runtime_error(
            "Grid::build_bands: deltaE_c_eV must be greater than or equal to zero"
        );
    }

    if (deltaE_v_J < 0.0) {
        throw std::runtime_error(
            "Grid::build_bands: deltaE_v_eV must be greater than or equal to zero"
        );
    }

    if (a_m <= 0.0) {
        throw std::runtime_error(
            "Grid::build_bands: lattice constant must be greater than zero"
        );
    }

    if (k_grid_.empty()) {
        throw std::runtime_error(
            "Grid::build_bands: k_grid must be initialized before building bands"
        );
    }

    valence_band_.clear();
    conduction_band_.clear();

    valence_band_.reserve(k_grid_.size());
    conduction_band_.reserve(k_grid_.size());

    for (double k : k_grid_) {
        const double ka = k * a_m;

        // Tight-binding bands in SI units, Joules.
        //
        // At k = 0:
        //   Ec = +E_gap / 2
        //   Ev = -E_gap / 2
        //
        // At zone edge:
        //   Ec = +E_gap / 2 + deltaE_c
        //   Ev = -E_gap / 2 - deltaE_v
        const double Ec =
            0.5 * E_gap_J + 0.5 * deltaE_c_J * (1.0 - std::cos(ka));

        const double Ev =
            -0.5 * E_gap_J - 0.5 * deltaE_v_J * (1.0 - std::cos(ka));

        conduction_band_.push_back(Ec);
        valence_band_.push_back(Ev);
    }
}