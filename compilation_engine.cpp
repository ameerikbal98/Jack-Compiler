#include "compilation_engine.h"

using keyword_type = tokenizer::keyword_type;
using token_type = tokenizer::token_type;

/*
======================================
           Jack Language Grammar
======================================
This part in tokenizer:
Lexical Elements:
-----------------
keyword:
  class | constructor | function | method | field | static | var |
  int | char | boolean | void | true | false | null | this |
  let | do | if | else | while | return

symbol:
  { } ( ) [ ] . , ; + - * / & | < > = ~

integerConstant:
  A decimal number in the range 0–32767

stringConstant:
  A sequence of Unicode characters not including " or newline

identifier:
  A sequence of letters, digits, and underscore (_) not starting with a digit

This part in compilation engine:
Program Structure:
------------------
class:
  class className { classVarDec* subroutineDec* }

classVarDec:
  (static | field) type varName (',' varName)* ;

type:
  int | char | boolean | className

subroutineDec:
  (constructor | function | method)
  (void | type) subroutineName ( parameterList ) subroutineBody

parameterList:
  (type varName (',' type varName)*)?

subroutineBody:
  { varDec* statements }

varDec:
  var type varName (',' varName)* ;

className:
  identifier

subroutineName:
  identifier

varName:
  identifier


Statements:
-----------
statements:
  statement*

statement:
  letStatement | ifStatement | whileStatement | doStatement | returnStatement

letStatement:
  let varName ('[' expression ']')? = expression ;

ifStatement:
  if ( expression ) { statements } (else { statements })?

whileStatement:
  while ( expression ) { statements }

doStatement:
  do subroutineCall ;

returnStatement:
  return expression? ;


Expressions:
------------
expression:
  term (op term)*

term:
  integerConstant | stringConstant | keywordConstant |
  varName | varName [ expression ] | subroutineCall |
  ( expression ) | unaryOp term

subroutineCall:
  subroutineName ( expressionList ) |
  (className | varName) . subroutineName ( expressionList )

expressionList:
  (expression (',' expression)*)?

op:
  + | - | * | / | & | | | < | > | =

unaryOp:
  - | ~

keywordConstant:
  true | false | null | this

======================================

This program employs a recursive
descent parser to parse the jack
program

======================================
*/

void compilation_engine::compile_class()
{
    xs.enter_tag(std::string("class"), tab_count);
    increment_tab_count();
    if (jt.return_token_type() == token_type::KEYWORD && jt.return_keyword_type() == keyword_type::CLASS)
    {
        xs.enter_tag(std::string("keyword"), std::string("class"), tab_count);
        jt.advance();
        if (jt.return_token_type() == token_type::IDENTIFIER)
        {
            xs.enter_tag(std::string("identifier"), jt.return_identifier_string_const(), tab_count);
            class_name = jt.return_identifier_string_const();
            jt.advance();
            if (jt.return_token_type() == token_type::SYMBOL && jt.return_identifier_string_const() == "{")
            {
                xs.enter_tag(std::string("symbol"), "{", tab_count);
                jt.advance();
                while (jt.return_token_type() == token_type::KEYWORD &&
                       (jt.return_keyword_type() == keyword_type::STATIC || jt.return_keyword_type() == keyword_type::FIELD))
                {
                    xs.enter_tag(std::string("classVarDec"), tab_count);
                    increment_tab_count();
                    compile_class_var_dec();
                    decrement_tab_count();
                    xs.enter_tag(std::string("/classVarDec"), tab_count);
                }
                while (jt.return_token_type() == token_type::KEYWORD && (jt.return_keyword_type() == keyword_type::CONSTRUCTOR || jt.return_keyword_type() == keyword_type::FUNCTION || jt.return_keyword_type() == keyword_type::METHOD))
                {
                    xs.enter_tag(std::string("subroutineDec"), tab_count);
                    increment_tab_count();
                    compile_subroutine();
                    decrement_tab_count();
                    xs.enter_tag(std::string("/subroutineDec"), tab_count);
                }
                if (jt.return_token_type() == token_type::SYMBOL && jt.return_symbol() == '}')
                {
                    xs.enter_tag("symbol", "}", tab_count);
                    jt.advance();
                    decrement_tab_count();
                    xs.enter_tag(std::string("/class"), tab_count);
                }
                else
                {
                    error("Expected }, line: ", jt.return_linenum());
                }
            }
            else
            {
                error("expected { , line: ", jt.return_linenum());
            }
        }
        else
        {
            error("Identifier Error, Line num: ", jt.return_linenum());
        }
    }
    else
    {
        error("Expected keyword class, Line num: ", jt.return_linenum());
    }
}

void compilation_engine::compile_class_var_dec() // compiles and create parse tree for the class variables, defines the variables in the symbol tree
{
    std::string var_name, type;

    kind ki;

    xs.enter_tag("keyword", tokenizer::KEYWORDS[jt.return_keyword_type()], tab_count);
    if (jt.return_keyword_type() == keyword_type::FIELD)
    {
        ki = kind::field_k;
    }
    else if (jt.return_keyword_type() == keyword_type::STATIC)
    {
        ki = kind::static_k;
    }
    jt.advance();
    if (jt.return_token_type() == token_type::KEYWORD)
    {
        xs.enter_tag("keyword", tokenizer::KEYWORDS[jt.return_keyword_type()], tab_count);
        type = tokenizer::KEYWORDS[jt.return_keyword_type()];
        jt.advance();
    }
    else if (jt.return_token_type() == token_type::IDENTIFIER)
    {
        xs.enter_tag("identifier", jt.return_identifier_string_const(), tab_count);
        type = jt.return_identifier_string_const();
        jt.advance();
    }
    else
    {
        error("Expected a type, line: ", jt.return_linenum());
    }
    while (true) // this loop will run until ; symbol is found
    {

        if (jt.return_token_type() == token_type::IDENTIFIER)
        {
            xs.enter_tag("identifier", jt.return_identifier_string_const(), tab_count);
            var_name = jt.return_identifier_string_const();
            jt.advance();
        }

        symboltable_class.define(var_name, type, ki); // defines the variable in the symbol table;

        if (jt.return_token_type() == token_type::SYMBOL && jt.return_symbol() == ',') // if , found will repeat to
        {
            xs.enter_tag("symbol", ",", tab_count);
            jt.advance();
        }
        else if (jt.return_token_type() == token_type::SYMBOL && jt.return_symbol() == ';')
        {
            xs.enter_tag("symbol", ";", tab_count);
            jt.advance();
            break;
        }
        else
        {
            error("expected ;, line: ", jt.return_linenum());
        }
    }
    return;
}

