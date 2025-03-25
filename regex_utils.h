#pragma once
#include<regex>

#include<vector>


namespace regex_utils
{

    std::vector<std::string> line_vectorize(std::string str, std::regex reg, int num); //returns the nth matches

    bool check_regex_str_exist(std::string str,std::regex reg,std::string check_str,int num); //returns true or false for regexes that nth matches a single time with the  


}