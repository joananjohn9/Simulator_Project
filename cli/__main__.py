import argparse
from cli.init import init_config 
from cli.run import run_command


def main(): 

    parser = argparse.ArgumentParser(
        prog="sim",
        description="Semiconductor Bloch Equation Simulation tool")
    
    subparsers = parser.add_subparsers(
        dest="command",
        required=True
    )

    #Init parser

    init_parser = subparsers.add_parser(
        "init",
        help="Create a default configuration file"

    )

    init_parser.add_argument(
        "config",
        nargs="?", # zero to one argument nargs is number of arguments
        default="config.yaml", # default value given
        help="Path to config.yaml file (Default: config.yaml)"
    )



    # Run parser

    run_parser = subparsers.add_parser(
        "run",
        help="Run the simulation from configuration file",
    )

    run_parser.add_argument(
        "config",
        help="Path to config.yaml"
    )

    run_parser.add_argument(
        "--out",
        default="output",
        help="Specify the output directory. (Default: ./output)"

    )


    args=parser.parse_args() 


    if args.command == "init":
        init_config(args.config)

    elif args.command=="run":
        run_command(args)

    else:
        parser.error(f"Unknown command {args.command}")




if __name__ == "__main__":
    main()
        
    
        