void compilation_engine::compile_subroutine()
{
    current_return_type = "";
    symboltable_subroutine.start_subroutine();
    subroutine_local_variable_count = 0; //resetting the local variable count
    xs.enter_tag("keyword", tokenizer::KEYWORDS[jt.return_keyword_type()], tab_count);
    if (jt.return_keyword_type() == keyword_type::CONSTRUCTOR)
    {
        sub_type = subroutine_type::constructor;
        jt.advance();
        if (jt.return_token_type() == token_type::IDENTIFIER)
        {
            current_return_type = "this";
            xs.enter_tag("identifier", jt.return_identifier_string_const(), tab_count);
            jt.advance();
        }
        else
        {
            error("Expected class name, line: ", jt.return_linenum());
        }
        if (jt.return_token_type() == token_type::IDENTIFIER)
        {
            current_subroutine_name = jt.return_identifier_string_const();
            xs.enter_tag("identifier", jt.return_identifier_string_const(), tab_count);
            jt.advance();
        }
        else
        {
            error("Expected constructor id, line: ", jt.return_linenum());
        }
    }
    else // deals with method and functions
    {
        if(jt.return_keyword_type() == keyword_type::FUNCTION)
        {
            sub_type = subroutine_type::function;
        }
        else
        {
            symboltable_subroutine.set_subroutine_method();
            sub_type = subroutine_type::method;
        }
        jt.advance();
        if (jt.return_token_type() == token_type::IDENTIFIER)
        {
            current_return_type = jt.return_identifier_string_const();
            xs.enter_tag("identifier", jt.return_identifier_string_const(), tab_count);
            jt.advance();
        }
        else if (jt.return_token_type() == token_type::KEYWORD &&
                 (jt.return_keyword_type() == keyword_type::BOOLEAN || jt.return_keyword_type() == keyword_type::CHAR ||
                  jt.return_keyword_type() == keyword_type::INT || jt.return_keyword_type() == keyword_type::VOID))

        {
            current_return_type = tokenizer::KEYWORDS[jt.return_keyword_type()];
            std::cout << current_return_type << std::endl;
            xs.enter_tag("keyword", tokenizer::KEYWORDS[jt.return_keyword_type()], tab_count);
            jt.advance();
        }

        else
        {
            error("Expected return type, line: ", jt.return_linenum());
        }

        if (jt.return_token_type() == token_type::IDENTIFIER)
        {
            current_subroutine_name = jt.return_identifier_string_const();
            xs.enter_tag("identifier", jt.return_identifier_string_const(), tab_count);
            jt.advance();
        }
        else
        {
            error("Expected function name, line: ", jt.return_linenum());
        }
    }

    if (jt.return_token_type() == token_type::SYMBOL && jt.return_symbol() == '(')
    {
        xs.enter_tag("symbol", "(", tab_count);
        jt.advance();
    }
    else
    {
        error("Expected (, line: ", jt.return_linenum());
    }

    xs.enter_tag("parameterList", tab_count);
    increment_tab_count();
    compile_parameter_list();
    decrement_tab_count();
    xs.enter_tag("/parameterList", tab_count);
    if (jt.return_token_type() == token_type::SYMBOL && jt.return_symbol() == ')')
    {
        xs.enter_tag("symbol", ")", tab_count);
        jt.advance();
    }
    else
    {
        error("Expected ),line: ", jt.return_linenum());
    }
    xs.enter_tag("subroutineBody", tab_count);
    increment_tab_count();
    compile_subroutine_body();
    decrement_tab_count();
    xs.enter_tag("/subroutineBody", tab_count);
}

void compilation_engine::compile_subroutine_body()
{
    if (jt.return_token_type() == token_type::SYMBOL && jt.return_symbol() == '{')
    {
        xs.enter_tag("symbol", "{", tab_count);
        jt.advance();
    }
    else
    {
        error("Expected {, line: ", jt.return_linenum());
    }
    while (jt.return_token_type() == token_type::KEYWORD && jt.return_keyword_type() == keyword_type::VAR)
    {
        xs.enter_tag("varDec", tab_count);
        increment_tab_count();
        compile_var_dec();
        decrement_tab_count();
        xs.enter_tag("/varDec", tab_count);
    }
    symboltable_subroutine.print_symbols();
    vm_wr.write_function(class_name + "." + current_subroutine_name, subroutine_local_variable_count);
    if(sub_type == subroutine_type::constructor)
    {
        vm_wr.write_push(segments::CONST,symboltable_class.var_count(kind::field_k));
        vm_wr.write_call("Memory.alloc",1);
        vm_wr.write_pop(segments::POINTER,0);
    }
    else if(sub_type == subroutine_type::method)
    {
        vm_wr.write_push(segments::ARG,0);
        vm_wr.write_pop(segments::POINTER,0);
    }
    xs.enter_tag("statements", tab_count);
    increment_tab_count();
    compile_statements();
    decrement_tab_count();
    xs.enter_tag("/statements", tab_count);
    if (jt.return_token_type() == token_type::SYMBOL && jt.return_symbol() == '}')
    {
        xs.enter_tag("symbol", "}", tab_count);
        jt.advance();
    }
    else
    {
        error("Expected }, line: ", jt.return_linenum());
    }
}

