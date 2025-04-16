#include <iostream>
#include <vector>
#include <regex>
#include <filesystem>
#include "jack_tokenizer.h"
#include "compilation_engine.h"

#pragma once



namespace fs = std::filesystem;

//This class serves as the point for all other classes that constitutes the jack compiler
class jack_analyzer
{
private:
    std::vector<std::string> file_name; //stores the file name
    std::vector<std::string> tokenizer_file_names;
    std::vector<std::string> parser_file_names;
    bool file_or_not; //stores the directory name
    std::ofstream token_filehandle;
    std::ofstream parser_filehandle;


public:
    jack_analyzer(std::string file_name);
    void analyze();
};