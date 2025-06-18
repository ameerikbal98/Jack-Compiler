#pragma once
#include "jack_tokenizer.h"
#include "vm_writer.h"
#include "symbol_table.h"
#include <string>
//this is a recursive descent parser

struct xml_string //stores the each tags in the xml 
{
    std::string xml_string;
    void enter_tag(std::string token_element,std::string token_name,int tabs)
    {
        for(int i = 0; i < tabs; i++)
        {
            xml_string.append("\t");
        }
        xml_string.append(std::string("<") + token_element + "> " + token_name + " </" + token_element + ">");
        xml_string.append("\n");
    }

    void enter_tag(std::string token_element, int tabs)
    {
        for(int i = 0; i < tabs; i++)
        {
            xml_string.append("\t");
        }
        xml_string.append(std::string("<") + token_element + ">");
        xml_string.append("\n");
    }

    void reset()
    {
        xml_string = "";
    }
};

enum subroutine_type
{
    constructor,function,method
};

class compilation_engine
{
private:
    tokenizer::jack_tokenizer jt;
    vm_writer vm_wr;
    symbol_table symboltable_class{'c'};
    symbol_table symboltable_subroutine{'s'};
    std::string class_name;
    xml_string xs;
    int tab_count = 0;
    subroutine_type sub_type;
    std::string current_subroutine_name;
    std::string current_return_type;
    int subroutine_local_variable_count;
    int label_count = 0;
    

public:
    compilation_engine() = default;
    compilation_engine(tokenizer::jack_tokenizer jt_tmp): jt{jt_tmp} {}
    void pass_tokenizer(tokenizer::jack_tokenizer jt_tmp) //this will reset the whole engine
    {
        jt = jt_tmp;
        xs.reset();
        tab_count = 0;
    }
    void compile()
    {
        compile_class();
    }
    
    void print()
    {
        std::cout << xs.xml_string << std::endl;
    }
    std::string return_parse_string()
    {
        return xs.xml_string;
    }
    

private:
    void error(std::string what, int line_num) //reports error
    {
        std::cout << xs.xml_string << std::endl;
        std::cout << jt.return_token_type() << std::endl;
        std::cerr << "Syntax Error : " + what + std::to_string(line_num)  << std::endl;
        exit(1);
    }

    void compile_class();
    void compile_class_var_dec();
    void compile_subroutine();
    void compile_subroutine_body();
    void compile_parameter_list();
    void compile_var_dec();
    void compile_statements();
    void compile_do();
    void compile_let();
    void compile_while();
    void compile_return();
    void compile_if();
    void compile_expression();
    void compile_term();
    int compile_expression_list();
    void compile_else(int);

    void increment_tab_count()
    {
        tab_count += 1;
    }

    void decrement_tab_count()
    {
        tab_count -= 1;
    }

};