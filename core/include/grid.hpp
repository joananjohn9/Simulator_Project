#pragma once 

#include <vector>
#include "config.hpp"

class Grid
{
    public:
        explicit Grid(const EngineConfig& config);

        const std::vector<double>& time_fs() const;
        const std::vector<double>& k_grid() const;
        const std::vector<double>& valence_band() const;
        const std::vector<double>& conduction_band() const;
        const double& dk() const;
        const double& two_dk() const;


    private:
        std::vector<double> time_fs_;
        std::vector<double> k_grid_;
        std::vector<double> valence_band_ ;
        std::vector<double> conduction_band_;
        std::vector<double> NL,NR; // Nearest left and right neighbour needed to find the Gradient
        double dk_ ;
        double two_dk_ ;

        void build_time_grid(const GridConfig& grid_config);
        void build_k_grid(const GridConfig& grid_config,
                          const ModelConfig& model_config);
        void build_bands(const ModelConfig& model_config);

};