#include "field_builder.hpp"
#include "config.hpp"


#include <cmath>
#include <cstddef>
#include <stdexcept>


namespace{
  void validate_time_grid(const std::vector<double>& time_fs){
    
    if(time_fs.empty()){
      throw std::invalid_argument(
        "build_field_samples: time vector is empty"
      );  
    } //if for empty

    for(double t: time_fs){
      if(!std::isfinite(t))
      {
        throw std::invalid_argument(
          "build_field_samples : time vector values should be finite"
        );
      } // if for finite
    }//for time loop
  }//validate_time

  void validate_dt(double dt_fs)
  {
    if(!std::isfinite(dt_fs)||dt_fs <= 0){
      throw std::invalid_argument(
        "build_field_samples : dt_fs should be finite and positive"
      );
    }

 

  } //validate dt 

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

    const double wL = optical.frequency.value();
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
  } //validate optical

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
  } //validate dc config

 double gaussian_optical_value(double t_s, const FieldConfig& optical)
 {

  constexpr double fs_to_s = 1e-15; //for converting to seconds
  constexpr double MV_cm = 1e8; // converting to V/m

  const double A = optical.amplitude;
  const double wL = optical.frequency.value();
  const double duration_fs = optical.duration_fs.value();
  const double t0_fs = optical.t0_fs.value();

  const double A_SI = A*MV_cm;
  const double t_s = t_fs*fs_to_s;
  const double duration_s = duration_fs*fs_to_s;
  const double t0_s = t0_fs*fs_to_s;

  const double sigma = duration_s /(2.0*std::sqrt(2.0*std::log(2.0)));

  const double dt_s = t_s - t0_s;

  const double envelope = std::exp(-(dt_s*dt_s)/(2.0*(sigma*sigma)));

  const double field_value =  A_SI*envelope*std::cos(wL*t_s);
  
  return field_value;

 } // gaussian opical value

 double box_dc_value(double t_fs, const FieldConfig& dc){

 }//dc_box_value

}//namespace

FieldSamples build_field_samples(
  const std::vector<double>& time_fs,
  const double dt_fs,
  const FieldConfig& optical,
  const FieldConfig& dc
){
  FieldSamples samples;

  validate_time_grid(time_fs);
  validate_dt(dt_fs);
  validate_optical_config(optical);
  validate_dc_config(dc);

  std::size_t n = time_fs.size();

  samples.optical_t.reserve(n);
  samples.optical_half.reserve(n);
  samples.optical_next.reserve(n);

  samples.dc_t.reserve(n);
  samples.dc_half.reserve(n);
  samples.dc_next.reserve(n); 




  return samples;
}