#include<regex>
#include <iostream>
#include "regex_utils.h"

using namespace std;

std::vector<std::string> regex_utils::line_vectorize(std::string str, std::regex reg, int num) //returns the nth matches
{

    std::vector<std::string> matches_vec;

    // Used when your searching a string
    std::smatch matches;
    
    // Determines if there is a match and match 
    // results are returned in matches
    while(std::regex_search(str, matches, reg))
    {
        
        // Get the first match
            matches_vec.push_back(matches.str(num));
        
        // Eliminate the previous match and create
        // a new string to search
        str = matches.suffix().str();
    }

    return matches_vec;
}

bool regex_utils::check_regex_str_exist(std::string str,std::regex reg,std::string check_str,int num) //returns true or false for regexes that returns the given string 
{

    std::smatch match;

    std::regex_search(str,match,reg);

    return (match.str(num) == check_str);
}