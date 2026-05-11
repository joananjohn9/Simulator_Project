#pragma once 

#include <vector>
#include <string>
#include<filesystem>
 

struct Column {
    
    std::string name;
    std::vector<double> values;
 
};

void write_table_tsv(
    const std::vector<Column>& columns,
    const std::filesystem::path& file_path
);