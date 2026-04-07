#include "engine.hpp"

#include <iostream>
#include <string>
#include <filesystem>

namespace fs = std::filesystem;

int main(int argc, char**   argv){

    if (argc != 3) {
    
        std::cerr<<"Usage: sbe_engine <input.json> <output_dir> \n";

        return 1;

    } 

    fs::path input_file = argv[1];
    fs::path output_dir = argv[2];

    try {
        Engine engine(input_file, output_dir);
        engine.run();
        return 0;
    }

    catch(const std::exception& e){
        std::cerr<<"Error: "<<e.what()<<"\n";
        return 1;
    }
    



    return 0;
}