#include<iostream>
#include <filesystem>
#include "jack_analyzer.h"
#include "regex_utils.h"



namespace fs = std::filesystem;
using jack_tokenizer = tokenizer::jack_tokenizer;

jack_analyzer::jack_analyzer(std::string name)
{
    file_or_not = regex_utils::check_regex_str_exist(name,std::regex("\\.jack$"),".jack",0); //returns if the the name have .jack extension or not if it doesn't its a directory
    
    if(file_or_not)
    {
        file_name.push_back(name);
        
        for(int i = 0,length = name.length(); i < length; i++)
        {
            tokenizer_file_names.push_back("");
            parser_file_names.push_back("");
            if(!(name[i]=='.' && name[i+1] == 'j' && name[i+2] == 'a' && name[i+3] == 'c' && name[i+4] == 'k'))
            {
                tokenizer_file_names[0].push_back(name[i]);
                parser_file_names[0].push_back(name[i]);
            }
            else
            {
                break;
            }
            
        }
        tokenizer_file_names[0].append("T.xml");
        parser_file_names[0].append(".xml");
    }
    else
    {
        for (const auto & entry : fs::directory_iterator(name))
        {
            if(regex_utils::check_regex_str_exist(entry.path(),std::regex("(\\.jack$)"),".jack",0))
            {
                file_name.push_back(entry.path());
                std::string fname = entry.path();
                int current_str = file_name.size() - 1;
                tokenizer_file_names.push_back("");
                parser_file_names.push_back("");
                for(int i = 0,length = file_name[current_str].length(); i < length; i++)
                {
                    
                    if(!(fname[i]=='.' && fname[i+1] == 'j' && fname[i+2] == 'a' && fname[i+3] == 'c' && fname[i+4] == 'k'))
                    {
                        tokenizer_file_names[current_str].append(std::string{fname[i]});
                        parser_file_names[current_str].append(std::string{fname[i]});
                    }
                    else
                    {
                        break;
                    }
                }
                tokenizer_file_names[current_str].append("T.xml");
                parser_file_names[current_str].append(".xml");
            }
        }
    }
    for(int i = 0; i < file_name.size();i++)
    {
        std::cout << file_name[i] << std::endl;
    }

    
}

void jack_analyzer::analyze()
{
    for(int i = 0; i < file_name.size();i++)
    {
        jack_tokenizer jt{file_name[i]};
        std::cout << file_name[i] << std::endl;
        token_filehandle.open(tokenizer_file_names[i]);
        parser_filehandle.open(parser_file_names[i]);
        std::string tokenizer_string{""};
        tokenizer_string.append("<token>\n");
        while(jt.has_more_token())
        {  
            if(jt.return_token_type() == tokenizer::token_type::KEYWORD)
            {
                tokenizer_string.append(std::string("\t<keyword>") + tokenizer::KEYWORDS[jt.return_keyword_type()]+"</keyword>\n");
            }

            else if(jt.return_token_type() == tokenizer::token_type::INT_CONST)
            {
                tokenizer_string.append(std::string("\t<integerConstant>") + std::to_string(jt.return_integer()) +"</integerConstant>\n");
            }

            else if(jt.return_token_type() == tokenizer::token_type::IDENTIFIER)
            {
                tokenizer_string.append(std::string("\t<identifier>") + jt.return_identifier_string_const() +"</identifier>\n");
            }
            else if(jt.return_token_type() == tokenizer::token_type::STRING_CONST)
            {
                tokenizer_string.append(std::string("\t<stringConstant>") + jt.return_identifier_string_const() +"</stringConstant>\n");
            }
            else if(jt.return_token_type() == tokenizer::token_type::SYMBOL)
            {
                if(jt.return_symbol() == '<')
                {
                    tokenizer_string.append(std::string("\t<symbol>")+"&lt;"+"</symbol>\n");
                }
                else if(jt.return_symbol() == '>')
                {
                    tokenizer_string.append(std::string("\t<symbol>")+"&gt;"+"</symbol>\n");
                }
                else
                {
                    tokenizer_string.append(std::string("\t<symbol>")+jt.return_symbol()+"</symbol>\n");
                }
            }
            jt.advance();
        }
        tokenizer_string.append("</token>\n");
        jt.reset_token_seeker();
        token_filehandle << tokenizer_string;
        compilation_engine engine{jt};
        engine.compile();
        parser_filehandle << engine.return_parse_string();
        parser_filehandle.close();
        token_filehandle.close();
        
    }
}

