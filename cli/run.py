from pathlib import Path
import yaml
import json
import subprocess
from config.validate import validate_config


def check_path(path: Path) -> Path:
    if not path.exists():
        raise FileNotFoundError(f"Configuration file doesn't exist at {path}.")
    if not path.is_file():
        raise IsADirectoryError(f"{path} is not a file")
    return path


def parse_yaml(path: Path) -> dict:
    try:
        with path.open("r", encoding="utf-8") as f:
            data = yaml.safe_load(f)
    except yaml.YAMLError as e:
        raise ValueError(f"Invalid yaml syntax in {path}") from e

    if data is None:
        raise ValueError("Yaml file is empty")

    if not isinstance(data, dict):
        raise ValueError(f"Top level yaml structure must be a mapping: {path}")

    return data





def make_json(validated_config_dict: dict, output_dir: Path) -> Path:
    output_dir.mkdir(parents=True, exist_ok=True)
    json_path = output_dir / "input.json"

    simulation = validated_config_dict["simulation"]
    discretization = validated_config_dict["discretization"]
    interactions = validated_config_dict.get("interactions", {})  # optional
    fields_cfg = validated_config_dict["fields"]
    output_cfg = validated_config_dict["output"]

    fields = []

    if "optical" in fields_cfg:
        pulse = fields_cfg["optical"]["pulse"]
        fields.append(
            {
                "kind": "optical",
                "pulse_type": pulse["type"],
                "amplitude": pulse["amplitude"],
                "frequency": pulse["frequency"],
                "duration_fs": pulse["duration_fs"],
                "t0_fs": pulse["t0_fs"],
            }
        )

    if "dc" in fields_cfg:
        pulse = fields_cfg["dc"]["pulse"]
        fields.append(
            {
                "kind": "dc",
                "pulse_type": pulse["type"],
                "amplitude": pulse["amplitude"],
                "t_on_fs": pulse["t_on_fs"],
                "t_off_fs": pulse["t_off_fs"],
            }
        )

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


def pass_to_core(json_path: Path, output_dir: Path) -> None:
    # repo_root is the "sim/" directory (because this file is sim/cli/run.py)
    repo_root = Path(__file__).resolve().parents[1]

    engine_path = repo_root / "core" / "build" / "bin" / "sbe_engine"
    if not engine_path.exists():
        raise FileNotFoundError(
            f"Engine executable not found at: {engine_path}\n"
            f"Build it first (CMake) so core/build/bin/sbe_engine exists."
        )

    # Important: use subprocess.run, and pass strings (portable)
    subprocess.run(
        [str(engine_path), str(json_path), str(output_dir)],
        check=True,
    )


def run_command(args) -> None:
    config_path = check_path(Path(args.config))
    output_dir = Path(args.out)

    config_dict = parse_yaml(config_path)
    validated = validate_config(config_dict)

    json_path = make_json(validated, output_dir)

    # This was missing in your earlier code:
    pass_to_core(json_path, output_dir)

    print(f"OK: wrote {json_path} and invoked engine -> outputs in {output_dir}")