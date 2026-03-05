from pathlib import Path
import yaml 
import json
import subprocess 



def check_path(path : Path) -> Path:

    if not path.exists() :
        raise FileNotFoundError(f"Configuration file doesn't exists at {path}.") 
    
    if not path.is_file() : 
        raise IsADirectoryError(f"{path} is not a file")
    
    return path     

def parse_yaml(path: Path) -> dict:

    
    try :
        #Open the path as file 
        with path.open("r", encoding="utf-8") as file:

            #Load data from the file to a dict 
            data = yaml.safe_load(file)
    # If there is any YAML Error
    except yaml.YAMLError as e: 
        #Raise value error 
        raise ValueError(f"Invalid yaml syntax in {path}") from e 
    
    #Check data has any values 
    if data is None:
        raise ValueError(f"Yaml file is empty")

    #Check whether data is a dict 
    if not isinstance(data, dict):
        raise ValueError(f"Top level yaml structure must be a mapping :{path}") 
    
    return data 


def validate_yaml(config_dict: dict) -> dict:

    REQUIRED_KEYS = ["simulation", "discretization","fields","output"]

    if not isinstance(config_dict, dict):
        raise ValueError(f"Top level yaml structure must be a mapping ")

    for key in REQUIRED_KEYS:
        
        if key not in config_dict:
            raise KeyError(key)
        
        if not isinstance(config_dict[key], dict):
            raise TypeError(f"Value corresponding to {key} is of wrong type. Expected dict got {type(config_dict[key])}")

    return config_dict


def make_json(validated_config_dict: dict, output_dir:Path) -> Path:

    '''
    This function takes the dictionary and output path and makes json and output directory
    '''
    
    #Make path for the output directory 
    output_dir.mkdir(parents=True, exist_ok=True)
    json_path = output_dir/"input.json"


    # Get the values of keys from the dict
    simulation = validated_config_dict["simulation"]
    discretization = validated_config_dict["discretization"]
    interactions = validated_config_dict.get("interactions", {})
    fields_cfg = validated_config_dict["fields"]
    output_cfg = validated_config_dict["output"]

    # Normalize fields into list 

    fields = [] 

    if "optical" in fields_cfg:
        pulse = fields_cfg["optical"]["pulse"]
        fields.append(
            {
                "kind" : "optical",
                "pulse_type": pulse["type"],
                "amplitude" : pulse["amplitude"],
                "frequency" : pulse["frequency"],
                "duration_fs": pulse["duration_fs"],
                "t0_fs" : pulse["t0_fs"]
            }
        )

    if "dc" in fields_cfg:
        pulse = fields_cfg["dc"]["pulse"]
        fields.append(
            {
                "kind" : "dc",
                "pulse_type": pulse["type"],
                "amplitude" : pulse["amplitude"],
                "t_on_fs" : pulse["t_on_fs"],
                "t_off_fs": pulse["t_off_fs"]
            }
        )

    engine_json = {
        "schema_version" : "0.1",
        "model" : simulation["model"],
        "order_expansion" : simulation["order_expansion"],
        "grid":{
            "k_points" : discretization["k_points"],
            "dt_fs" : discretization["dt_fs"],
            "t_start_fs": discretization["t_start_fs"],
            "t_end_fs": discretization["t_end_fs"]

        },
        "interactions": interactions,
        "fields" : fields,
        "output" : output_cfg
    }

    #Opening json file and writing it

    with json_path.open("w",encoding="utf-8") as file:
        json.dump(engine_json,file, indent= 2)


    return json_path


def pass_to_core(json_path:Path, output_dir:Path) -> None:
    
    #Find the project root 
    project_root = Path(__file__).resolve().parents[1] # .../sim
    engine_path = project_root/"core"/"build"/"bin"/"sbe_engine" # This is the path

    # Check if engine path exist 
    if not engine_path.exists():
        raise FileNotFoundError(
            f"Executable doesn't exists at {engine_path} \n."
            f"First build it first (CMake) so that core/build/bin/sbe_engine exists."
            )

    if not engine_path.is_file():
        raise FileNotFoundError(f"{engine_path} exists but its not a file") 

    if not (engine_path.stat().st_mode & 0o111):
        raise PermissionError(f"Permission denied. Try chmod +x {engine_path}")


    result = subprocess.run([str(engine_path), str(json_path), str(output_dir)], check=False) 
    
    if result.returncode != 0:
        raise RuntimeError(f"sbe_engine failed with exit code {result.returncode}")
    else :
        print("Engine run successful")

        






      

def run_command(args):
    
    # Getting path from the args
    path = Path(args.config)
    output_dir = Path(args.out)

    config_path = check_path(path) #Validate the path

    config_dict = parse_yaml(config_path) # Make the yaml file to a dict 

    validated_config_dict = validate_yaml(config_dict)

    json_path = make_json(validated_config_dict, output_dir)

    pass_to_core(json_path,output_dir) 

    

    
