#pragma once

#include <string>
#include <fstream>
#include <iostream>
#include <sstream>



enum token_type
{
    KEYWORD,SYMBOL,IDENTIFIER,INT_CONST,STRING_CONST,ERROR
};
// enum class keyword_type
// {
//     CLASS,METHOD,FUNCTION,CONSTRUCTOR,INT,BOOLEAN,CHAR,VOID,VAR,STATIC,FIELD,LET,DO,IF,ELSE,WHILE,RETURN,TRUE,FALSE,NULL_TYPE,THIS,ERROR_TYPE
// };

struct token
{
    token_type type;
    std::string token_name;
    int line_num;

    void print()
    {
        std::cout << token_name << " " << line_num << std::endl;
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

public:
    jack_tokenizer(std::string); //this class takes the file as a string

    void print();

private:
    //private tokenizing function functions which are called from the constructor
    void tokenize();
    char get_char();//This function returns the next character until the last read;
    char peek_char();
    std::string get_string_const(); //this function returns the a string until the provided character is found
    bool EOF_(); // returns whether the file handle have reached the end of file
    std::string get_num(); //return a number in case a numebr is found in the stream
    bool check_comment(); // checks if the subsequent characters are comment or not if a comment is found it will consume the characters until the end of character
    std::string get_identifier();
};