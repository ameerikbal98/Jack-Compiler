#include<iostream>
#include <filesystem>
#include "jack_analyzer.h"


namespace fs = std::filesystem;

jack_analyzer::jack_analyzer(std::string name)
{
    file_or_not = regex_utils::check_regex_str_exist(name,std::regex("\\.jack$"),".jack",0); //returns if the the name have .jack extension or not if it doesn't its a directory
    std::cout << std::boolalpha;
    if(file_or_not)
    {
        file_name.push_back(name);
        std::cout << name << std::endl;
    }
    else
    {
        for (const auto & entry : fs::directory_iterator(name))
        {
            if(regex_utils::check_regex_str_exist(entry.path(),std::regex("(\\.jack$)"),".jack",0))
            {
                file_name.push_back(entry.path()); 
            }
        }
    }

    jack_tokenizer jt{file_name[0]};
    jt.print();
}


