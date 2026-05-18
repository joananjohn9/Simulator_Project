#pragma once

#include "config.hpp"

#include <vector>

class Grid {
public:
    explicit Grid(const EngineConfig& config);

    const std::vector<double>& time_s() const;
    const std::vector<double>& k_grid() const;
    const std::vector<double>& valence_band() const;
    const std::vector<double>& conduction_band() const;

    double dk() const;
    double two_dk() const;

private:
    std::vector<double> time_s_;
    std::vector<double> k_grid_;
    std::vector<double> valence_band_;
    std::vector<double> conduction_band_;

    double dk_ = 0.0;
    double two_dk_ = 0.0;

    void build_time_grid(const GridConfig& grid_config);

    void build_k_grid(
        const GridConfig& grid_config,
        const ModelConfig& model_config
    );

    void build_bands(const ModelConfig& model_config);
};