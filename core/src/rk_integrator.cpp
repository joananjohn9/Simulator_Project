#include "rk_integrator.hpp"
#include "sbe_rhs.hpp"

#include <complex>
#include <cstddef>
#include <stdexcept>

namespace {

void validate_state_sizes(
    const SBEState& state,
    const std::vector<double>& omega_k
)
{
    const std::size_t nk = state.p_k.size();

    if (nk == 0) {
        throw std::invalid_argument("rk4_step_simple: empty state");
    }

    if (state.n_k.size() != nk) {
        throw std::invalid_argument("rk4_step_simple: state size mismatch");
    }

    if (omega_k.size() != nk) {
        throw std::invalid_argument("rk4_step_simple: omega_k size mismatch");
    }
}

SBEState make_zero_state(std::size_t nk)
{
    SBEState state;

    state.p_k.assign(nk, std::complex<double>{0.0, 0.0});
    state.n_k.assign(nk, std::complex<double>{0.0, 0.0});

    return state;
}

SBEState add_scaled_state(
    const SBEState& state,
    const SBEState& derivative,
    double scale
)
{
    const std::size_t nk = state.p_k.size();

    SBEState result;
    result.p_k.resize(nk);
    result.n_k.resize(nk);

    for (std::size_t k = 0; k < nk; ++k) {
        result.p_k[k] = state.p_k[k] + scale * derivative.p_k[k];
        result.n_k[k] = state.n_k[k] + scale * derivative.n_k[k];
    }

    return result;
}

} // namespace

void rk4_step_simple(
    SBEState& state,
    const std::vector<double>& omega_k,
    double E_t,
    double E_half,
    double E_next,
    double dt_s,
    const ModelConfig& model
)
{
    validate_state_sizes(state, omega_k);

    if (dt_s <= 0.0) {
        throw std::invalid_argument("rk4_step_simple: dt_s must be positive");
    }

    const std::size_t nk = state.p_k.size();

    SBEState k1 = make_zero_state(nk);
    SBEState k2 = make_zero_state(nk);
    SBEState k3 = make_zero_state(nk);
    SBEState k4 = make_zero_state(nk);

    compute_rhs_simple(state, k1, omega_k, E_t, model);

    SBEState temp = add_scaled_state(state, k1, 0.5 * dt_s);
    compute_rhs_simple(temp, k2, omega_k, E_half, model);

    temp = add_scaled_state(state, k2, 0.5 * dt_s);
    compute_rhs_simple(temp, k3, omega_k, E_half, model);

    temp = add_scaled_state(state, k3, dt_s);
    compute_rhs_simple(temp, k4, omega_k, E_next, model);

    for (std::size_t k = 0; k < nk; ++k) {
        state.p_k[k] +=
            (dt_s / 6.0) *
            (k1.p_k[k] + 2.0 * k2.p_k[k] + 2.0 * k3.p_k[k] + k4.p_k[k]);

        state.n_k[k] +=
            (dt_s / 6.0) *
            (k1.n_k[k] + 2.0 * k2.n_k[k] + 2.0 * k3.n_k[k] + k4.n_k[k]);
    }
}