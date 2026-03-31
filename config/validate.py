
def validate_config(config_dict: dict) -> dict:
    REQUIRED_KEYS= ["simulation", "discretization","fields","sweep", "output"]

    if not isinstance(config_dict, dict) :
        raise TypeError ("Top Level yaml structure must be a mapping (dict)")

    for key in REQUIRED_KEYS:
        if key not in config_dict:
            raise KeyError (f"Missing Required section {key}" )

        value = config_dict[key]
        if not isinstance(value, dict):

            raise TypeError (f"{key} should be a mapping. Check config.yaml")
    
    return config_dict 

def validate_fields(field_dict: dict) -> dict:
    REQUIRED_KEYS = ["optical", "dc"]

    if not isinstance(field_dict, dict):
        raise TypeError("fields must be a mapping (dict)")

    for key in REQUIRED_KEYS:
        if key not in field_dict:
            raise KeyError(f"fields missing required key '{key}'")

        field_type_value = field_dict[key]

        if not isinstance(field_type_value, list):
            raise TypeError(
                f"fields.{key} must be a list, got {type(field_type_value).__name__}"
            )

        if len(field_type_value) < 1:
            raise ValueError(f"fields.{key} should not be empty")

        for i, field in enumerate(field_type_value):
            if not isinstance(field, dict):
                raise TypeError(
                    f"fields.{key}[{i}] must be a dict, got {type(field).__name__}"
                )

    return field_dic