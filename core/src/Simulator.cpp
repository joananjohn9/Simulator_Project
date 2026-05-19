#include "Simulator.hpp"
#include "SBEState.hpp"
#include "SimulationResult.hpp"
#include "field_builder.hpp"
#include "sbe_rhs.hpp"

#include <complex>
#include <cstddef>
#include <stdexcept>

Simulator::Simulator(const EngineConfig& config)
    : config_(config),
      grid_(config)
{
}

SimulationResult Simulator::run()
{
    SimulationResult result = initialize_result();

    SBEState state = make_initial_state(result.k_grid.size());
    
    std::vector<double> omega_k = compute_transition_frequencies(result.conduction_band,
         result.valence_band);

    SBEState rhs = make_initial_state(result.k_grid.size());

    const double dt_s = config_.grid_config.dt_fs*1.0e-15;

    for (std::size_t ti = 0; ti < result.time_s.size(); ++ti) {

        const double E_t = result.fields.optical_t[ti] ;

        compute_rhs_simple(
            state,
            rhs,
            omega_k,
            E_t,
            config_.simulation_config.model_config
        );

        for(std::size_t k = 0; k < state.p_k.size(); ++k){
            
            state.p_k[k] += dt_s*rhs.p_k[k];
            state.n_k[k] += dt_s * rhs.n_k[k] ;
        }
        
        const Macroscopic_observables obs = compute_macroscopic_observables(state);

        append_observables(result,obs);
    }

    return result;
}

void Simulator::validate_required_fields() const
{
    if (!config_.optical_config.has_value()) {
        throw std::runtime_error(
            "Simulator::run: missing optical field config"
        );
    }

    if (!config_.dc_config.has_value()) {
        throw std::runtime_error(
            "Simulator::run: missing dc field config"
        );
    }
}

SBEState Simulator::make_initial_state(std::size_t k_points) const
{
    SBEState state;

    state.p_k.assign(k_points, std::complex<double>{0.0, 0.0});
    state.n_k.assign(k_points, std::complex<double>{0.0, 0.0});

    return state;
}

void Simulator::reserve_macroscopic_outputs(SimulationResult& result) const
{
    const std::size_t n_t = result.time_s.size();

    result.P_real.reserve(n_t);
    result.P_imag.reserve(n_t);

    result.n_real.reserve(n_t);
    result.n_imag.reserve(n_t);

    result.J_real.reserve(n_t);
    result.J_imag.reserve(n_t);
}

void Simulator::append_zero_observables(SimulationResult& result) const
{
    result.P_real.push_back(0.0);
    result.P_imag.push_back(0.0);

    result.n_real.push_back(0.0);
    result.n_imag.push_back(0.0);

    result.J_real.push_back(0.0);
    result.J_imag.push_back(0.0);
}

SimulationResult Simulator::initialize_result()
{
    validate_required_fields();

    SimulationResult result;

    result.time_s = grid_.time_s();
    result.k_grid = grid_.k_grid();
    result.valence_band = grid_.valence_band();
    result.conduction_band = grid_.conduction_band();

    const double dt_s = config_.grid_config.dt_fs * 1.0e-15;

    result.fields = build_field_samples(
        result.time_s,
        dt_s,
        config_.optical_config.value(),
        config_.dc_config.value()
    );

    reserve_macroscopic_outputs(result);

    return result;
}

Macroscopic_observables Simulator::compute_macroscopic_observables(const SBEState& state)const
{
        Macroscopic_observables obs {
            std::complex<double> {0.0,0.0},
            std::complex<double> {0.0,0.0},
            std::complex<double> {0.0,0.0},

            
        };

        const std::size_t n_k = state.p_k.size();

        if (n_k != state.n_k.size())
        {
            throw std::runtime_error("Simulator::compute_macroscopic_observables: state size mismatch");
        }

        if (n_k == 0)
        {
            throw std::runtime_error("Simulator::compute_macroscopic_observable: empty state");
        }

        const double mu_C_m = config_.simulation_config.model_config.mu_e_A*1.602176634e-19*1.0e-10;

        for (std::size_t k=0; k<n_k; k++)
        {
            obs.P += mu_C_m * ( state.p_k[k] + std::conj( state.p_k[k])  ) ;
            obs.n += state.n_k[k]; 
        }

        obs.P /= static_cast<double>(n_k);
        obs.n /= static_cast<double>(n_k);
        obs.J /= static_cast<double>(n_k);

        return obs;
}

void Simulator::append_observables(SimulationResult& result, const Macroscopic_observables& obs) const
{

    result.P_real.push_back(obs.P.real());
    result.P_imag.push_back(obs.P.imag());
    result.n_real.push_back(obs.n.real());
    result.n_imag.push_back(obs.n.imag());
    result.J_real.push_back(obs.J.real());
    result.J_imag.push_back(obs.J.imag());
    
}