void compilation_engine::compile_parameter_list()
{
    std::string var_name, type;

    kind ki = kind::arg_k;

    if (jt.return_token_type() == token_type::SYMBOL && jt.return_symbol() == ')')
    {
        return;
    }
    while (true)
    {
        if (jt.return_token_type() == token_type::KEYWORD && (jt.return_keyword_type() == keyword_type::BOOLEAN || jt.return_keyword_type() == keyword_type::CHAR || jt.return_keyword_type() == keyword_type::INT || jt.return_keyword_type() == keyword_type::VOID))
        { // considers built in types
            type = tokenizer::KEYWORDS[jt.return_keyword_type()];
            xs.enter_tag("keyword", tokenizer::KEYWORDS[jt.return_keyword_type()], tab_count);
            jt.advance();
        }
        else if (jt.return_token_type() == token_type::IDENTIFIER) // considers the user defined type
        {
            type = jt.return_identifier_string_const();
            xs.enter_tag("identifier", jt.return_identifier_string_const(), tab_count);
            jt.advance();
        }

        else
        {
            error("Expected a type", jt.return_linenum());
        }

        if (jt.return_token_type() == token_type::IDENTIFIER)
        {
            var_name = jt.return_identifier_string_const();
            xs.enter_tag("identifier", jt.return_identifier_string_const(), tab_count);
            jt.advance();
        }
        else
        {
            error("Expected identifier, line:", jt.return_linenum());
        }
        symboltable_subroutine.define(var_name, type, ki);
        if (jt.return_token_type() == token_type::SYMBOL && jt.return_symbol() == ',')
        {
            xs.enter_tag("symbol", ",", tab_count);
            jt.advance();
        }
        else if (jt.return_token_type() == token_type::SYMBOL && jt.return_symbol() == ')')
        {
            return;
        }
        else
        {
            error("Expected ), line: ", jt.return_linenum());
        }
    }
}

void compilation_engine::compile_var_dec()
{
    std::string var_name, type;
    kind ki = kind::var_k;   
    xs.enter_tag("keyword", tokenizer::KEYWORDS[jt.return_keyword_type()], tab_count);
    jt.advance();
    if (jt.return_token_type() == token_type::KEYWORD && (jt.return_keyword_type() == keyword_type::BOOLEAN || jt.return_keyword_type() == keyword_type::CHAR || jt.return_keyword_type() == keyword_type::INT || jt.return_keyword_type() == keyword_type::VOID))
    {
        type = tokenizer::KEYWORDS[jt.return_keyword_type()];
        xs.enter_tag("keyword", tokenizer::KEYWORDS[jt.return_keyword_type()], tab_count);
        jt.advance();
    }
    else if (jt.return_token_type() == token_type::IDENTIFIER)
    {
        type = jt.return_identifier_string_const();
        xs.enter_tag("identifier", jt.return_identifier_string_const(), tab_count);
        jt.advance();
    }
    else
    {
        error("Expected type, ", jt.return_linenum());
    }

    if (jt.return_token_type() == token_type::IDENTIFIER)
    {
        var_name = jt.return_identifier_string_const();
        xs.enter_tag("identifier", jt.return_identifier_string_const(), tab_count);
        jt.advance();
    }
    else
    {
        error("Expected identifier, line:", jt.return_linenum());
    }
    std::cout << var_name << " " << type << " " << ki << std::endl;
    symboltable_subroutine.define(var_name, type, ki); //define the variable in symbol table
    subroutine_local_variable_count++;

    if (jt.return_token_type() == token_type::SYMBOL && jt.return_symbol() == ',')
    {
        while (!(jt.return_token_type() == token_type::SYMBOL && jt.return_symbol() == ';'))
        {
            if (jt.return_token_type() == token_type::SYMBOL && jt.return_symbol() == ',')
                xs.enter_tag("symbol", ",", tab_count);
            else
                error("Expected, line: ", jt.return_linenum());
            jt.advance();
            if (jt.return_token_type() == token_type::IDENTIFIER)
            {
                var_name = jt.return_identifier_string_const();
                symboltable_subroutine.define(var_name, type, ki); //define the next variable in symbol table
                xs.enter_tag("identifier", jt.return_identifier_string_const(), tab_count);
                subroutine_local_variable_count++;
                jt.advance();
            }
            else
            {
                error("Expected identifier, line:", jt.return_linenum());
            }
        }
    }
    if (jt.return_token_type() == token_type::SYMBOL && jt.return_symbol() == ';')
    {
        xs.enter_tag("symbol", ";", tab_count);
        jt.advance();
    }
    else
    {
        error("Expected ;, line: ", jt.return_linenum());
    }
    return;
}

void compilation_engine::compile_statements()
{
    while (jt.return_token_type() == token_type::KEYWORD && (jt.return_keyword_type() == keyword_type::IF || jt.return_keyword_type() == keyword_type::LET ||
                                                             jt.return_keyword_type() == keyword_type::WHILE || jt.return_keyword_type() == keyword_type::DO || jt.return_keyword_type() == keyword_type::RETURN))
    {
        if (jt.return_keyword_type() == keyword_type::LET)
        {
            xs.enter_tag("letStatement", tab_count);
            increment_tab_count();
            compile_let();
            decrement_tab_count();
            xs.enter_tag("/letStatement", tab_count);
        }
        else if (jt.return_keyword_type() == keyword_type::DO)
        {
            xs.enter_tag("doStatement", tab_count);
            increment_tab_count();
            compile_do();
            decrement_tab_count();
            xs.enter_tag("/doStatement", tab_count);
        }
        else if (jt.return_keyword_type() == keyword_type::WHILE)
        {
            xs.enter_tag("whileStatement", tab_count);
            increment_tab_count();
            compile_while();
            decrement_tab_count();
            xs.enter_tag("/whileStatement", tab_count);
        }
        else if (jt.return_keyword_type() == keyword_type::RETURN)
        {
            xs.enter_tag("returnStatement", tab_count);
            increment_tab_count();
            compile_return();
            decrement_tab_count();
            xs.enter_tag("/returnStatement", tab_count);
        }
        else if (jt.return_keyword_type() == keyword_type::IF)
        {
            xs.enter_tag("ifStatement", tab_count);
            increment_tab_count();
            compile_if();
            decrement_tab_count();
            xs.enter_tag("/ifStatement", tab_count);
        }
    }
    return;
}

