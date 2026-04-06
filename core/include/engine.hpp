#pragma once 

#include<string>
#include<filesystem>

#include "config_loader.hpp"

namespace fs = std::filesystem;

class Engine {

    public :

        //Constructor: takes input JSON path and output directory
        Engine (const std::string& input_json_path,
            const std::string& output_json_path);
        
        // Entry poit for the main execution 
        void run ();

    private:

            //----- Paths -----
            fs::path input_json_; //path to input.json
            fs::path output_dir_; // root output directory
            fs::path sim_output_dir_; //actual simulation output directory

            //------ Configuration ------
            EngineConfig config_;

            //------- Internal helpers --------

            void setup_paths();
            void load_config();

            



};
