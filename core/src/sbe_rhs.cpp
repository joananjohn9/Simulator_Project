#include "sbe_rhs.hpp"
#include<string>
#include <cmath>
#include <complex>
#include <cstddef>
#include <stdexcept>
#include <vector>

namespace {

constexpr std::complex<double> I{0.0, 1.0};

constexpr double hbar_J_s = 1.054571817e-34;
constexpr double e_C = 1.602176634e-19;
constexpr double angstrom_to_m = 1.0e-10;
constexpr double fs_to_s = 1.0e-15;

void validate_state_and_omega(
    const SBEState& state,
    const std::vector<double>& omega_k,
    const char* function_name
)
{
    const std::size_t nk = state.p_k.size();

    if (nk == 0) {
        throw std::invalid_argument(
            std::string(function_name) + ": empty state"
        );
    }

    if (nk != state.n_k.size()) {
        throw std::invalid_argument(
            std::string(function_name) + ": state size mismatch"
        );
    }

    if (nk != omega_k.size()) {
        throw std::invalid_argument(
            std::string(function_name) + ": omega_k size mismatch"
        );
    }
}

void validate_field(double E_t, const char* function_name)
{
    if (!std::isfinite(E_t)) {
        throw std::invalid_argument(
            std::string(function_name) + ": E_t must be finite"
        );
    }
}

double compute_gamma_2(const ModelConfig& model, const char* function_name)
{
    const double T2_s = model.T2_fs * fs_to_s;

    if (!std::isfinite(T2_s) || T2_s <= 0.0) {
        throw std::invalid_argument(
            std::string(function_name) + ": T2_s must be finite and positive"
        );
    }

    return 1.0 / T2_s;
}

double compute_gamma_1(const ModelConfig& model, const char* function_name)
{
    const double T1_s = model.T1_fs * fs_to_s; 

    

    if(!std::isfinite(T1_s) || T1_s < 0.0)
    {
        throw std::invalid_argument(

            std::string(function_name) + " : T1_s must be finite and non-negative"

        );
    }

    if (T1_s == 0)
    {
        return 0;
    }

    return 1/T1_s;

}

double compute_mu_C_m(const ModelConfig& model, const char* function_name)
{
    const double mu_C_m = model.mu_e_A * e_C * angstrom_to_m;

    if (!std::isfinite(mu_C_m)) {
        throw std::invalid_argument(
            std::string(function_name) + ": dipole moment must be finite"
        );
    }

    return mu_C_m;
}

} // namespace

std::vector<double> compute_transition_frequencies(
    const std::vector<double>& conduction_band,
    const std::vector<double>& valence_band
)
{
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
    constexpr const char* function_name = "compute_rhs_simple";

    const std::size_t nk = state.p_k.size();

    validate_state_and_omega(state, omega_k, function_name);
    validate_field(E_t, function_name);

    const double gamma_2 = compute_gamma_2(model, function_name);
    const double mu_C_m = compute_mu_C_m(model, function_name);
    const double Omega = mu_C_m * E_t / hbar_J_s;

    rhs.p_k.assign(nk, std::complex<double>{0.0, 0.0});
    rhs.n_k.assign(nk, std::complex<double>{0.0, 0.0});

    for (std::size_t k = 0; k < nk; ++k) {
        rhs.p_k[k] =
            -(I * omega_k[k] + gamma_2) * state.p_k[k]
            + I * Omega;

        rhs.n_k[k] = std::complex<double>{0.0, 0.0};
    }
}

void compute_rhs_non_interacting(
    const SBEState& state,
    SBEState& rhs,
    const std::vector<double>& omega_k,
    double E_t,
    const ModelConfig& model
)
{
    constexpr const char* function_name = "compute_rhs_non_interacting";
    const std::size_t nk = state.p_k.size();

    validate_state_and_omega(state,omega_k,function_name);
    validate_field(E_t, function_name); 

    const double gamma_2 = compute_gamma_2(model,function_name);
    const double gamma_1 = compute_gamma_1(model,function_name);
    const double mu_C_m = compute_mu_C_m(model, function_name);
    const double Omega = mu_C_m * E_t / hbar_J_s;

    rhs.p_k.assign(nk, std::complex<double> {0.0 , 0.0});
    rhs.n_k.assign(nk, std::complex<double> {0.0 , 0.0});

    for(std::size_t k = 0 ; k < nk; ++k){

        rhs.p_k[k] = 
            -(I *omega_k[k] + gamma_2) * state.p_k[k] + I*Omega*(1.0 - 2.0 *state.n_k[k]); 

        rhs.n_k[k] = 
            - 2.0 * std::imag(Omega*std::conj(state.p_k[k])) - state.n_k[k]*gamma_1; 
    }
}