void compilation_engine::compile_let()
{
    std::string target;
    bool is_array = false;
    xs.enter_tag("keyword", "let", tab_count);
    jt.advance();
    if (jt.return_token_type() == token_type::IDENTIFIER)
    {
        target = jt.return_identifier_string_const();
        xs.enter_tag("identifier", jt.return_identifier_string_const(), tab_count);
        jt.advance();
    }
    else
    {
        error("Expected identifier, line:", jt.return_linenum());
    }

    if (jt.return_token_type() == token_type::SYMBOL && jt.return_symbol() == '[')
    {
        if(symboltable_class.symbol_exists_of(target))
        {
            segments seg = segments::ARG;
            if(symboltable_class.kind_of(target) == kind::static_k)
            {
                seg = segments::STATIC;
            }
            else if(symboltable_class.kind_of(target) == kind::field_k)
            {
                seg = segments::THIS;
            }
            vm_wr.write_push(seg,symboltable_class.index_of(target));
        }
        else if(symboltable_subroutine.symbol_exists_of(target))
        {
            segments seg = segments::ARG;
            if(symboltable_subroutine.kind_of(target) == kind::arg_k)
            {
                seg = segments::ARG;
            }
            else if(symboltable_subroutine.kind_of(target) == kind::var_k)
            {
                seg = segments::LOCAL;
            }
            vm_wr.write_push(seg,symboltable_subroutine.index_of(target));
        }
        is_array = true;
        xs.enter_tag("symbol", "[", tab_count);
        jt.advance();
        xs.enter_tag("expression", tab_count);
        increment_tab_count();
        compile_expression();
        decrement_tab_count();
        xs.enter_tag("/expression", tab_count);
        if (jt.return_token_type() == token_type::SYMBOL && jt.return_symbol() == ']')
        {
            xs.enter_tag("symbol", "]", tab_count);
            jt.advance();
        }
        else
        {
            error("Expected ], line: ", jt.return_linenum());
        }
        vm_wr.write_arithmetic(command::ADD);
    }
    

    if (jt.return_token_type() == token_type::SYMBOL && jt.return_symbol() == '=')
    {
        xs.enter_tag("symbol", "=", tab_count);
        jt.advance();
    }
    else
    {
        error("Exppected = , line:", jt.return_linenum());
    }
    xs.enter_tag("expression", tab_count);
    increment_tab_count();
    compile_expression();
    decrement_tab_count();
    xs.enter_tag("/expression", tab_count);
    segments seg = segments::CONST;
    if(is_array)
    {
        vm_wr.write_pop(segments::TEMP,0);
        vm_wr.write_pop(segments::POINTER,1);
        vm_wr.write_push(segments::TEMP,0);
        vm_wr.write_pop(segments::THAT,0);
    }
    else
    {
        if(symboltable_class.symbol_exists_of(target))
        {
            if(symboltable_class.kind_of(target) == kind::static_k)
            {
                seg = segments::STATIC;
            }
            else if(symboltable_class.kind_of(target) == kind::field_k)
            {
                seg = segments::THIS;
            }
            vm_wr.write_pop(seg,symboltable_class.index_of(target));
        }
        else if(symboltable_subroutine.symbol_exists_of(target))
        {
            
            if(symboltable_subroutine.kind_of(target) == kind::arg_k)
            {
                seg = segments::ARG;
            }
            else if(symboltable_subroutine.kind_of(target) == kind::var_k)
            {
                seg = segments::LOCAL;
            }
            vm_wr.write_pop(seg,symboltable_subroutine.index_of(target));
        }
    }
    
    
    if (jt.return_token_type() == token_type::SYMBOL && jt.return_symbol() == ';')
    {
        xs.enter_tag("symbol", ";", tab_count);
        jt.advance();
    }
    return;
}

