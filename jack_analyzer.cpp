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

    for(int i = 0; i < file_name.size();i++)
    {
        std::cout << file_name[i] << std::endl;
        jack_tokenizer jt{file_name[i]};
        while(jt.has_more_token())
        {
            std::cout << token_names[jt.return_token_type()] << " ";

            if(jt.return_token_type() == token_type::KEYWORD)
            {
                std::cout << keywords[jt.return_keyword_type()] << " ";
            }

            else if(jt.return_token_type() == token_type::INT_CONST)
            {
                std::cout << jt.return_integer() << " ";
            }

            else if(jt.return_token_type() == token_type::IDENTIFIER || jt.return_token_type() == token_type::STRING_CONST)
            {
                std::cout << jt.return_identifier_string_const() << " ";
            }

            else if(jt.return_token_type() == token_type::SYMBOL)
            {
                std::cout << jt.return_symbol() << " ";
            }
            std::cout << std::endl;
            jt.advance();
        }
    }
}

