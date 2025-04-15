#include <iostream>
#include <vector>
#include <regex>
#include <filesystem>
#include "regex_utils.h"
#include "jack_tokenizer.h"
#include "compilation_engine.h"

#pragma once



namespace fs = std::filesystem;

//This class serves as the point for all other classes that constitutes the jack compiler
class jack_analyzer
{
private:
    std::vector<std::string> file_name; //stores the file name
    bool file_or_not; //stores the directory name


public:
    jack_analyzer(std::string file_name);
};