void compilation_engine::compile_do()
{
    int num_of_args;
    std::string id1,id2;
    xs.enter_tag("keyword", "do", tab_count);
    jt.advance();
    if (jt.return_token_type() == token_type::IDENTIFIER)
    {
        id1 = jt.return_identifier_string_const();
        xs.enter_tag("identifier", jt.return_identifier_string_const(), tab_count);
        jt.advance();
    }

    if (jt.return_token_type() == token_type::SYMBOL && jt.return_symbol() == '(')
    {
        xs.enter_tag("symbol", "(", tab_count);
        jt.advance();
        xs.enter_tag("expressionList", tab_count);
        increment_tab_count();
        num_of_args = compile_expression_list();
        decrement_tab_count();
        xs.enter_tag("/expressionList", tab_count);
        if (jt.return_token_type() == token_type::SYMBOL && jt.return_symbol() == ')')
        {
            xs.enter_tag("symbol", ")", tab_count);
            jt.advance();
        }
        else
        {
            error("Expected ) , line: ", jt.return_linenum());
        }
        vm_wr.write_call(class_name + "." + id1,num_of_args);
        vm_wr.write_pop(segments::TEMP,0);
    }

    if (jt.return_token_type() == token_type::SYMBOL && jt.return_symbol() == '.')
    {
        xs.enter_tag("symbol", ".", tab_count);
        jt.advance();
        if (jt.return_token_type() == token_type::IDENTIFIER)
        {
            id2 = jt.return_identifier_string_const();
            xs.enter_tag("identifier", jt.return_identifier_string_const(), tab_count);
            jt.advance();
        }
        else
        {
            error("Expected identifier, line: ", jt.return_linenum());
        }

    

        if (jt.return_token_type() == token_type::SYMBOL && jt.return_symbol() == '(')
        {
            xs.enter_tag("symbol", "(", tab_count);
            jt.advance();
            xs.enter_tag("expressionList", tab_count);
            increment_tab_count();
            num_of_args = compile_expression_list();
            decrement_tab_count();
            xs.enter_tag("/expressionList", tab_count);
            if (jt.return_token_type() == token_type::SYMBOL && jt.return_symbol() == ')')
            {
                xs.enter_tag("symbol", ")", tab_count);
                jt.advance();
            }
            else
            {
                error("Expected ) , line: ", jt.return_linenum());
            }
            if(symboltable_class.symbol_exists_of(id1))
            {
                segments seg;
                if(symboltable_class.kind_of(id1) == kind::static_k)
                {
                    seg = segments::STATIC;
                }
                else if(symboltable_class.kind_of(id1) == kind::field_k)
                {
                    seg = segments::THIS;
                }
                vm_wr.write_push(seg,symboltable_class.index_of(id1));
                vm_wr.write_call(symboltable_class.type_of(id1) + "." + id2,num_of_args + 1);
                vm_wr.write_pop(segments::TEMP,0);
            }
            else if(symboltable_subroutine.symbol_exists_of(id1))
            {
                segments seg;
                if(symboltable_subroutine.kind_of(id1) == kind::var_k)
                {
                    seg = segments::LOCAL;
                }
                else if(symboltable_subroutine.kind_of(id1) == kind::arg_k)
                {
                    seg = segments::ARG;
                }
                vm_wr.write_push(seg,symboltable_subroutine.index_of(id1));
                vm_wr.write_call(symboltable_subroutine.type_of(id1) + "." + id2,num_of_args + 1);
                vm_wr.write_pop(segments::TEMP,0);
            }
            else
            {
                vm_wr.write_call(id1 + "." + id2 , num_of_args);
                vm_wr.write_pop(segments::TEMP,0);
            }
        }
    }


    if (jt.return_token_type() == token_type::SYMBOL && jt.return_symbol() == ';')
    {
        xs.enter_tag("symbol", ";", tab_count);
        jt.advance();
    }
    else
    {
        error("Expected ;, line: ", jt.return_linenum());
    }
    return;
}

void compilation_engine::compile_if()
{
    label_count += 1;
    int this_label_count = label_count;
    xs.enter_tag("keyword", "if", tab_count);
    jt.advance();
    if (jt.return_token_type() == token_type::SYMBOL && jt.return_symbol() == '(')
    {
        xs.enter_tag("symbol", "(", tab_count);
        jt.advance();
        xs.enter_tag("expression", tab_count);
        increment_tab_count();
        compile_expression();
        decrement_tab_count();
        xs.enter_tag("/expression", tab_count);
        vm_wr.write_arithmetic(command::NOT);
        vm_wr.write_if(std::string("L") + std::to_string(this_label_count) + "_initial");
        if (jt.return_token_type() == token_type::SYMBOL && jt.return_symbol() == ')')
        {
            xs.enter_tag("symbol", ")", tab_count);
            jt.advance();
            if (jt.return_token_type() == token_type::SYMBOL && jt.return_symbol() == '{')
            {
                xs.enter_tag("symbol", "{", tab_count);
                jt.advance();
                xs.enter_tag("statements", tab_count);
                increment_tab_count();
                compile_statements();
                decrement_tab_count();
                xs.enter_tag("/statements", tab_count);
                if (jt.return_token_type() == token_type::SYMBOL && jt.return_symbol() == '}')
                {
                    xs.enter_tag("symbol", "}", tab_count);
                    jt.advance();
                    vm_wr.write_goto(std::string("L") + std::to_string(this_label_count) + "_end");
                    vm_wr.write_label(std::string("L") + std::to_string(this_label_count) + "_initial");
                    if (jt.return_token_type() == token_type::KEYWORD && jt.return_keyword_type() == keyword_type::ELSE)
                        compile_else(this_label_count);
                    
                    vm_wr.write_label(std::string("L") + std::to_string(this_label_count) + "_end");
                    return;
                }
                else
                {
                    error("Expected }, line: ", jt.return_linenum());
                }
            }
            else
            {
                error("Expected { , line: ", jt.return_linenum());
            }
        }
        else
        {
            error("Expected ) , line: ", jt.return_linenum());
        }
    }
    else
    {
        error("Expected (, line: ", jt.return_linenum());
    }
    return;
}

void compilation_engine::compile_else(int this_label_count) // this is technically part of if
{
    xs.enter_tag("keyword", "else", tab_count);
    jt.advance();
    if (jt.return_token_type() == token_type::SYMBOL && jt.return_symbol() == '{')
    {
        xs.enter_tag("symbol", "{", tab_count);
        jt.advance();
        xs.enter_tag("statements", tab_count);
        increment_tab_count();
        compile_statements();
        decrement_tab_count();
        xs.enter_tag("/statements", tab_count);
        if (jt.return_token_type() == token_type::SYMBOL && jt.return_symbol() == '}')
        {
            xs.enter_tag("symbol", "}", tab_count);
            jt.advance();
            return;
        }
        else
        {
            error("Expected line: ", jt.return_linenum());
        }
    }
    else
    {
        error("Expected line: ", jt.return_linenum());
    }
}

