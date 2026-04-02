from pathlib import Path
import yaml
import json
import subprocess

from config.validate import validate_config


# ---------------------------
# File Handling
# ---------------------------
def check_path(path: Path) -> Path:
    if not path.exists():
        raise FileNotFoundError(f"Configuration file doesn't exist at {path}")
    if not path.is_file():
        raise IsADirectoryError(f"{path} is not a file")
    return path


def parse_yaml(path: Path) -> dict:
    try:
        with path.open("r", encoding="utf-8") as f:
            data = yaml.safe_load(f)
    except yaml.YAMLError as e:
        raise ValueError(f"Invalid YAML syntax in {path}") from e

    if data is None:
        raise ValueError("YAML file is empty")

    if not isinstance(data, dict):
        raise ValueError("Top-level YAML must be a mapping (dict)")

    return data


# ---------------------------
# Run Expansion
# ---------------------------
def expand_runs(config: dict) -> list[dict]:
    optical_fields = config["fields"]["optical"]
    dc_fields = config["fields"]["dc"]

    runs = []

    for opt in optical_fields:
        for dc in dc_fields:
            runs.append(
                {
                    "optical": opt,
                    "dc": dc,
                }
            )

    return runs


# ---------------------------
# JSON Builder (ONE run only)
# ---------------------------
def make_json(run: dict, config: dict, output_dir: Path) -> Path:
    output_dir.mkdir(parents=True, exist_ok=True)
    json_path = output_dir / "input.json"

    simulation = config["simulation"]
    discretization = config["discretization"]
    interactions = config.get("interactions", {})
    output_cfg = config["output"]

    optical = run["optical"]
    dc = run["dc"]

    fields = [
        {
            "kind": "optical",
            "id": optical["id"],
            "pulse_type": optical["pulse_type"],
            "amplitude": optical["amplitude"],
            "frequency": optical["frequency"],
            "duration_fs": optical["duration_fs"],
            "t0_fs": optical["t0_fs"],
        },
        {
            "kind": "dc",
            "id": dc["id"],
            "pulse_type": dc["pulse_type"],
            "amplitude": dc["amplitude"],
            "t_on_fs": dc["t_on_fs"],
            "t_off_fs": dc["t_off_fs"],
        },
    ]

    engine_json = {
        "schema_version": "0.1",
        "model": simulation["model"],
        "order_expansion": simulation["order_expansion"],
        "grid": {
            "k_points": discretization["k_points"],
            "dt_fs": discretization["dt_fs"],
            "t_start_fs": discretization["t_start_fs"],
            "t_end_fs": discretization["t_end_fs"],
        },
        "interactions": interactions,
        "fields": fields,
        "output": output_cfg,
    }

    with json_path.open("w", encoding="utf-8") as f:
        json.dump(engine_json, f, indent=2)

    return json_path


# ---------------------------
# Core Execution
# ---------------------------
def pass_to_core(json_path: Path, output_dir: Path) -> None:
    repo_root = Path(__file__).resolve().parents[1]
    engine_path = repo_root / "core" / "build" / "bin" / "sbe_engine"

    if not engine_path.exists():
        raise FileNotFoundError(
            f"Engine executable not found at: {engine_path}\n"
            "Build it first (CMake)."
        )

    subprocess.run(
        [str(engine_path), str(json_path), str(output_dir)],
        check=True,
    )


# ---------------------------
# Entry Point
# ---------------------------
def run_command(args) -> None:
    config_path = check_path(Path(args.config))
    base_output_dir = Path(args.out)

    config_dict = parse_yaml(config_path)
    validated = validate_config(config_dict)

    runs = expand_runs(validated)

    for i, run in enumerate(runs):
        run_dir = base_output_dir / f"run_{i}"

        json_path = make_json(run, validated, run_dir)
        pass_to_core(json_path, run_dir)

    print(f"OK: executed {len(runs)} runs in {base_output_dir}")