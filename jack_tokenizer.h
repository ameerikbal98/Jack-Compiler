#pragma once

#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <algorithm>



enum token_type
{
    KEYWORD,SYMBOL,IDENTIFIER,INT_CONST,STRING_CONST,ERROR
};
const std::string token_names[] {"KEYWORD","SYMBOL","IDENTIFIER","INTEGER_CONSTANT","STRING_CONSTANT"};
//======================================================================================================================================================
enum keyword_type
{
    CLASS,METHOD,FUNCTION,CONSTRUCTOR,INT,BOOLEAN,CHAR,VOID,VAR,STATIC,FIELD,LET,DO,IF,ELSE,WHILE,RETURN,TRUE,FALSE,NULL_TYPE,THIS,ERROR_TYPE
};

const std::string keywords[] {"class","method","function","constructor","int","boolean","char","void","var","static","field","let","do","if","else","while","return","true","false","null","this"};
//===============================================================================================

struct token
{
    token_type type;
    std::string token_name;
    int line_num;

    void print()
    {
        std::cout << type << " " << token_name << " " << line_num << std::endl;
    }

};


class jack_tokenizer
{   
private:
    std::ifstream filehandle;
    std::vector<token> token_list;
    std::string file_str;
    std::string line;
    int line_num; //stores the current line being processed;
    int current_char_pos; //strores the current character position within a line

    //_______________________________________________________________________________________

    int current_token = 0;

    //----------------------------------------------------------------------------------------


public:
    jack_tokenizer(std::string); //this class takes the file as a string

    void print();

    bool has_more_token()
    {
        return !(current_token == token_list.size());
    }
    
    token_type return_token_type()
    {
        return token_list[current_token].type;
    } 

    keyword_type return_keyword_type()
    {
        auto it = std::find(cbegin(keywords), cend(keywords), token_list[current_token].token_name);
        return static_cast<keyword_type>(it - cbegin(keywords));
    }

    char return_symbol()
    {
        return token_list[current_token].token_name[0];
    }
    

    std::string return_identifier_string_const() // this function can be called for both identifier and string constant
    {
        return token_list[current_token].token_name;
    }
    int return_integer()
    {
        return std::stoi(token_list[current_token].token_name);
    }

    void advance()
    {
        current_token += 1;
    }

private:
    //private tokenizing function functions which are called from the constructor
    void tokenize();
    char get_char();//This function returns the next character until the last read;
    std::string get_string_const(); //this function returns the a string until the provided character is found
    bool EOF_(); // returns whether the file handle have reached the end of file
    std::string get_num(); //return a number in case a numebr is found in the stream
    bool check_comment(); // checks if the subsequent characters are comment or not if a comment is found it will consume the characters until the end of character
    std::string get_identifier();
};