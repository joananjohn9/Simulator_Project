def _require_keys(section: dict, required_keys: list[str], path: str) -> None:
    for key in required_keys:
        if key not in section:
            raise KeyError(f"{path} missing required key '{key}'")


def _reject_unknown_keys(section: dict, allowed_keys: list[str], path: str) -> None:
    for key in section:
        if key not in allowed_keys:
            raise KeyError(f"{path} has unknown key '{key}'")


def _require_type(value, expected_type, path: str) -> None:
    if not isinstance(value, expected_type):
        if isinstance(expected_type, tuple):
            expected_name = " or ".join(t.__name__ for t in expected_type)
        else:
            expected_name = expected_type.__name__
        raise TypeError(f"{path} must be {expected_name}, got {type(value).__name__}")


def validate_config(config_dict: dict) -> dict:
    required_keys = ["simulation", "discretization", "fields", "sweep", "output"]

    _require_type(config_dict, dict, "top-level config")
    _require_keys(config_dict, required_keys, "config")

    for key in required_keys:
        _require_type(config_dict[key], dict, f"config.{key}")

    fields = validate_fields(config_dict["fields"])
    validate_optical_fields(fields["optical"])
    validate_dc_fields(fields["dc"])
    validate_sweep(config_dict["sweep"])
    validate_output(config_dict["output"])

    return config_dict


def validate_fields(field_dict: dict) -> dict:
    required_keys = ["optical", "dc"]

    _require_type(field_dict, dict, "fields")
    _require_keys(field_dict, required_keys, "fields")
    _reject_unknown_keys(field_dict, required_keys, "fields")

    for key in required_keys:
        field_list = field_dict[key]

        _require_type(field_list, list, f"fields.{key}")

        if len(field_list) < 1:
            raise ValueError(f"fields.{key} should not be empty")

        for i, field in enumerate(field_list):
            _require_type(field, dict, f"fields.{key}[{i}]")

    return field_dict


def validate_optical_fields(optical_fields: list) -> list:
    required_keys = ["id", "pulse_type", "amplitude", "frequency", "duration_fs", "t0_fs"]
    allowed_keys = required_keys

    for i, optical_field in enumerate(optical_fields):
        path = f"fields.optical[{i}]"

        _require_keys(optical_field, required_keys, path)
        _reject_unknown_keys(optical_field, allowed_keys, path)

        _require_type(optical_field["id"], str, f"{path}.id")
        _require_type(optical_field["pulse_type"], str, f"{path}.pulse_type")
        _require_type(optical_field["amplitude"], (int, float), f"{path}.amplitude")
        _require_type(optical_field["frequency"], (int, float), f"{path}.frequency")
        _require_type(optical_field["duration_fs"], (int, float), f"{path}.duration_fs")
        _require_type(optical_field["t0_fs"], (int, float), f"{path}.t0_fs")

    return optical_fields


def validate_dc_fields(dc_fields: list) -> list:
    required_keys = ["id", "pulse_type", "amplitude", "t_on_fs", "t_off_fs"]
    allowed_keys = required_keys

    for i, dc_field in enumerate(dc_fields):
        path = f"fields.dc[{i}]"

        _require_keys(dc_field, required_keys, path)
        _reject_unknown_keys(dc_field, allowed_keys, path)

        _require_type(dc_field["id"], str, f"{path}.id")
        _require_type(dc_field["pulse_type"], str, f"{path}.pulse_type")
        _require_type(dc_field["amplitude"], (int, float), f"{path}.amplitude")
        _require_type(dc_field["t_on_fs"], (int, float), f"{path}.t_on_fs")
        _require_type(dc_field["t_off_fs"], (int, float), f"{path}.t_off_fs")

    return dc_fields


def validate_sweep(sweep: dict) -> dict:
    required_keys = ["strategy", "nest_by"]
    allowed_keys = required_keys
    allowed_strategies = ["cartesian"]
    allowed_nesting = ["optical", "dc", "flat"]

    _require_keys(sweep, required_keys, "sweep")
    _reject_unknown_keys(sweep, allowed_keys, "sweep")

    _require_type(sweep["strategy"], str, "sweep.strategy")
    _require_type(sweep["nest_by"], str, "sweep.nest_by")

    if sweep["strategy"] not in allowed_strategies:
        raise ValueError(
            f"sweep.strategy must be one of {allowed_strategies}, got '{sweep['strategy']}'"
        )

    if sweep["nest_by"] not in allowed_nesting:
        raise ValueError(
            f"sweep.nest_by must be one of {allowed_nesting}, got '{sweep['nest_by']}'"
        )

    return sweep


def validate_output(output: dict) -> dict:
    required_keys = [
        "root_dir",
        "format",
        "save_k_resolved",
        "save_macroscopic",
        "observables",
    ]
    allowed_keys = required_keys

    _require_keys(output, required_keys, "output")
    _reject_unknown_keys(output, allowed_keys, "output")

    _require_type(output["root_dir"], str, "output.root_dir")
    _require_type(output["format"], str, "output.format")
    _require_type(output["save_k_resolved"], bool, "output.save_k_resolved")
    _require_type(output["save_macroscopic"], bool, "output.save_macroscopic")
    _require_type(output["observables"], list, "output.observables")

    for i, observable in enumerate(output["observables"]):
        _require_type(observable, str, f"output.observables[{i}]")

    return output