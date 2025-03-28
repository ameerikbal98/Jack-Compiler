#pragma once

#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <algorithm>



enum token_type
{
    KEYWORD,SYMBOL,IDENTIFIER,INT_CONST,STRING_CONST
};
const std::string token_names[] {"KEYWORD","SYMBOL","IDENTIFIER","INTEGER_CONSTANT","STRING_CONSTANT"};
//======================================================================================================================================================
enum keyword_type
{
    CLASS,METHOD,FUNCTION,CONSTRUCTOR,INT,BOOLEAN,CHAR,VOID,VAR,STATIC,FIELD,LET,DO,IF,ELSE,WHILE,RETURN,TRUE,FALSE,NULL_TYPE,THIS
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
    jack_tokenizer(){}
    jack_tokenizer(std::string); //this class takes the file as a string

    jack_tokenizer(jack_tokenizer& jt) : token_list{jt.token_list},file_str {jt.file_str},line {jt.line},line_num {jt.line_num},current_char_pos {jt.current_char_pos}
    {}

    jack_tokenizer(jack_tokenizer&& jt) : token_list {std::move(jt.token_list)},file_str {jt.file_str},line {jt.line},line_num {jt.line_num},current_char_pos {jt.current_char_pos}
    {}

    jack_tokenizer& operator= (jack_tokenizer& jt)
    {
        if(this != &jt)
        {
            token_list = jt.token_list;
            file_str = jt.file_str;
            line = jt.line;
            line_num = jt.line_num;
            current_char_pos = jt.current_char_pos;
        }
        return *this;
    }

    jack_tokenizer& operator= (jack_tokenizer&& jt) noexcept
    {
        if(this != &jt)
        {
            token_list = std::move(jt.token_list);
            file_str = std::move(jt.file_str);
            line = jt.line;
            line_num = jt.line_num;
            current_char_pos = jt.current_char_pos;
        }
        return *this;
    }

    ~jack_tokenizer() 
    {
        if (filehandle.is_open()) 
        {
            filehandle.close();
        }
    }

    void print();//it will print the entire tokens

    bool has_more_token() //check if there is any more token
    {
        return !(current_token == token_list.size());
    }
    
    token_type return_token_type() //returns the type of token
    {
        return token_list[current_token].type;
    } 

    keyword_type return_keyword_type()  //should only be called when token is a keyword
    {
        auto it = std::find(cbegin(keywords), cend(keywords), token_list[current_token].token_name);
        return static_cast<keyword_type>(it - cbegin(keywords));
    }

    char return_symbol() //only to be called when the token is a symbol
    {
        return token_list[current_token].token_name[0];
    }
    

    std::string return_identifier_string_const() // this function can be called for both identifier and string constant
    {
        return token_list[current_token].token_name;
    }
    int return_integer() //should only be called when the token is an integer constant
    {
        return std::stoi(token_list[current_token].token_name);
    }

    void advance() //advances the object to the next token
    {
        current_token += 1;
    }

    int return_linenum() //returns the line number of the token as found in the original file
    {
        return token_list[current_token].line_num;
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