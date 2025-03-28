#pragma once
#include "jack_tokenizer.h"


class compilation_engine
{
private:
    jack_tokenizer jt;

public:
    compilation_engine(jack_tokenizer jt_tmp): jt{jt_tmp} {}
    void compile()
    {
        compile_class();
    }  
    

private:
    void error(std::string what, int line_num) //reports error
    {
        std::cerr << "Syntax Error : " + what + std::to_string(line_num)  << std::endl;
    }

    void compile_class();
    void compile_class_var_dec();
    void compile_subroutine();
    void compile_parameter_list();
    void compile_var_dec();
    void compile_statements();
    void compile_do();
    void compile_let();
    void compile_while();
    void compile_return();
    void compile_return();
    void compile_return();
    void compile_if();
    void compile_expression();
    void compile_term();

};