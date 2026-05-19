#pragma once

#include "SBEState.hpp"
#include "config.hpp"

#include <vector>

std::vector<double> compute_transition_frequencies(
    const std::vector<double>& conduction_band,
    const std::vector<double>& valence_band
);

void compute_rhs_simple(
    const SBEState& state,
    SBEState& rhs,
    const std::vector<double>& omega_k,
    double E_t,
    const ModelConfig& model
);