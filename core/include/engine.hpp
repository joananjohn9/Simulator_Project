#pragma once 
#include<string>
#include<filesystem>



class Engine {

public : 

    Engine(const std::string& input_json, const std::string& output_dir);

    void run();
    

private:
    std::string input_json_;
    std::string output_dir_; 
    std::filesystem::path sim_output_dir_;




};
