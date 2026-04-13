def _ensure_mapping(value, name: str) -> dict:
    if not isinstance(value, dict):
        raise TypeError(f"{name} must be a mapping (dict)")
    return value


def _ensure_number(value, name: str) -> None:
    if not isinstance(value, (int, float)):
        raise TypeError(f"{name} must be a number")


def _ensure_string(value, name: str) -> None:
    if not isinstance(value, str):
        raise TypeError(f"{name} must be a string")


def validate_simulation(simulation_dict: dict) -> dict:
    _ensure_mapping(simulation_dict, "simulation")

    if "model" not in simulation_dict:
        raise KeyError("simulation missing required key 'model'")
    if "order_expansion" not in simulation_dict:
        raise KeyError("simulation missing required key 'order_expansion'")

    model = _ensure_mapping(simulation_dict["model"], "simulation.model")

    required_model_keys = ["type", "lattice_constant_A", "E_gap_eV"]
    for key in required_model_keys:
        if key not in model:
            raise KeyError(f"simulation.model missing required key '{key}'")

    _ensure_string(model["type"], "simulation.model.type")
    _ensure_number(model["lattice_constant_A"], "simulation.model.lattice_constant_A")
    _ensure_number(model["E_gap_eV"], "simulation.model.E_gap_eV")

    if model["lattice_constant_A"] <= 0:
        raise ValueError("simulation.model.lattice_constant_A must be > 0")

    if model["E_gap_eV"] < 0:
        raise ValueError("simulation.model.E_gap_eV must be >= 0")

    order = _ensure_mapping(simulation_dict["order_expansion"], "simulation.order_expansion")

    if "enabled" not in order:
        raise KeyError("simulation.order_expansion missing required key 'enabled'")
    if not isinstance(order["enabled"], bool):
        raise TypeError("simulation.order_expansion.enabled must be a bool")

    if "max_order" not in order:
        raise KeyError("simulation.order_expansion missing required key 'max_order'")
    if not isinstance(order["max_order"], int):
        raise TypeError("simulation.order_expansion.max_order must be an int")
    if order["max_order"] < 0:
        raise ValueError("simulation.order_expansion.max_order must be >= 0")

    return simulation_dict


def validate_discretization(discretization_dict: dict) -> dict:
    _ensure_mapping(discretization_dict, "discretization")

    required = ["k_points", "dt_fs", "t_start_fs", "t_end_fs"]
    for key in required:
        if key not in discretization_dict:
            raise KeyError(f"discretization missing required key '{key}'")

    if not isinstance(discretization_dict["k_points"], int):
        raise TypeError("discretization.k_points must be an int")
    if discretization_dict["k_points"] <= 0:
        raise ValueError("discretization.k_points must be > 0")

    _ensure_number(discretization_dict["dt_fs"], "discretization.dt_fs")
    _ensure_number(discretization_dict["t_start_fs"], "discretization.t_start_fs")
    _ensure_number(discretization_dict["t_end_fs"], "discretization.t_end_fs")

    if discretization_dict["dt_fs"] <= 0:
        raise ValueError("discretization.dt_fs must be > 0")

    if discretization_dict["t_end_fs"] <= discretization_dict["t_start_fs"]:
        raise ValueError("discretization.t_end_fs must be greater than discretization.t_start_fs")

    return discretization_dict


