def _require_keys(section:dict, required_keys:list[str], path:str) -> None:
    for key in required_keys:
        if key not in section:
            raise KeyError(f"{path} is missing required key '{key}'.")

def _reject_unknown_keys(section:dict, allowed_keys:list[str], path:str) -> None:
    for key in section:
        if key not in allowed_keys:
            raise KeyError(f"{path} has unknown key '{key}'.")

def _require_type(value,expected_type,path:str) -> None:
    if not isinstance(value,expected_type):
        if isinstance(expected_type, tuple):
            expected_name = "or".join(t.__name__ for t in expected_type)
        else :
            expected_name = expected_type.__name__
        raise TypeError(f"{path} must be {expected_name}, got {type(value).__name__}")

def validate_config(config_dict:dict) -> dict:
    required_keys = ["simulation","discretization","fields","sweep", "output"]

    _require_type(config_dict,dict,"top-level config")
    _require_keys(config_dict,required_keys, "config")

    for key in required_keys:
        _require_type(config_dict[key],dict,f"config.{key}")



    # Deeper validations    
    validate_simulation(config_dict["simulation"])
    validate_discretization(config_dict["discretization"])
    fields=validate_fields(config_dict["fields"])
    validate_optical_fields(fields["optical"])
    validate_dc_fields(fields["dc"])
    validate_sweep(config_dict["sweep"])
    validate_output(config_dict["output"])

    return config_dict

def validate_simulation(simulation_dict:dict) -> dict :
    required_keys_simulation = ["model","order_expansion"]
    required_keys_order_exp = ["enabled","max_order"]
    required_keys_model = ["type", "lattice_constant_A", "E_gap_eV"]

   

    _require_type(simulation_dict,dict,"simulation")
    _require_keys(simulation_dict,required_keys_simulation, "simulation")
    _reject_unknown_keys(simulation_dict,required_keys_simulation,"simulation")

    _require_type(simulation_dict["model"],dict,"simulation.model") 
    _require_keys(simulation_dict["model"],required_keys_model,"simulation.model")
    _require_type(simulation_dict["model"]["type"], str, "simulation.model.type")
    _require_type(simulation_dict["model"]["lattice_constant_A"],float,"simulation.model.lattice_constant")
    _require_type(simulation_dict["model"]["E_gap_eV"],float,"simulation.model.E_gap_eV")

    

    _require_type(simulation_dict["order_expansion"],dict, "simulation.order_expansion")
    _require_keys(simulation_dict["order_expansion"],required_keys_order_exp, "simulation.order_expansion")
    _reject_unknown_keys(simulation_dict["order_expansion"],required_keys_order_exp, "simulation.order_expansion")

    _require_type(simulation_dict["order_expansion"]["enabled"], bool, "simulation.order_expansion.enabled")
    _require_type(simulation_dict["order_expansion"]["max_order"], int, "simulation.order_expansion.max_order")

    if simulation_dict["order_expansion"]["max_order"] < 0:
        raise ValueError(
            f"simulation.order_expansion.max_order should be greater than or equal to zero"
        )

    if simulation_dict["model"]["type"] != "sbe_1d":
        raise ValueError(
            f"simulation.model must be 'sbe_1d', got '{simulation_dict['model']['type']}'"
        )

    return simulation_dict

def validate_discretization(discretization_dict: dict) -> dict:
    required_keys = ["k_points", "dt_fs", "t_start_fs", "t_end_fs"]

    _require_type(discretization_dict, dict, "discretization")
    _require_keys(discretization_dict, required_keys, "discretization")
    _reject_unknown_keys(discretization_dict, required_keys, "discretization")

    _require_type(discretization_dict["k_points"], int, "discretization.k_points")
    _require_type(discretization_dict["dt_fs"], (int, float), "discretization.dt_fs")
    _require_type(discretization_dict["t_start_fs"], (int, float), "discretization.t_start_fs")
    _require_type(discretization_dict["t_end_fs"], (int, float), "discretization.t_end_fs")

    if discretization_dict["k_points"] <= 0:
        raise ValueError("discretization.k_points must be greater than 0")

    if discretization_dict["dt_fs"] <= 0:
        raise ValueError("discretization.dt_fs must be greater than 0")

    if discretization_dict["t_end_fs"] <= discretization_dict["t_start_fs"]:
        raise ValueError(
            "discretization.t_end_fs must be greater than discretization.t_start_fs"
        )

    return discretization_dict

    



    

def validate_fields(field_dict:dict) -> dict:
    required_keys = ["optical","dc"]

    _require_type(field_dict,dict,"fields")
    _require_keys(field_dict,required_keys,"fields")
    _reject_unknown_keys(field_dict,required_keys,"fields")

    for key in required_keys:
        field_list =  field_dict[key]
        _require_type(field_list,list,f"fields.{key}")

        if len(field_list) < 1:
            raise ValueError(f"fields.{key} should not be empty")

        for i,field in enumerate(field_list):
            _require_type(field,dict,f"fields.{key}[{i}]")
    
    return field_dict

def validate_optical_fields(optical_fields:dict) -> dict:
    required_keys = ["id", "pulse_type", "amplitude","duration_fs","frequency","t0_fs"]
    allowed_keys = required_keys 

    for i, optical_field in enumerate(optical_fields):
        path = f"fields.optical[{i}]"

        _require_keys(optical_field,required_keys,path)
        _reject_unknown_keys(optical_field,allowed_keys,path) 

        _require_type(optical_field['id'],str,f"{path}.id")
        _require_type(optical_field['pulse_type'],str,f"{path}.pulse_type")
        _require_type(optical_field['amplitude'],(int,float), f"{path}.amplitude")
        _require_type(optical_field["duration_fs"], (int,float),f"{path}.duration_fs")
        _require_type(optical_field["t0_fs"],(int,float),f"{path}.t0_fs")
    
    return optical_fields

def validate_dc_fields(dc_fields:dict) -> dict:
    required_keys =  ["id", "pulse_type", "amplitude", "t_on_fs", "t_off_fs"]
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
