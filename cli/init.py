from pathlib import Path 

from pathlib import Path

DEFAULT_CONFIG = """\
simulation:
  model: sbe
  order_expansion:
    enabled: true
    max_order: 3

discretization:
  k_points: 1000
  dt_fs: 0.01
  t_start_fs: -1000
  t_end_fs: 2000

interactions:
  coulomb:
    enabled: false
    screening: none

fields:
  optical:
    - id: opt_1
      pulse_type: gaussian
      amplitude: 0.01
      frequency: 0.3
      duration_fs: 100
      t0_fs: 0

  dc:
    - id: dc_1
      pulse_type: box
      amplitude: 0.002
      t_on_fs: -500
      t_off_fs: 1500

sweep:
  strategy: cartesian
  nest_by: optical

output:
  root_dir: output
  format: binary
  save_k_resolved: true
  save_macroscopic: true
  observables:
    - polarization
    - number_density
    - current_density

analysis:
  fft:
    enabled: true
    extract_harmonics: [1, 2, 3]
"""

def init_config(path : str) -> None:

  # Making path string into a Path Object 
  path = Path(path) 

  #Check whether file exists or not 
  if path.exists():
    # Raise File Exist error
    raise FileExistsError(f"Configuration file already exist at {path}")

  if not path.parent.exists(): 
    path.parent.mkdir(parents=True, exist_ok=True)

  path.write_text(DEFAULT_CONFIG, encoding="utf-8")

  print(f"Config file is created at {path} ")