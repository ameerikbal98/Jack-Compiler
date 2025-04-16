#include<filesystem>
#include <fstream>
#include "regex_utils.h"
#include "jack_tokenizer.h"


//==================================================================================================================================================================
//From now own its all tokenizer and its associated functions---------------------------------------------------------------------------------------------
/*
Grammer


*/

namespace tokenizer
{

void jack_tokenizer::tokenize() // main tokenizing function
{
    char c;
    
    while(!EOF_())
    {
        c = get_char();
        switch (c)
        {
            case '{': case '}': case '(': case ')': case ';': case '.': case '[': case ']': case ',':
            case '+': case '=': case '*': case '&': case '|': case '<': case '>': case '-': case '~':
                token_list.push_back(token{token_type::SYMBOL,std::string{c},line_num});
                break;

            case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
            {
                std::string num = get_num();
                token_list.push_back(token{token_type::INT_CONST,num,line_num});
                break;
            }

            case '"':
            {
                std::string str = get_string_const();

                token_list.push_back(token{token_type::STRING_CONST,str,line_num});

                break;

            }

            case '/' : // '/' symbol can signifies division or comments so special case
            {
                if(!check_comment())
                {
                    token_list.push_back(token{token_type::SYMBOL,std::string{c},line_num});
                }
                break;
            }

            case ' ': case '\t': case '\n': case '\r':
                break; // do nothing if space comes up

            default:
            {
                if(isalpha(c)|| c=='_')
                {
                    std::string str;
                    str = get_identifier(); // this will get an identifier
                    if(str == "class" || str == "constructor" || str == "function" || str == "method" || str == "field" || str == "static" || str == "var" ||
                    str == "int" || str == "char" || str == "boolean" || str == "void" || str == "true" || str == "false" || str == "null" || str == "this" ||
                        str == "let" || str == "do" || str == "if" || str == "else" || str == "while" || str == "return")
                    {
                        token_list.push_back(token{token_type::KEYWORD,str,line_num}); //this is an token
                    }
                    else
                    {
                        token_list.push_back(token{token_type::IDENTIFIER,str,line_num}); //this is an identifier
                    }
                }
                else
                {
                    std::cerr << "Error at line: char " << int(c) <<" " << line_num << std::endl;
                }
                break;
            }
        

        }

    }
}

//====================================================================================================================================================================
char jack_tokenizer::get_char() //returns next character from the file
{
    char c;
    if(current_char_pos == line.length()) 
    {
        line = "";
        while(line == "")  // loop until we get a non empty line
        {
            if(filehandle.peek() == EOF) //if we reach the end of file we return a space and set the current char pos to zero so the tokenizer loop will
                                        // in the next iteration as EOF_() will return true
            {
                current_char_pos = 0;
                return ' ';
            }
            std::getline(filehandle,line);
            line_num+=1;
            
        }
        current_char_pos = 0; //set the position to 0
        
    }
    c = line[current_char_pos];
    current_char_pos += 1;
    return c;
    
}


//=========================================================================================================================================================================
bool jack_tokenizer::EOF_() //returns whether the file have reached the end of file
{
    bool flag = false;
    if(current_char_pos == line.length() && filehandle.peek() == EOF)
    {
        flag =  true; //returns true if its the end of file
    }
    return flag;
}


//========================================================================================================================================================================

std::string jack_tokenizer::get_num()
{
    std::string num;
    current_char_pos -= 1; //as the we need to get back the first character which is already consumed by the tokenizer
    char c = get_char();
    while(isdigit(c))
    {
        num+=c;
        c = get_char();
    }   
    current_char_pos -= 1; //returns the last character
    return num;
}

//=======================================================================================================================================================================
std::string jack_tokenizer::get_string_const() //this function returns the a string until the provided character is found
{
    std::string str;
    char c = get_char();
    while(c != '"')
    {
        str += c;
        c = get_char();
    }
    return str;
}

//=======================================================================================================================================================================
bool jack_tokenizer::check_comment()
{
    bool flag = 0;
    char c = get_char();
    if(c == '/') // if a second / symbol comes up we ended up neglecting the entire line
    {
        current_char_pos = line.length(); //this will skip the entire line
        flag = 1;
    }
    else if (c == '*') // if a * comes up after the / we we will scan until the */ comes up and will ignore it 
    {
        flag = 1;
        char c2;
        while(true)
        {
            if(EOF_())
            {
                std::cerr << "Commend end not found" << std::endl;
                exit(0);
            }
            c = get_char();
            
            if(c =='*')
            {
                c = get_char();
                if(c == '/')
                    break;
            }
        }
    }
    else
    {
        current_char_pos -= 1; //if not comment return the last consumed character
    }
    return flag;
}



//========================================================================================================================================================================

std::string jack_tokenizer::get_identifier()
{
    std::string str;
    current_char_pos -= 1; //first character already consumed by the tokenizer
    char c = get_char();
    while(isalnum(c) || c == '_')
    {
        str += c;
        c = get_char();
    }
    current_char_pos -= 1; //returns the last character
    return str;
}

//==================================================================================================================================================================

void jack_tokenizer::print()
{
    for (int i = 0; i < token_list.size(); i++)
    {
        token_list[i].print();
    }
}

}