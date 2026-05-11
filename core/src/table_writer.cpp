#include "table_writer.hpp" 
#include <cstddef>
#include <filesystem>
#include <stdexcept>
#include <iomanip>
#include <fstream>
#include <vector>

namespace{

    void validate_columns(const std::vector<Column>& columns)
    {
        // Check whether the columns are empty 
        if (columns.empty()){

            throw std::invalid_argument("write_table_tsv: there are no columns to write");
        }

        size_t n = columns[0].values.size(); 

        if(n == 0)
        {
            throw std::invalid_argument("write_table_tsv: column length is zero");
        }

        for(const auto& col : columns){

            if(col.values.size() != n){

                throw std::invalid_argument("write_table_tsv: column size mismatch");
            }
        }

    }

    void write_header(
        const std::vector<Column>& columns,
        std::ofstream& out
    )
    {
        for(std::size_t j = 0; j < columns.size(); ++j)
        {
            out << columns[j].name;

            if(j+1 < columns.size())
            {
                out<<"\t";
            }//end of if



        }//end of for

        out<<"\n";
    }

    void write_rows(
        const std::vector<Column>& columns,
        std::ofstream& out
    )
    {
       const std::size_t n = columns[0].values.size();

        for(std::size_t i = 0; i < n; ++i){

            for(std::size_t j = 0; j < columns.size(); ++j){

                out << columns[j].values[i];
                
                if(j+1 < columns.size()){
                    out<< "\t";
                }


            } //end of for loop iterate through each row
            out<<"\n";

        }//end of for loop iterate through each value
    }//write_rows_end



    

}//namespace end

void write_table_tsv(
    const std::vector<Column>& columns,
    const std::filesystem::path& file_path
)

{
    validate_columns(columns); //validation done here 
    
    std::filesystem::path parent = file_path.parent_path();
    if(!parent.empty()){
        std::filesystem::create_directories(parent);
    }

    std::ofstream out(file_path);

    if(!out)
    {
        throw std::runtime_error(
            "write_table_tsv: failed to open output file " + file_path.string()
        );
    }

    out<<std::setprecision(16);

    write_header(columns,out);
    write_rows(columns,out);
}