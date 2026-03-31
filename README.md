# Semiconductor Bloch Equation Simulator

A modular C++ framework for simulating **light--matter interaction in
semiconductors** using the **Semiconductor Bloch Equations (SBE)** with
optional **order-expanded polarization dynamics**.

The simulator models the interaction of **optical and DC electric fields
with semiconductor band structures**, allowing investigation of:

-   nonlinear polarization
-   carrier dynamics
-   harmonic generation
-   field-driven transport
-   Coulomb interaction effects (optional)

The project is designed for **research simulations**, emphasizing
**reproducibility, modular architecture, and configurable experiments**.

------------------------------------------------------------------------

# Features

## Physics Simulation

-   Semiconductor Bloch Equation solver
-   k-space discretized band structure
-   Runge--Kutta time integration
-   Optical field interaction
-   Static/DC field interaction
-   Optional **order expansion** of polarization

Supported expansion orders include

p01 p10\
p11 p20\
n11 n21

This enables systematic investigation of **nonlinear optical response**.

------------------------------------------------------------------------

## Field Models

Supported field types

### Optical field

-   Gaussian pulse
-   Box pulse

Parameters

-   amplitude
-   frequency
-   duration
-   pulse center

### DC field

-   amplitude
-   switch-on time
-   switch-off time

------------------------------------------------------------------------

## Coulomb Interaction (optional)

The simulator allows enabling Coulomb interaction with configurable
screening models.

Example configuration:

    coulomb:
      enabled: true
      screening: none

------------------------------------------------------------------------

## Flexible Configuration

Simulations are controlled via structured configuration files.

Main configuration groups:

-   GridConfig
-   FieldConfig
-   InteractionConfig
-   OrderExpansionConfig
-   OutputConfig

This makes experiments **fully reproducible**.

------------------------------------------------------------------------

# Project Structure

    .
    ├── include/
    │   ├── config/
    │   ├── simulation/
    │   └── io/
    │
    ├── src/
    │
    ├── configs/
    │
    ├── results/
    │
    ├── CMakeLists.txt
    └── README.md

------------------------------------------------------------------------

# Simulation Overview

The solver evolves the following quantities in time

p(k,t) → microscopic polarization\
n(k,t) → carrier density

The dynamics include

∂p/∂t = -iω(k)p + driving terms + Coulomb terms\
∂n/∂t = population dynamics

The macroscopic polarization is obtained by integrating over k-space

P(t) = Σₖ p(k,t)

From this the simulator computes observables such as

-   polarization
-   current density
-   harmonic spectra

------------------------------------------------------------------------

# Example Configuration

Example YAML configuration file

``` yaml
grid:
  k_points: 1000
  dt_fs: 0.1
  t_start_fs: -1000
  t_end_fs: 2000

optical_field:
  kind: optical
  pulse_type: gaussian
  amplitude: 0.005
  frequency: 0.057
  duration_fs: 50
  t0_fs: 0

dc_field:
  kind: dc
  amplitude: 0.002
  t_on_fs: -500
  t_off_fs: 1500

order_expansion:
  enabled: true
  max_order: 3

interaction:
  coulomb:
    enabled: false
    screening: none

output:
  format: binary
  save_k_resolved: false
  save_macroscopic: true
  observables:
    - polarization
    - current
```

------------------------------------------------------------------------

# Building the Project

Requirements

-   C++17 or newer
-   CMake 3.16+

Build steps

``` bash
git clone https://github.com/yourusername/sbe-simulator.git

cd sbe-simulator

mkdir build
cd build

cmake ..
make
```

------------------------------------------------------------------------

# Running a Simulation

Example run

    ./simulator --config configs/example_config.yaml

Output files are written to the `results/` directory.

------------------------------------------------------------------------

# Output

When **order expansion is enabled**, the simulator outputs

-   p01.bin
-   p10.bin
-   p11.bin
-   p20.bin
-   n11.bin
-   n21.bin

Otherwise it outputs only **macroscopic observables**

-   polarization.bin
-   current.bin
-   electric_field.bin

------------------------------------------------------------------------

# Post‑Processing

Simulation outputs can be analyzed using Python scripts.

Typical analysis includes

-   Fourier transforms
-   harmonic spectra
-   Gabor transforms
-   time‑frequency analysis

Example stack

-   NumPy
-   SciPy
-   Matplotlib
-   Pandas

------------------------------------------------------------------------

# Research Applications

Possible applications include

-   high harmonic generation in semiconductors
-   nonlinear optical response
-   field-driven carrier transport
-   ultrafast polarization dynamics
-   validation of analytical order-expansion theory

------------------------------------------------------------------------

# Future Improvements

Planned extensions

-   GPU acceleration
-   parallel k-space integration
-   additional band models
-   improved Coulomb screening models
-   Python API
-   visualization tools

------------------------------------------------------------------------

# Author

Joanan John Jacob\
MSc Optoelectronics and Photonics\
Paderborn University