void compilation_engine::compile_while()
{
    label_count += 1;
    int this_label = label_count;
    //=============================================================================================
    vm_wr.write_label(std::string("L") + std::to_string(this_label) + "_initial");
    //=============================================================================================
    if (jt.return_token_type() == token_type::KEYWORD && jt.return_keyword_type() == keyword_type::WHILE)
    {
        xs.enter_tag("keyword", "while", tab_count);
        jt.advance();
    }

    if (jt.return_token_type() == token_type::SYMBOL && jt.return_symbol() == '(')
    {
        xs.enter_tag("symbol", "(", tab_count);
        jt.advance();
    }
    else
    {
        error("Expected (, line: ", jt.return_linenum());
    }

    xs.enter_tag("expression", tab_count);
    increment_tab_count();
    compile_expression();
    decrement_tab_count();
    xs.enter_tag("/expression", tab_count);
    //================================================================================================
    vm_wr.write_arithmetic(command::NOT);
    vm_wr.write_if(std::string("L") + std::to_string(this_label) + "_end");
    //================================================================================================

    if (jt.return_token_type() == token_type::SYMBOL && jt.return_symbol() == ')')
    {
        xs.enter_tag("symbol", ")", tab_count);
        jt.advance();
    }
    else
    {
        error("Expected ), line: ", jt.return_linenum());
    }

    if (jt.return_token_type() == token_type::SYMBOL && jt.return_symbol() == '{')
    {
        xs.enter_tag("symbol", "{", tab_count);
        jt.advance();
        xs.enter_tag("statements", tab_count);
        increment_tab_count();
        compile_statements();
        decrement_tab_count();
        xs.enter_tag("/statements", tab_count);
    }
    else
    {
        error("Expected {, line: ", jt.return_linenum());
    }
    //=============================================================================
    vm_wr.write_goto(std::string("L") + std::to_string(this_label) + "_initial");
    vm_wr.write_label(std::string("L") + std::to_string(this_label) + "_end");
    //=============================================================================


    if (jt.return_token_type() == token_type::SYMBOL && jt.return_symbol() == '}')
    {
        xs.enter_tag("symbol", "}", tab_count);
        jt.advance();
        return;
    }
    else
    {
        error("Expected }, line: ", jt.return_linenum());
    }
}

