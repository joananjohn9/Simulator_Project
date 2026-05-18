#include "field_builder.hpp"
#include "config.hpp"

#include <stdexcept>
#include <cmath>
#include <cstddef>


namespace
{
    void validate_time(const std::vector<double>& time_s)
    {
        if(time_s.empty()){
            throw std::invalid_argument(
                "build_field_samples : time vector is empty"
            );        
        } // if end 

        for (double t : time_s){
            if(!std::isfinite(t)){

                throw std::invalid_argument(
                    "build_field_samples : time value must be finite"
                );
            }//if 
        }//for 
    }//validate_time

    void validate_dt(double dt_s){

        if(!std::isfinite(dt_s)|| dt_s <=0){
            
            throw std::invalid_argument(
                "buld_field_samples : dt_s must be finite and positive"
            );

        }//end if
    }//validate dt

    void validate_optical_config(const FieldConfig& optical)
    {
    if (optical.pulse_type != "gaussian") {
        throw std::invalid_argument(
            "build_field_samples: unsupported optical pulse type: " + optical.pulse_type
        );
    }

    if (!std::isfinite(optical.amplitude)) {
        throw std::invalid_argument("build_field_samples: optical amplitude must be finite");
    }

    if (!optical.frequency) {
        throw std::invalid_argument("build_field_samples: missing optical wL/frequency");
    }

    if (!optical.duration_fs) {
        throw std::invalid_argument("build_field_samples: missing optical duration_fs");
    }

    if (!optical.t0_fs) {
        throw std::invalid_argument("build_field_samples: missing optical t0_fs");
    }

    const double wL = optical.frequency.value() ;
    const double duration_fs = optical.duration_fs.value();
    const double t0_fs = optical.t0_fs.value();

    if (!std::isfinite(wL) || wL <= 0.0) {
        throw std::invalid_argument("build_field_samples: optical wL/frequency must be finite and positive");
    }

    if (!std::isfinite(duration_fs) || duration_fs <= 0.0) {
        throw std::invalid_argument("build_field_samples: optical duration_fs must be finite and positive");
    }

    if (!std::isfinite(t0_fs)) {
        throw std::invalid_argument("build_field_samples: optical t0_fs must be finite");
    }
    } //validate optical field config

    void validate_dc_config(const FieldConfig& dc)
    {
    if (dc.pulse_type != "box") {
        throw std::invalid_argument(
            "build_field_samples: unsupported dc pulse type: " + dc.pulse_type
        );
    }

    if (!std::isfinite(dc.amplitude)) {
        throw std::invalid_argument("build_field_samples: dc amplitude must be finite");
    }

    if (!dc.t_on_fs) {
        throw std::invalid_argument("build_field_samples: missing dc t_on_fs");
    }

    if (!dc.t_off_fs) {
        throw std::invalid_argument("build_field_samples: missing dc t_off_fs");
    }

    const double t_on_fs = dc.t_on_fs.value();
    const double t_off_fs = dc.t_off_fs.value();

    if (!std::isfinite(t_on_fs)) {
        throw std::invalid_argument("build_field_samples: dc t_on_fs must be finite");
    }

    if (!std::isfinite(t_off_fs)) {
        throw std::invalid_argument("build_field_samples: dc t_off_fs must be finite");
    }

    if (t_off_fs < t_on_fs) {
        throw std::invalid_argument("build_field_samples: dc t_off_fs must be >= t_on_fs");
    }
    } // validate dc config 


    double gaussian_optical_value(double t_s, const FieldConfig& optical)
    {

        double field_value;


        constexpr double eV = 1.602176634e-19;
        constexpr double h_cut = 1.054571817e-34;

        const double A = optical.amplitude;
        const double wL = optical.frequency.value()*eV/h_cut;
        const double duration_fs = optical.duration_fs.value();
        const double t0_fs = optical.t0_fs.value();

        // convert everything to SI
        
        constexpr double fs = 1e-15;
        constexpr double V_m = 1e8;

        const double A_SI = A*V_m;
        const double t0_s = t0_fs*fs;
        const double duration_s = duration_fs*fs;

        const double sigma = duration_s / (2.0*std::sqrt(2.0*std::log(2.0)));

        double Dt_s = t_s - t0_s;

        const double envelope = std::exp(-(Dt_s*Dt_s)/(2*sigma*sigma));

        field_value = A_SI*envelope*std::cos(wL*t_s);


        return field_value;
    } //gaussian optical value

    double smooth_box_dc_value(double t_s, const FieldConfig& dc)
{
    const double A = dc.amplitude;

    constexpr double V_m = 1e8;

    const double A_SI = A*V_m;

    constexpr double fs_to_s = 1.0e-15;

    const double t_on_s = dc.t_on_fs.value() * fs_to_s;
    const double t_off_s = dc.t_off_fs.value() * fs_to_s;

    // Temporary default. Better: add edge_width_fs to config later.
    const double edge_width_s = 10.0 * fs_to_s;

    const double rise =
        0.5 * (1.0 + std::tanh((t_s - t_on_s) / edge_width_s));

    const double fall =
        0.5 * (1.0 - std::tanh((t_s - t_off_s) / edge_width_s));

    return A_SI * rise * fall;
}

}//namespace

FieldSamples build_field_samples(
    const std::vector<double>& time_s,
    double dt_s,
    const FieldConfig& optical,
    const FieldConfig& dc
)
{
    validate_time(time_s);
    validate_dt(dt_s);
    validate_optical_config(optical);
    validate_dc_config(dc);

    const std::size_t n = time_s.size();

    FieldSamples samples;

    samples.optical_t.reserve(n);
    samples.optical_half.reserve(n);
    samples.optical_next.reserve(n);

    samples.dc_t.reserve(n);
    samples.dc_half.reserve(n);
    samples.dc_next.reserve(n);

    for (double t_s : time_s) {
        samples.optical_t.push_back(
            gaussian_optical_value(t_s, optical)
        );

        samples.optical_half.push_back(
            gaussian_optical_value(t_s + 0.5 * dt_s, optical)
        );

        samples.optical_next.push_back(
            gaussian_optical_value(t_s + dt_s, optical)
        );

        samples.dc_t.push_back(
            smooth_box_dc_value(t_s, dc)
        );

        samples.dc_half.push_back(
            smooth_box_dc_value(t_s + 0.5 * dt_s, dc)
        );

        samples.dc_next.push_back(
            smooth_box_dc_value(t_s + dt_s, dc)
        );
    }

    return samples;
}