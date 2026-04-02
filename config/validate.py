

# Top Level Validation 
def validate_config(config_dict: dict) -> dict:
    REQUIRED_KEYS= ["simulation", "discretization","fields","sweep", "output"]

    if not isinstance(config_dict, dict) :
        raise TypeError ("Top Level yaml structure must be a mapping (dict)")

    for key in REQUIRED_KEYS:
        if key not in config_dict:
            raise KeyError (f"Missing required section '{key}'" )

        value = config_dict[key]
        if not isinstance(value, dict):

            raise TypeError (f"{key} should be a mapping. Check config.yaml")

    
    # Calling Deeper validation functions here
    fields = validate_fields(config_dict["fields"])
    validate_optical_fields(fields["optical"])
    validate_dc_fields(fields["dc"])
    validate_sweep(config_dict['sweep'])

    return config_dict 


# Field validations
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
    return field_dict 


# Validating optical field 
def validate_optical_fields(optical_fields:list) -> list:

    #Defining required keys 

    REQUIRED_KEYS = ["id","pulse_type","amplitude","frequency",'duration_fs', 't0_fs']
    ALLOWED_KEYS = REQUIRED_KEYS

    for i, optical_field in enumerate(optical_fields): # loop through each optical field with index stored in i 

        #check for keys 
        for key in REQUIRED_KEYS:
            if key not in optical_field:
                raise KeyError (
                    f"fields.optical[{i}] missing required key '{key}'. Please check the config.yaml file "
                ) #checking for keys and raising key error
        
        #checking for unallowed keys
        for key in optical_field:
            if key not in ALLOWED_KEYS:
                raise KeyError(
                    f"fields.optical[{i}] has an unknown key '{key}'"
                )

        
        #check whether id is a string
        if not isinstance(optical_field["id"],str):
            raise TypeError(
                f"fields.optical[{i}].id must be a string, got {type(optical_field['id']).__name__}"
            )

        #check whether pulse_type is a string 
        if not isinstance(optical_field["pulse_type"],str):
            raise TypeError(
                f"fields.optical[{i}].pulse_type must be a string, got {type(optical_field['pulse_type']).__name__}"
            )

        #check whether amplitude is int or float
        if not isinstance(optical_field["amplitude"],(int,float)):
            raise TypeError(
                f"fields.optical[{i}].amplitude must be an int or float, got {type(optical_field['amplitude']).__name__}"
            )
        #check whether frequency is int or float
        if not isinstance(optical_field["frequency"],(int,float)):
            raise TypeError(
                f"fields.optical[{i}].frequency must be an int or float, got {type(optical_field['frequency']).__name__}"
            )
        #check whether duration is int or float
        if not isinstance(optical_field["duration_fs"],(int,float)):
            raise TypeError(
                f"fields.optical[{i}].duration_fs must be an int or float, got {type(optical_field['duration_fs']).__name__}"
            )
        #check whether t0_fs is int or float
        if not isinstance(optical_field["t0_fs"],(int,float)):
            raise TypeError(
                f"fields.optical[{i}].t0_fs must be an int or float, got {type(optical_field['t0_fs']).__name__}"
            )
        
    return optical_fields

def validate_dc_fields(dc_fields:list) -> list:
    #Defining Required keys
    REQUIRED_KEYS = ['id', 'pulse_type', 'amplitude','t_on_fs','t_off_fs']

    ALLOWED_KEYS = REQUIRED_KEYS


    # Looping through each dc field 
    for i,dc_field in enumerate(dc_fields):

        #checking for keys
        for key in REQUIRED_KEYS:

            if key not in dc_field:
                raise KeyError(
                    f"fields.dc[{i}] missing required key '{key}'. Please check config.yaml file"
                )  # key check complete 
        
        #checking for unallowed keys
        for key in dc_field:
            if key not in ALLOWED_KEYS:
                raise KeyError(
                    f"fields.dc[{i}] has an unknown key '{key}'"
                )

        
        # checking whether id is a string
        
        if not isinstance(dc_field['id'],str):
            raise TypeError(
                f"fields.dc[{i}].id must be a string, got {type(dc_field['id']).__name__}"
        )

        #checking whether pulse_type is a string
        if not isinstance(dc_field['pulse_type'],str):
            raise TypeError(
                f"fields.dc[{i}].pulse_type must be a string, got {type(dc_field['pulse_type']).__name__}"
            )

        #checking amplitude is an int or float 
        if not isinstance(dc_field['amplitude'],(int,float)):
            raise TypeError(
                f"fields.dc[{i}].amplitude must be an integer or a float, got {type(dc_field['amplitude']).__name__}"
            )


        #checking on time is an int or float 
        if not isinstance(dc_field['t_on_fs'],(int,float)):
            raise TypeError(
                f"fields.dc[{i}].t_on_fs must be an integer or a float, got {type(dc_field['t_on_fs']).__name__}"
            )

        #checking off time is an int or float 
        if not isinstance(dc_field['t_off_fs'],(int,float)):
            raise TypeError(
                f"fields.dc[{i}].t_off_fs must be an integer or a float, got {type(dc_field['t_off_fs']).__name__}"
            )
    return dc_fields

def validate_sweep(sweep:dict)->dict:
    #Defining required keys 
    REQUIRED_KEYS = ['strategy', 'nest_by']
    ALLOWED_KEYS = REQUIRED_KEYS
    ALLOWED_STRATEGIES = ['cartesian']
    ALLOWED_NESTING = ['optical','dc','flat']

    #checking for required keys
    for key in REQUIRED_KEYS: 
        if key not in sweep:
            raise KeyError(
                f"Missing key '{key}' in sweep structure. Please check config.yaml"
            )
    #checking for allowed keys
    for key in sweep:
        if key not in ALLOWED_KEYS:
            raise KeyError(
                f"Unknown key '{key}'. Please check config.yaml"
            )
        #checking types 
        if not isinstance(sweep[key],str):
            raise TypeError(
                f"sweep.{key} must be a string, got {type(sweep[key]).__name__}"
            )

    #checking values

    #strategy
    if sweep["strategy"] not in ALLOWED_STRATEGIES:
        raise ValueError(
            f"sweep.strategy must be one of {ALLOWED_STRATEGIES}, got {sweep['strategy']}"
        )
    #nesting
    if sweep["nest_by"] not in ALLOWED_NESTING:
            raise ValueError(
            f"sweep.nest_by must be one of {ALLOWED_NESTING}, got {sweep['nest_by']}"
        )
            

    return sweep


#Validating output

def validate_output(output:dict)->dict:
    pass
    

    
