#include "sbe_rhs.hpp"

#include <cmath>
#include <complex>
#include <cstddef>
#include <stdexcept>
#include <vector>

std::vector<double> compute_transition_frequencies(
    const std::vector<double>& conduction_band,
    const std::vector<double>& valence_band
)
{
    constexpr double hbar_J_s = 1.054571817e-34;

    const std::size_t n_k = conduction_band.size();

    if (n_k != valence_band.size()) {
        throw std::invalid_argument(
            "compute_transition_frequencies: size mismatch between conduction and valence band"
        );
    }

    if (n_k == 0) {
        throw std::invalid_argument(
            "compute_transition_frequencies: empty band arrays"
        );
    }

    std::vector<double> omega_k;
    omega_k.reserve(n_k);

    for (std::size_t k = 0; k < n_k; ++k) {
        omega_k.push_back(
            (conduction_band[k] - valence_band[k]) / hbar_J_s
        );
    }

    return omega_k;
}

void compute_rhs_simple(
    const SBEState& state,
    SBEState& rhs,
    const std::vector<double>& omega_k,
    double E_t,
    const ModelConfig& model
)
{
    constexpr std::complex<double> I{0.0, 1.0};

    constexpr double hbar_J_s = 1.054571817e-34;
    constexpr double e_C = 1.602176634e-19;
    constexpr double angstrom_to_m = 1.0e-10;
    constexpr double fs_to_s = 1.0e-15;

    const std::size_t nk = state.p_k.size();

    if (nk == 0) {
        throw std::invalid_argument("compute_rhs_simple: empty state");
    }

    if (nk != state.n_k.size()) {
        throw std::invalid_argument("compute_rhs_simple: state size mismatch");
    }

    if (nk != omega_k.size()) {
        throw std::invalid_argument("compute_rhs_simple: omega_k size mismatch");
    }

    if (!std::isfinite(E_t)) {
        throw std::invalid_argument("compute_rhs_simple: E_t must be finite");
    }

    const double T2_s = model.T2_fs * fs_to_s;

    if (!std::isfinite(T2_s) || T2_s <= 0.0) {
        throw std::invalid_argument(
            "compute_rhs_simple: T2_s must be finite and positive"
        );
    }

    const double gamma_2 = 1.0 / T2_s;

    const double mu_C_m = model.mu_e_A * e_C * angstrom_to_m;

    if (!std::isfinite(mu_C_m)) {
        throw std::invalid_argument(
            "compute_rhs_simple: dipole moment must be finite"
        );
    }

    rhs.p_k.assign(nk, std::complex<double>{0.0, 0.0});
    rhs.n_k.assign(nk, std::complex<double>{0.0, 0.0});

    for (std::size_t k = 0; k < nk; ++k) {
        rhs.p_k[k] =
            -(I * omega_k[k] + gamma_2) * state.p_k[k]
            + I * mu_C_m * E_t / hbar_J_s;

        rhs.n_k[k] = std::complex<double>{0.0, 0.0};
    }
}