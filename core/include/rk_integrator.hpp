#pragma once

#include "SBEState.hpp"
#include "config.hpp"

#include <vector>

void rk4_step_simple(
    SBEState& state,
    const std::vector<double>& omega_k,
    double E_t,
    double E_half,
    double E_next,
    double dt_s,
    const ModelConfig& model
);