def validate_fields(field_dict: dict) -> dict:
    required_groups = ["optical", "dc"]
    _ensure_mapping(field_dict, "fields")

    for key in required_groups:
        if key not in field_dict:
            raise KeyError(f"fields missing required key '{key}'")

        group = field_dict[key]
        if not isinstance(group, list):
            raise TypeError(f"fields.{key} must be a list")

        if len(group) == 0:
            raise ValueError(f"fields.{key} must not be empty")

        seen_ids = set()

        for i, field in enumerate(group):
            if not isinstance(field, dict):
                raise TypeError(f"fields.{key}[{i}] must be a dict")

            if "id" not in field:
                raise KeyError(f"fields.{key}[{i}] missing required key 'id'")
            _ensure_string(field["id"], f"fields.{key}[{i}].id")
            if field["id"] in seen_ids:
                raise ValueError(f"Duplicate id in fields.{key}: {field['id']}")
            seen_ids.add(field["id"])

            if "pulse_type" not in field:
                raise KeyError(f"fields.{key}[{i}] missing required key 'pulse_type'")
            _ensure_string(field["pulse_type"], f"fields.{key}[{i}].pulse_type")

            if "amplitude" not in field:
                raise KeyError(f"fields.{key}[{i}] missing required key 'amplitude'")
            _ensure_number(field["amplitude"], f"fields.{key}[{i}].amplitude")

            if key == "optical":
                for req in ["frequency", "duration_fs", "t0_fs"]:
                    if req not in field:
                        raise KeyError(f"fields.optical[{i}] missing required key '{req}'")
                    _ensure_number(field[req], f"fields.optical[{i}].{req}")

                if field["duration_fs"] <= 0:
                    raise ValueError(f"fields.optical[{i}].duration_fs must be > 0")

            if key == "dc":
                for req in ["t_on_fs", "t_off_fs"]:
                    if req not in field:
                        raise KeyError(f"fields.dc[{i}] missing required key '{req}'")
                    _ensure_number(field[req], f"fields.dc[{i}].{req}")

                if field["t_off_fs"] < field["t_on_fs"]:
                    raise ValueError(f"fields.dc[{i}].t_off_fs must be >= t_on_fs")

    return field_dict


def validate_sweep(sweep_dict: dict) -> dict:
    _ensure_mapping(sweep_dict, "sweep")

    required = ["strategy", "nest_by"]
    for key in required:
        if key not in sweep_dict:
            raise KeyError(f"sweep missing required key '{key}'")
        _ensure_string(sweep_dict[key], f"sweep.{key}")

    allowed_strategies = {"cartesian"}
    if sweep_dict["strategy"] not in allowed_strategies:
        raise ValueError(f"Unsupported sweep.strategy: {sweep_dict['strategy']}")

    allowed_nest_by = {"optical", "dc", "flat"}
    if sweep_dict["nest_by"] not in allowed_nest_by:
        raise ValueError(f"Unsupported sweep.nest_by: {sweep_dict['nest_by']}")

    return sweep_dict


def validate_output(output_dict: dict) -> dict:
    _ensure_mapping(output_dict, "output")

    required = ["root_dir", "format", "save_k_resolved", "save_macroscopic", "observables"]
    for key in required:
        if key not in output_dict:
            raise KeyError(f"output missing required key '{key}'")

    _ensure_string(output_dict["root_dir"], "output.root_dir")
    _ensure_string(output_dict["format"], "output.format")

    if not isinstance(output_dict["save_k_resolved"], bool):
        raise TypeError("output.save_k_resolved must be a bool")
    if not isinstance(output_dict["save_macroscopic"], bool):
        raise TypeError("output.save_macroscopic must be a bool")

    if not isinstance(output_dict["observables"], list):
        raise TypeError("output.observables must be a list")
    if len(output_dict["observables"]) == 0:
        raise ValueError("output.observables must not be empty")

    for i, item in enumerate(output_dict["observables"]):
        _ensure_string(item, f"output.observables[{i}]")

    return output_dict


def validate_config(config_dict: dict) -> dict:
    required_top = ["simulation", "discretization", "fields", "sweep", "output"]

    if not isinstance(config_dict, dict):
        raise TypeError("Top-level YAML structure must be a mapping (dict)")

    for key in required_top:
        if key not in config_dict:
            raise KeyError(f"Missing required section '{key}'")

    config_dict["simulation"] = validate_simulation(config_dict["simulation"])
    config_dict["discretization"] = validate_discretization(config_dict["discretization"])
    config_dict["fields"] = validate_fields(config_dict["fields"])
    config_dict["sweep"] = validate_sweep(config_dict["sweep"])
    config_dict["output"] = validate_output(config_dict["output"])

    if "interactions" in config_dict and not isinstance(config_dict["interactions"], dict):
        raise TypeError("interactions must be a mapping (dict)")

    if "analysis" in config_dict and not isinstance(config_dict["analysis"], dict):
        raise TypeError("analysis must be a mapping (dict)")

    return config_dict