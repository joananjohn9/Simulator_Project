#pragma once 

#include "config.hpp"

#include <vector>


struct FieldSamples{

    //optical field
    std::vector<double> optical_t;
    std::vector<double> optical_half;
    std::vector<double> optical_next;

    //dc field
    std::vector<double> dc_t;
    std::vector<double> dc_half;
    std::vector<double> dc_next;

};

FieldSamples buld_field_samples(const std::vector<double>& time_fs,
    double dt_fs,
    const FieldConfig &optical,
    const FieldConfig & dc);