void compilation_engine::compile_return() // parses return statement
{
    if (jt.return_token_type() == token_type::KEYWORD && jt.return_keyword_type() == keyword_type::RETURN)
    {
        xs.enter_tag("keyword", "return", tab_count);
        jt.advance();
        if (!(jt.return_token_type() == token_type::SYMBOL && jt.return_symbol() == ';'))
        {
            xs.enter_tag("expression", tab_count);
            increment_tab_count();
            compile_expression();
            decrement_tab_count();
            xs.enter_tag("/expression", tab_count);
        }
        if (jt.return_token_type() == token_type::SYMBOL && jt.return_symbol() == ';')
        {
            xs.enter_tag("symbol", ";", tab_count);
            jt.advance();
        }
        else
        {
            error("Expected ;, line: ", jt.return_linenum());
        }

        if(current_return_type == "void")
        {
            vm_wr.write_push(segments::CONST,0);
        }
        vm_wr.write_return();
    }
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------
// This part is parses the expression term--------------------------------------------------------------

void compilation_engine::compile_expression()
{
    xs.enter_tag("term", tab_count);
    increment_tab_count();
    compile_term();
    decrement_tab_count();
    xs.enter_tag("/term", tab_count);
    if (jt.return_token_type() == token_type::SYMBOL)
    {
        while (true)
        {
            switch (jt.return_symbol())
            {
            case '+':
                jt.advance();
                xs.enter_tag("symbol", "+", tab_count);
                xs.enter_tag("term", tab_count);
                increment_tab_count();
                compile_term();
                decrement_tab_count();
                xs.enter_tag("/term", tab_count);
                vm_wr.write_arithmetic(command::ADD);
                break;
            case '-':
                jt.advance();
                xs.enter_tag("symbol", "-", tab_count);
                xs.enter_tag("term", tab_count);
                increment_tab_count();
                compile_term();
                decrement_tab_count();
                xs.enter_tag("/term", tab_count);
                vm_wr.write_arithmetic(command::SUB);
                break;
            case '*':
                jt.advance();
                xs.enter_tag("symbol", "*", tab_count);
                xs.enter_tag("term", tab_count);
                increment_tab_count();
                compile_term();
                decrement_tab_count();
                xs.enter_tag("/term", tab_count);
                vm_wr.write_arithmetic(command::MUL);
                break;
            case '/':
                jt.advance();
                xs.enter_tag("symbol", "/", tab_count);
                xs.enter_tag("term", tab_count);
                increment_tab_count();
                compile_term();
                decrement_tab_count();
                xs.enter_tag("/term", tab_count);
                vm_wr.write_arithmetic(command::DIV);
                break;
            case '&':
                jt.advance();
                xs.enter_tag("symbol", "&amp;", tab_count);
                xs.enter_tag("term", tab_count);
                increment_tab_count();
                compile_term();
                decrement_tab_count();
                xs.enter_tag("/term", tab_count);
                vm_wr.write_arithmetic(command::AND);
                break;
            case '|':
                jt.advance();
                xs.enter_tag("symbol", "|", tab_count);
                xs.enter_tag("term", tab_count);
                increment_tab_count();
                compile_term();
                decrement_tab_count();
                xs.enter_tag("/term", tab_count);
                vm_wr.write_arithmetic(command::OR);
                break;
            case '<':
                jt.advance();
                xs.enter_tag("symbol", "&lt;", tab_count);
                xs.enter_tag("term", tab_count);
                increment_tab_count();
                compile_term();
                decrement_tab_count();
                xs.enter_tag("/term", tab_count);
                vm_wr.write_arithmetic(command::LT);
                break;
            case '>':
                jt.advance();
                xs.enter_tag("symbol", "&gt;", tab_count);
                xs.enter_tag("term", tab_count);
                increment_tab_count();
                compile_term();
                decrement_tab_count();
                xs.enter_tag("/term", tab_count);
                vm_wr.write_arithmetic(command::GT);
                break;
            case '=':
                jt.advance();
                xs.enter_tag("symbol", "=", tab_count);
                xs.enter_tag("term", tab_count);
                increment_tab_count();
                compile_term();
                decrement_tab_count();
                xs.enter_tag("/term", tab_count);
                vm_wr.write_arithmetic(command::EQ);
                break;
            case ',':
            case ';':
            case ')':
            case ']':
                return;
            default:
                error("invalid operand, line", jt.return_linenum());
                break;
            }
        }
    }
    else
    {
        error("Expected an operand, line:", jt.return_linenum());
    }
}

void compilation_engine::compile_term()
{
    if (jt.return_token_type() == token_type::INT_CONST)
    {
        vm_wr.write_push(segments::CONST,jt.return_integer());
        xs.enter_tag("integerConstant", std::to_string(jt.return_integer()), tab_count);
        jt.advance();
        return; 
    }
    else if (jt.return_token_type() == token_type::STRING_CONST)
    {
        std::string str = jt.return_identifier_string_const();
        int str_len = 0;
        if(str!="")
            str_len = str.length();
        vm_wr.write_push(segments::CONST,str_len);
        vm_wr.write_call("String.new",1);

        for(int i = 0; i < str_len;i++)
        {
            vm_wr.write_push(segments::CONST,str[i]);
            vm_wr.write_call("String.appendChar",2);
        }
        xs.enter_tag("stringConstant", jt.return_identifier_string_const(), tab_count);
        jt.advance();
        return;
    }
    else if (jt.return_token_type() == token_type::KEYWORD && (jt.return_keyword_type() == keyword_type::TRUE || jt.return_keyword_type() == keyword_type::FALSE || jt.return_keyword_type() == keyword_type::THIS || jt.return_keyword_type() == keyword_type::NULL_TYPE))
    {
        if(jt.return_keyword_type() == keyword_type::TRUE)
        {
            vm_wr.write_push(segments::CONST,0);
        }
        else if(jt.return_keyword_type() == keyword_type::FALSE)
        {
            vm_wr.write_push(segments::CONST,1);
            vm_wr.write_arithmetic(command::NEG);
        }
        else if(jt.return_keyword_type() == keyword_type::THIS)
        {
            vm_wr.write_push(segments::POINTER,0);
        }
        else if (jt.return_keyword_type() == keyword_type::NULL_TYPE)
        {
            vm_wr.write_push(segments::CONST,0);
        }
        xs.enter_tag("keyword", tokenizer::KEYWORDS[jt.return_keyword_type()], tab_count);
        jt.advance();
        return;
    }
    else if (jt.return_token_type() == token_type::IDENTIFIER)
    {
        std::string id1;
        std::string id2;
        id1 = jt.return_identifier_string_const();
        xs.enter_tag("identifier", jt.return_identifier_string_const(), tab_count);
        jt.advance();
        if (jt.return_token_type() == token_type::SYMBOL && jt.return_symbol() == '[')
        {
            if(symboltable_class.symbol_exists_of(id1))
            {
                segments seg;
                if(symboltable_class.kind_of(id1) == kind::static_k)
                {
                    seg = segments::STATIC;
                }
                else if(symboltable_class.kind_of(id1) == kind::field_k)
                {
                    seg = segments::THIS;
                }
                vm_wr.write_push(seg,symboltable_class.index_of(id1));
            }
            else if(symboltable_subroutine.symbol_exists_of(id1))
            {
                segments seg;
                if(symboltable_subroutine.kind_of(id1) == kind::var_k)
                {
                    seg = segments::LOCAL;
                }
                else if(symboltable_subroutine.kind_of(id1) == kind::arg_k)
                {
                    seg = segments::ARG;
                }
                vm_wr.write_push(seg,symboltable_subroutine.index_of(id1));
            }
            xs.enter_tag("symbol", "[", tab_count);
            jt.advance();
            xs.enter_tag("expression", tab_count);
            increment_tab_count();
            compile_expression();
            decrement_tab_count();
            xs.enter_tag("/expression", tab_count);
            vm_wr.write_arithmetic(command::ADD);
            vm_wr.write_pop(segments::POINTER,1);
            vm_wr.write_push(segments::THAT,0);
            if (jt.return_token_type() == token_type::SYMBOL && jt.return_symbol() == ']')
            {
                xs.enter_tag("symbol", "]", tab_count);
                jt.advance();
                return;
            }
            else
            {
                error("Expected ], line: ", jt.return_linenum());
            }
            
            
        }
        else if (jt.return_token_type() == token_type::SYMBOL && jt.return_symbol() == '.')
        {
            int num;
            xs.enter_tag("symbol", ".", tab_count);
            jt.advance();
            if (jt.return_token_type() == token_type::IDENTIFIER)
            {
                bool val = symboltable_subroutine.symbol_exists_of(id1);
                if(val)
                {
                    vm_wr.write_push(segments::LOCAL,symboltable_subroutine.index_of(id1));
                }
                id2 = jt.return_identifier_string_const();
                xs.enter_tag("identifier", jt.return_identifier_string_const(), tab_count);
                jt.advance();
                if (jt.return_token_type() == token_type::SYMBOL && jt.return_symbol() == '(')
                {
                    xs.enter_tag("symbol", "(", tab_count);
                    jt.advance();
                    xs.enter_tag("expressionList", tab_count);
                    increment_tab_count();
                    num = compile_expression_list();
                    decrement_tab_count();
                    xs.enter_tag("/expressionList", tab_count);

                    if(symboltable_class.symbol_exists_of(id1))
                    {
                        segments seg;
                        if(symboltable_class.kind_of(id1) == kind::static_k)
                        {
                            seg = segments::STATIC;
                        }
                        else if(symboltable_class.kind_of(id1) == kind::field_k)
                        {
                            seg = segments::THIS;
                        }
                        vm_wr.write_push(seg,symboltable_class.index_of(id1));
                        vm_wr.write_call(symboltable_class.type_of(id1) + "." + id2,num);
                    }
                    else if(symboltable_subroutine.symbol_exists_of(id1))
                    {
                        segments seg;
                        if(symboltable_subroutine.kind_of(id1) == kind::var_k)
                        {
                            seg = segments::LOCAL;
                        }
                        else if(symboltable_subroutine.kind_of(id1) == kind::arg_k)
                        {
                            seg = segments::ARG;
                        }
                        vm_wr.write_push(seg,symboltable_subroutine.index_of(id1));
                        vm_wr.write_call(symboltable_subroutine.type_of(id1) + "." + id2,num);
                    }
                    else
                    {
                        vm_wr.write_call(id1 + "." + id2 , num);
                    }

                    if (jt.return_token_type() == token_type::SYMBOL && jt.return_symbol() == ')')
                    {
                        xs.enter_tag("symbol", ")", tab_count);
                        jt.advance();
                        return;
                    }
                    else
                    {
                        error("Expected ) , line: ", jt.return_linenum());
                    }
                    if(val)
                    {
                        num = num + 1;
                    }

                    vm_wr.write_call(id1,num);
                }
                else
                {
                    error("Expected (, line: ", jt.return_linenum());
                }
            }
            else
            {
                error("Expected identifier, line: ", jt.return_linenum());
            }
        }
        else if (jt.return_token_type() == token_type::SYMBOL && jt.return_symbol() == '(')
        {
            int num_of_arg;
            xs.enter_tag("symbol", "(", tab_count);
            jt.advance();
            xs.enter_tag("expressionList", tab_count);
            increment_tab_count();
            num_of_arg = compile_expression_list();
            decrement_tab_count();
            xs.enter_tag("/expressionList", tab_count);
            if (jt.return_token_type() == token_type::SYMBOL && jt.return_symbol() == ')')
            {
                xs.enter_tag("symbol", ")", tab_count);
                jt.advance();
                return;
            }
            else
            {
                error("Expected ) , line: ", jt.return_linenum());
            }
            vm_wr.write_call(id1,num_of_arg);
        }
        else
        {
            if(symboltable_class.symbol_exists_of(id1))
            {
                segments seg;
                if(symboltable_class.kind_of(id1) == kind::static_k)
                {
                    seg = segments::STATIC;
                }
                else if(symboltable_class.kind_of(id1) == kind::field_k)
                {
                    seg = segments::THIS;
                }
                vm_wr.write_push(seg,symboltable_class.index_of(id1));
            }
            else if(symboltable_subroutine.symbol_exists_of(id1))
            {
                segments seg;
                if(symboltable_subroutine.kind_of(id1) == kind::var_k)
                {
                    seg = segments::LOCAL;
                }
                else if(symboltable_subroutine.kind_of(id1) == kind::arg_k)
                {
                    seg = segments::ARG;
                }
                vm_wr.write_push(seg,symboltable_subroutine.index_of(id1));
            }

        }
    }
    else if (jt.return_token_type() == token_type::SYMBOL)
    {
        switch (jt.return_symbol())
        {
        case '-': // unary - oeprator
            jt.advance();
            xs.enter_tag("symbol", "-", tab_count);
            xs.enter_tag("term", tab_count);
            increment_tab_count();
            compile_term();
            decrement_tab_count();
            xs.enter_tag("/term", tab_count);
            vm_wr.write_arithmetic(command::NEG);
            break;
        case '~': // unary not operator
            jt.advance();
            xs.enter_tag("symbol", "~", tab_count);
            xs.enter_tag("term", tab_count);
            increment_tab_count();
            compile_term();
            decrement_tab_count();
            xs.enter_tag("/term", tab_count);
            vm_wr.write_arithmetic(command::NOT);
            break;
        case '(': // '(' expression ')'
            jt.advance();
            xs.enter_tag("symbol", "(", tab_count);
            xs.enter_tag("expression", tab_count);
            increment_tab_count();
            compile_expression();
            decrement_tab_count();
            xs.enter_tag("/expression", tab_count);
            if (jt.return_token_type() == token_type::SYMBOL && jt.return_symbol() == ')')
                xs.enter_tag("symbol", ")", tab_count);
            else
                error("Expected ), line: ", jt.return_linenum());
            jt.advance();
            break;
        case ';':
            return;
        default:
            error("Invalid operator, line: ", jt.return_linenum());
            break;
        }
    }
    else
    {
        error("syntax error, line:nn ", jt.return_linenum());
    }
    return;
}

int compilation_engine::compile_expression_list()
{
    int num = 0;
    if (jt.return_token_type() == token_type::SYMBOL && jt.return_symbol() == ')')
    {
        return 0;
    }
    while (true)
    {
        num++; //calculates the number of arguments
        xs.enter_tag("expression", tab_count);
        increment_tab_count();
        compile_expression();
        decrement_tab_count();
        xs.enter_tag("/expression", tab_count);

        if (jt.return_token_type() == token_type::SYMBOL && jt.return_symbol() == ',')
        {
            xs.enter_tag("symbol", ",", tab_count);
            jt.advance();
        }
        else if (jt.return_token_type() == token_type::SYMBOL && jt.return_symbol() == ')')
        {
            return num; //returns the number of arguments
        }
        else
        {
            error("expected , line: ", jt.return_linenum());
        }
    }
}
