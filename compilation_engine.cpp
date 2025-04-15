#include "compilation_engine.h"
#include <string>

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
    xs.enter_tag(std::string("class"),tab_count);
    increment_tab_count();
    if(jt.return_token_type() == token_type::KEYWORD && jt.return_keyword_type() == keyword_type::CLASS)
    {
        xs.enter_tag(std::string("token"),std::string("class"),tab_count);       
        jt.advance();
        if(jt.return_token_type() == token_type::IDENTIFIER)
        {
            xs.enter_tag(std::string("identifier"),jt.return_identifier_string_const(),tab_count);    
            jt.advance();
            if(jt.return_token_type() == token_type::SYMBOL && jt.return_identifier_string_const() == "{")
            {
                xs.enter_tag(std::string("symbol"), "{", tab_count); 
                jt.advance();
                while(jt.return_token_type() == token_type::KEYWORD && 
                    (jt.return_keyword_type() == keyword_type::STATIC || jt.return_keyword_type() == keyword_type::FIELD))
                {
                    xs.enter_tag(std::string("classVarDec"),tab_count);
                    increment_tab_count();
                    compile_class_var_dec();
                    decrement_tab_count();
                    xs.enter_tag(std::string("/classVarDec"),tab_count);
                }
                while(jt.return_token_type() == token_type::KEYWORD && (jt.return_keyword_type() == keyword_type::CONSTRUCTOR 
                    || jt.return_keyword_type() == keyword_type::FUNCTION || jt.return_keyword_type() == keyword_type::METHOD))
                {
                    xs.enter_tag(std::string("subroutineDec"),tab_count);
                    increment_tab_count();
                    compile_subroutine();
                    decrement_tab_count();
                    xs.enter_tag(std::string("/subroutineDec"),tab_count);
                    
                }
                
            }
            else
            {
                error("expected { , line: ",jt.return_linenum());
            }
        }
        else
        {
            error("Identifier Error, Line num: ",jt.return_linenum());
        }
    }
    else
    {
        error("Expected keyword class, Line num: ",jt.return_linenum());
    }
    
}


void compilation_engine::compile_class_var_dec() //compiles and create parse tree for the class variables
{
    xs.enter_tag("keyword",tokenizer::KEYWORDS[jt.return_keyword_type()],tab_count);
    jt.advance();

    while(true)
    {
        if(jt.return_token_type() == token_type::KEYWORD)
        {
            xs.enter_tag("keyword",tokenizer::KEYWORDS[jt.return_keyword_type()],tab_count);    
            jt.advance();
            if(jt.return_token_type() == token_type::IDENTIFIER)
            {
                xs.enter_tag("identifier",jt.return_identifier_string_const(),tab_count);    
                jt.advance();
            }
        } 
        else if(jt.return_token_type() == token_type::IDENTIFIER)
        {
            xs.enter_tag("identifier",jt.return_identifier_string_const(),tab_count);    
            jt.advance();
            if(jt.return_token_type() == token_type::IDENTIFIER)
            {
                xs.enter_tag("identifier",jt.return_identifier_string_const(),tab_count);    
                jt.advance();
            }
        }
        else
        {
            error("Expected a type, line: ",jt.return_linenum());
        }

        if(jt.return_token_type() == token_type::SYMBOL && jt.return_symbol() == ',')
        {
            xs.enter_tag("symbol",",",tab_count);
            jt.advance();
        }
        else if(jt.return_token_type() == token_type::SYMBOL && jt.return_symbol() == ';')
        {
            xs.enter_tag("symbol",";",tab_count);
            jt.advance();
            break;
        }
        else
        {
            error("expected ;, line: ",jt.return_linenum());
        }
    }
    return;
    
}


void compilation_engine::compile_subroutine()
{
    xs.enter_tag("keyword",tokenizer::KEYWORDS[jt.return_keyword_type()],tab_count);
    if(jt.return_keyword_type() == keyword_type::CONSTRUCTOR)
    {
        std::cout << 123 << std::endl;
        jt.advance();
        if(jt.return_token_type() == token_type::IDENTIFIER)
        {
            xs.enter_tag("identifier",jt.return_identifier_string_const(),tab_count);
            jt.advance();
            
        }
        else
        {
            error("Expected class name, line: ",jt.return_linenum());
        }
        if(jt.return_token_type() == token_type::IDENTIFIER)
        {
            xs.enter_tag("identifier",jt.return_identifier_string_const(),tab_count);
            jt.advance();
            
        }
        else
        {
            error("Expected constructor id, line: ",jt.return_linenum());
        }
    }
    else //deals with method and functions
    {
        jt.advance();
        if(jt.return_token_type() == token_type::IDENTIFIER)
        {
            xs.enter_tag("identifier",jt.return_identifier_string_const(),tab_count);
            jt.advance();
         
        }
        else if(jt.return_token_type() == token_type::KEYWORD && 
                (jt.return_keyword_type() == keyword_type::BOOLEAN || jt.return_keyword_type() == keyword_type::CHAR || 
                jt.return_keyword_type() == keyword_type::INT || jt.return_keyword_type() == keyword_type::VOID))

        {
            xs.enter_tag("keyword",tokenizer::KEYWORDS[jt.return_keyword_type()],tab_count);
            jt.advance(); 
        }

        else
        {
            error("Expected return type, line: ",jt.return_linenum());
        }

        if(jt.return_token_type() == token_type::IDENTIFIER)
        {
            xs.enter_tag("identifier",jt.return_identifier_string_const(),tab_count);
            jt.advance();
            
        }
        else
        {
            error("Expected function name, line: ",jt.return_linenum());
        }
        
    }

    if(jt.return_token_type() == token_type::SYMBOL && jt.return_symbol() == '(')
    {
        xs.enter_tag("symbol","(",tab_count);
        jt.advance();
    }
    else
    {
        error("Expected (, line: ",jt.return_linenum());
    }

    xs.enter_tag("parameterList",tab_count);
    increment_tab_count();
    compile_parameter_list();
    decrement_tab_count();
    xs.enter_tag("/parameterList",tab_count);
    if(jt.return_token_type() == token_type::SYMBOL && jt.return_symbol() == ')')
    {
        xs.enter_tag("symbol",")",tab_count);
        jt.advance();
    }
    else
    {
        error("Expected ),line: ",jt.return_linenum());
    }
    xs.enter_tag("subroutineBody",tab_count);
    increment_tab_count();
    compile_subroutine_body();
    decrement_tab_count();
    xs.enter_tag("/subroutineBody",tab_count);
}

void compilation_engine::compile_subroutine_body()
{
    if(jt.return_token_type() == token_type::SYMBOL && jt.return_symbol() == '{')
    {
        xs.enter_tag("symbol","{",tab_count);
        jt.advance();
    }
    else
    {
        error("Expected {, line: ",jt.return_linenum());
    }
    while(jt.return_token_type() == token_type::KEYWORD && jt.return_keyword_type() == keyword_type::VAR)
    {
        xs.enter_tag("varDec",tab_count);
        increment_tab_count();
        compile_var_dec();
        decrement_tab_count();
        xs.enter_tag("/varDec",tab_count);
    }
    xs.enter_tag("statments",tab_count);
    increment_tab_count();
    compile_statements();
    decrement_tab_count();
    xs.enter_tag("/statments",tab_count);
    if(jt.return_token_type() == token_type::SYMBOL && jt.return_symbol() == '}')
    {
        xs.enter_tag("symbol","}",tab_count);
        jt.advance();
    }
    else
    {
        error("Expected }, line: ",jt.return_linenum());
    }
}


void compilation_engine::compile_parameter_list()
{
    if(jt.return_token_type() == token_type::SYMBOL && jt.return_symbol() == ')')
    {
        return;
    }
    while(true)
    {
        if(jt.return_token_type() == token_type::KEYWORD && (jt.return_keyword_type() == keyword_type::BOOLEAN || jt.return_keyword_type() == keyword_type::CHAR || jt.return_keyword_type() == keyword_type::INT || jt.return_keyword_type() == keyword_type::VOID))
        { //considers built in types 
            xs.enter_tag("keyword",tokenizer::KEYWORDS[jt.return_keyword_type()],tab_count);
            jt.advance();
        }
        else if(jt.return_token_type() == token_type::IDENTIFIER ) //considers the user defined type
        {
            xs.enter_tag("identifier",jt.return_identifier_string_const(),tab_count);
            jt.advance();  
        }

        else
        {
            error("Expected a type",jt.return_linenum());
        }

        if(jt.return_token_type() == token_type::IDENTIFIER)
        {
            xs.enter_tag("identifier",jt.return_identifier_string_const(),tab_count);
            jt.advance();
            
        }
        else
        {
            error("Expected identifier, line:",jt.return_linenum());
        }
        if(jt.return_token_type() == token_type::SYMBOL && jt.return_symbol() == ',')
        {
            xs.enter_tag("symbol",",",tab_count);
            jt.advance();
        }
        else if(jt.return_token_type() == token_type::SYMBOL && jt.return_symbol() == ')')
        {
            return;
        }
        else
        {
            error("Expected ), line: ",jt.return_linenum());
        }
    }
}


void compilation_engine::compile_var_dec()
{
    xs.enter_tag("keyword",tokenizer::KEYWORDS[jt.return_keyword_type()],tab_count);
    jt.advance();
    if(jt.return_token_type() == token_type::KEYWORD && (jt.return_keyword_type() == keyword_type::BOOLEAN || jt.return_keyword_type() == keyword_type::CHAR || jt.return_keyword_type() == keyword_type::INT || jt.return_keyword_type() == keyword_type::VOID))
    {
        xs.enter_tag("keyword",tokenizer::KEYWORDS[jt.return_keyword_type()],tab_count);
        jt.advance();        
    }
    else if(jt.return_token_type() == token_type::IDENTIFIER)
    {
        xs.enter_tag("identifier",jt.return_identifier_string_const(),tab_count);
        jt.advance();
    }
    else
    {
        error("Expected type, ",jt.return_linenum());
    }

    if(jt.return_token_type() == token_type::IDENTIFIER)
    {
        xs.enter_tag("identifier",jt.return_identifier_string_const(),tab_count);
        jt.advance();
    }
    else
    {
        error("Expected identifier, line:", jt.return_linenum());
    }

    if(jt.return_token_type() == token_type::SYMBOL && jt.return_symbol() == ';')
    {
        xs.enter_tag("symbol",";",tab_count);
        jt.advance();
    }
    else
    {
        error("Expected ;, line: ", jt.return_linenum());
    }

    if(jt.return_token_type() == token_type::KEYWORD && jt.return_keyword_type() == keyword_type::VAR)
    {
        compile_var_dec();
    }
    return;
}

void compilation_engine::compile_statements()
{
    while(jt.return_token_type() == token_type::KEYWORD && (jt.return_keyword_type() == keyword_type::IF || jt.return_keyword_type() == keyword_type::LET ||
                                                            jt.return_keyword_type() == keyword_type::WHILE || jt.return_keyword_type() == keyword_type::DO || jt.return_keyword_type() == keyword_type::RETURN ))
    {
        if(jt.return_keyword_type() == keyword_type::LET)
        {
            xs.enter_tag("letStatement",tab_count);
            increment_tab_count();
            compile_let();
            decrement_tab_count();
            xs.enter_tag("/letStatement",tab_count);
        }
        else if(jt.return_keyword_type() == keyword_type::DO)
        {
            xs.enter_tag("doStatement",tab_count);
            increment_tab_count();
            compile_do();
            decrement_tab_count();
            xs.enter_tag("/doStatement",tab_count);
        }
        else if(jt.return_keyword_type() == keyword_type::WHILE)
        {
            xs.enter_tag("whileStatement",tab_count);
            increment_tab_count();
            compile_while();
            decrement_tab_count();
            xs.enter_tag("/whileStatement",tab_count);
        }
        else if(jt.return_keyword_type() == keyword_type::RETURN)
        {
            xs.enter_tag("returnStatement",tab_count);
            increment_tab_count();
            compile_return();
            decrement_tab_count();
            xs.enter_tag("/returnStatement",tab_count);
        }
        else if(jt.return_keyword_type() == keyword_type::IF)
        {
            xs.enter_tag("ifStatement",tab_count);
            increment_tab_count();
            compile_if();
            decrement_tab_count();
            xs.enter_tag("ifStatement",tab_count);
        }
    }
    return;
}





void compilation_engine::compile_let()
{
    xs.enter_tag("keyword","let",tab_count);
    jt.advance();
    if(jt.return_token_type() == token_type::IDENTIFIER)
    {
        xs.enter_tag("identifier",jt.return_identifier_string_const(),tab_count);
        jt.advance();
    }
    else
    {
        error("Expected identifier, line:",jt.return_linenum());
    }

    if(jt.return_token_type() == token_type::SYMBOL && jt.return_symbol() == '=')
    {
        xs.enter_tag("symbol","=",tab_count);
        jt.advance();
    }
    else
    {
        error("Exppected = , line:",jt.return_linenum());
    }
    xs.enter_tag("expression",tab_count);
    increment_tab_count();
    compile_expression();
    decrement_tab_count();
    xs.enter_tag("/expression",tab_count);
    if(jt.return_token_type() == token_type::SYMBOL && jt.return_symbol() == ';')
    {
        xs.enter_tag("symbol",";",tab_count);
        jt.advance();
    }
    return;
}

void compilation_engine::compile_do()
{
    xs.enter_tag("keyword","do",tab_count);
    jt.advance();
    xs.enter_tag("expression",tab_count);
    increment_tab_count();
    compile_expression();
    decrement_tab_count();
    xs.enter_tag("/expression",tab_count);
    if(jt.return_token_type() == token_type::SYMBOL && jt.return_symbol() == ';')
    {
        xs.enter_tag("symbol",";",tab_count);
        jt.advance();
    }
    else
    {
        error("Expected ;, line: ",jt.return_linenum());
    }
    return;

}

void compilation_engine::compile_if()
{
    xs.enter_tag("keyword","if",tab_count);
    jt.advance();
    if(jt.return_token_type() == token_type::SYMBOL && jt.return_symbol() == '(')
    {
        xs.enter_tag("symbol","(",tab_count);
        jt.advance();
        xs.enter_tag("expression",tab_count);
        increment_tab_count();
        compile_expression();
        decrement_tab_count();
        xs.enter_tag("/expression",tab_count);
        if(jt.return_token_type() == token_type::SYMBOL && jt.return_symbol() == ')')
        {
            xs.enter_tag("symbol",")",tab_count);
            jt.advance();
            if(jt.return_token_type() == token_type::SYMBOL && jt.return_symbol() == '{')
            {
                xs.enter_tag("symbol","{",tab_count);
                jt.advance();
                xs.enter_tag("statements",tab_count);
                increment_tab_count();
                compile_statements();
                decrement_tab_count();
                xs.enter_tag("/statments",tab_count);
                if(jt.return_token_type() == token_type::SYMBOL && jt.return_symbol() == '}')
                {
                    xs.enter_tag("symbol","}",tab_count);
                    jt.advance();
                    if(jt.return_token_type() == token_type::KEYWORD && jt.return_keyword_type() == keyword_type::ELSE)
                        compile_else();
                    return;
                }
                else
                {
                    error("Expected }, line: ",jt.return_linenum());
                }

            }
            else
            {
                error("Expected { , line: ",jt.return_linenum());
            }
        }
        else
        {
            error("Expected ) , line: ",jt.return_linenum());
        }
    }
    else
    {
        error("Expected (, line: ",jt.return_linenum());
    }
    return;
}

void compilation_engine::compile_else() //this is technically part of if
{
    xs.enter_tag("keyword","else",tab_count);
    jt.advance();
    if(jt.return_token_type() == token_type::SYMBOL && jt.return_symbol() == '{')
    {
        xs.enter_tag("symbo","{",tab_count);
        jt.advance();
        xs.enter_tag("statements",tab_count);
        increment_tab_count();
        compile_statements();
        decrement_tab_count();
        xs.enter_tag("/statments",tab_count);
        if(jt.return_symbol() == '}')
        if(jt.return_token_type() == token_type::SYMBOL && jt.return_symbol() == '}')
        {
            xs.enter_tag("symbol","}",tab_count);
            jt.advance();
            return;
        }
        else
        {
            error("Expected line: ",jt.return_linenum());
        }
    }
    else
    {
        error("Expected line: ",jt.return_linenum());
    }
}

void compilation_engine::compile_while()
{
    if(jt.return_token_type() == token_type::KEYWORD && jt.return_keyword_type() == keyword_type::WHILE)
    {
        xs.enter_tag("keyword","while",tab_count);
        jt.advance();
    }

    if(jt.return_token_type() == token_type::SYMBOL && jt.return_symbol() == '(')
    {
        xs.enter_tag("symbol","(",tab_count);
        jt.advance();
    }
    else
    {
        error("Expected (, line: ",jt.return_linenum());
    }

    xs.enter_tag("expression",tab_count);
    increment_tab_count();
    compile_expression();
    decrement_tab_count();
    xs.enter_tag("/expression",tab_count);
    if(jt.return_token_type() == token_type::SYMBOL && jt.return_symbol() == ')')
    {
        xs.enter_tag("symbol",")",tab_count);
        jt.advance();
    }
    else
    {
        error("Expected ), line: ",jt.return_linenum());
    }

    if(jt.return_token_type() == token_type::SYMBOL && jt.return_symbol() == '{')
    {
        xs.enter_tag("symbol","{",tab_count);
        jt.advance();
        xs.enter_tag("statements",tab_count);
        increment_tab_count();
        compile_statements();
        decrement_tab_count();
        xs.enter_tag("/statments",tab_count);
        
    }
    else
    {
        error("Expected {, line: ",jt.return_linenum());
    }
    if(jt.return_token_type() == token_type::SYMBOL && jt.return_symbol() == '}')
    {
        xs.enter_tag("symbol","}",tab_count);
        jt.advance();
        return;
    }
    else
    {
        error("Expected }, line: ",jt.return_linenum());
    }
}


void compilation_engine::compile_return() //parses return statement
 {
    if(jt.return_token_type() == token_type::KEYWORD && jt.return_keyword_type() == keyword_type::RETURN)
    {
        xs.enter_tag("keyword","return",tab_count);
        jt.advance();
        if(!(jt.return_token_type() == token_type::SYMBOL && jt.return_symbol() == ';'))
        {
            xs.enter_tag("expression",tab_count);
            increment_tab_count();
            compile_expression();
            decrement_tab_count();
            xs.enter_tag("/expression",tab_count);
        
        }
        if(jt.return_token_type() == token_type::SYMBOL && jt.return_symbol() == ';')
        {
            xs.enter_tag("symbol",";",tab_count);
            jt.advance();
        }
        else
        {
            error("Expected ;, line: ",jt.return_linenum());
        }
    }
 }

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------
//This part is parses the expression term--------------------------------------------------------------

void compilation_engine::compile_expression()
{
    if(jt.return_token_type() == token_type::SYMBOL)
        std::cout << jt.return_symbol() << std::endl;
    xs.enter_tag("term",tab_count);
    increment_tab_count();
    compile_term();
    decrement_tab_count();
    xs.enter_tag("/term",tab_count);
    if(jt.return_token_type() == token_type::SYMBOL)
    {
        while(true)
        {
            switch(jt.return_symbol())
            {
                case '+':
                    jt.advance();
                    xs.enter_tag("symbol","+",tab_count);
                    xs.enter_tag("term",tab_count);
                    increment_tab_count();
                    compile_term();
                    decrement_tab_count();
                    xs.enter_tag("/term",tab_count);
                    break;
                case '-':
                    jt.advance();
                    xs.enter_tag("symbol","-",tab_count);
                    xs.enter_tag("term",tab_count);
                    increment_tab_count();
                    compile_term();
                    decrement_tab_count();
                    xs.enter_tag("/term",tab_count);
                    break;
                case '*':
                    jt.advance();
                    xs.enter_tag("symbol","*",tab_count);
                    xs.enter_tag("term",tab_count);
                    increment_tab_count();
                    compile_term();
                    decrement_tab_count();
                    xs.enter_tag("/term",tab_count);
                    break;
                case '/':
                    jt.advance();
                    xs.enter_tag("symbol","/",tab_count);
                    xs.enter_tag("term",tab_count);
                    increment_tab_count();
                    compile_term();
                    decrement_tab_count();
                    xs.enter_tag("/term",tab_count);
                    break;
                case '&':
                    jt.advance();
                    xs.enter_tag("symbol","&",tab_count);
                    xs.enter_tag("term",tab_count);
                    increment_tab_count();
                    compile_term();
                    decrement_tab_count();
                    xs.enter_tag("/term",tab_count);
                    break;
                case '|':
                    jt.advance();
                    xs.enter_tag("symbol","|",tab_count);
                    xs.enter_tag("term",tab_count);
                    increment_tab_count();
                    compile_term();
                    decrement_tab_count();
                    xs.enter_tag("/term",tab_count);
                    break;
                case '<':
                    jt.advance();
                    xs.enter_tag("symbol","&lgt;",tab_count);
                    xs.enter_tag("term",tab_count);
                    increment_tab_count();
                    compile_term();
                    decrement_tab_count();
                    xs.enter_tag("/term",tab_count);
                    break;
                case '>':
                    jt.advance();
                    xs.enter_tag("symbol","&ggt;",tab_count);
                    xs.enter_tag("term",tab_count);
                    increment_tab_count();
                    compile_term();
                    decrement_tab_count();
                    xs.enter_tag("/term",tab_count);
                    break;
                case '=':
                    jt.advance();
                    xs.enter_tag("symbol","=",tab_count);
                    xs.enter_tag("term",tab_count);
                    increment_tab_count();
                    compile_term();
                    decrement_tab_count();
                    xs.enter_tag("/term",tab_count);
                    break;
                case ',': case ';': case ')': case ']':
                    return;
                default:
                    error("invalid operand, line",jt.return_linenum());
                    break;
            }
        }
    }
    else
    {
        error("Expected an operand, line:",jt.return_linenum());
    }
}

void compilation_engine::compile_term()
{
    if(jt.return_token_type() == token_type::INT_CONST)
    {
        xs.enter_tag("integerConstant",std::to_string(jt.return_integer()),tab_count);
        jt.advance();
        return;
    }
    else if(jt.return_token_type() == token_type::STRING_CONST)
    {
        xs.enter_tag("stringConstant",jt.return_identifier_string_const(),tab_count);
        jt.advance();
        return;
    }
    else if(jt.return_token_type() == token_type::KEYWORD && (jt.return_keyword_type() == keyword_type::TRUE || jt.return_keyword_type() == keyword_type::FALSE || jt.return_keyword_type() == keyword_type::THIS || jt.return_keyword_type() == keyword_type::NULL_TYPE))
    {
        xs.enter_tag("keywordConstant",tokenizer::KEYWORDS[jt.return_keyword_type()],tab_count);
        jt.advance();
        return;
    }
    else if(jt.return_token_type() == token_type::IDENTIFIER)
    {
        xs.enter_tag("identifier",jt.return_identifier_string_const(),tab_count);
        jt.advance();
        if(jt.return_token_type() == token_type::SYMBOL && jt.return_symbol() == '[')
        {
            xs.enter_tag("symbol","[",tab_count);
            jt.advance();
            xs.enter_tag("expression",tab_count);
            increment_tab_count();
            compile_expression();
            decrement_tab_count();
            xs.enter_tag("/expression",tab_count);
            if(jt.return_token_type() == ']' && jt.return_symbol() == ']')
            {
                xs.enter_tag("symbol","]",tab_count);
                jt.advance();
                return;
            }
            else
            {
                error("Expected ], line: ",jt.return_linenum());
            }
            
        }
        else if(jt.return_token_type() == token_type::SYMBOL && jt.return_symbol() == '.')
        {
            xs.enter_tag("symbol",".",tab_count);
            jt.advance();
            if(jt.return_token_type() == token_type::IDENTIFIER)
            {
                xs.enter_tag("identifier",jt.return_identifier_string_const(),tab_count);
                jt.advance();
                if(jt.return_token_type() == token_type::SYMBOL && jt.return_symbol() == '(')
                {
                    xs.enter_tag("symbol","(",tab_count);
                    jt.advance();
                    xs.enter_tag("expressionList",tab_count);
                    increment_tab_count();
                    compile_expression_list();
                    decrement_tab_count();
                    xs.enter_tag("/expressionList",tab_count);
                    if(jt.return_token_type() == token_type::SYMBOL && jt.return_symbol() == ')')
                    {
                        xs.enter_tag("symbol",")",tab_count);
                        jt.advance();
                        return;
                    }
                    else
                    {
                        error("Expected ) , line: ",jt.return_linenum());
                    }
                }
                else
                {
                    error("Expected (, line: ",jt.return_linenum());
                }
            }
            else
            {
                error("Expected identifier, line: ", jt.return_linenum());
            }
        }
        else if(jt.return_token_type() == token_type::SYMBOL && jt.return_symbol() == '(')
        {
            xs.enter_tag("symbol","(",tab_count);
            jt.advance();
            xs.enter_tag("expressionList",tab_count);
            increment_tab_count();
            compile_expression_list();
            decrement_tab_count();
            xs.enter_tag("/expressionList",tab_count);
            if(jt.return_token_type() == token_type::SYMBOL && jt.return_symbol() == ')')
            {
                xs.enter_tag("symbol",")",tab_count);
                jt.advance();
                return;
            }
            else
            {
                error("Expected ) , line: ",jt.return_linenum());
            }
        }
    }
    else if(jt.return_token_type() == token_type::SYMBOL)
    {
        switch(jt.return_symbol())
        {
            case '-'://unary - oeprator
                jt.advance();
                xs.enter_tag("symbol","-",tab_count);
                xs.enter_tag("term",tab_count);
                increment_tab_count();
                compile_term();
                decrement_tab_count();
                xs.enter_tag("/term",tab_count);
                break;
            case '~': //unary not operator
                jt.advance();
                xs.enter_tag("symbol","~",tab_count);
                xs.enter_tag("term",tab_count);
                increment_tab_count();
                compile_term();
                decrement_tab_count();
                xs.enter_tag("/term",tab_count);
                break;
            case '(': // '(' expression ')'
                jt.advance();
                xs.enter_tag("symbol","(",tab_count);
                xs.enter_tag("expression",tab_count);
                increment_tab_count();
                compile_expression();
                decrement_tab_count();
                xs.enter_tag("/expression",tab_count);
                xs.enter_tag("symbol",")",tab_count);
                jt.advance();
                break;
            case ';':
                return;
            default:
                std::cout << jt.return_symbol() << std::endl;
                error("Wrong symbol, line: ",jt.return_linenum());
                break;
        }
    }
    else
    {
        error("syntax error, line:nn ",jt.return_linenum());
    }
    return;
}


void compilation_engine::compile_expression_list()
{
    if(jt.return_token_type() == token_type::SYMBOL && jt.return_symbol() == ')')
    {
        return;
    }
    while(true)
    {
        xs.enter_tag("expression",tab_count);
        increment_tab_count();
        compile_expression();
        decrement_tab_count();
        xs.enter_tag("/expression",tab_count);
        
        if(jt.return_token_type() == token_type::SYMBOL && jt.return_symbol() == ',')
        {
            xs.enter_tag("symbol",",",tab_count);
            jt.advance();
        }
        else if(jt.return_token_type() == token_type::SYMBOL && jt.return_symbol() == ')')
        {
            return;
        }
        else
        {
            error("expected , line: ",jt.return_linenum());
        }
    }
}


void compilation_engine::compile_while()
{
    if(jt.return_token_type() == token_type::KEYWORD && jt.return_keyword_type() == keyword_type::WHILE)
    {
        xs.enter_tag("keyword","while",tab_count);
        jt.advance();
    }

    if(jt.return_token_type() == token_type::SYMBOL && jt.return_symbol() == '(')
    {
        xs.enter_tag("symbol","(",tab_count);
        jt.advance();
    }
    else
    {
        error("Expected (, line: ",jt.return_linenum());
    }

    xs.enter_tag("expression",tab_count);
    increment_tab_count();
    compile_expression();
    decrement_tab_count();
    xs.enter_tag("/expression",tab_count);
    if(jt.return_token_type() == token_type::SYMBOL && jt.return_symbol() == ')')
    {
        xs.enter_tag("symbol",")",tab_count);
        jt.advance();
    }
    else
    {
        error("Expected ), line: ",jt.return_linenum());
    }

    if(jt.return_token_type() == token_type::SYMBOL && jt.return_symbol() == '{')
    {
        xs.enter_tag("symbol","{",tab_count);
        jt.advance();
        xs.enter_tag("statements",tab_count);
        increment_tab_count();
        compile_statements();
        decrement_tab_count();
        xs.enter_tag("/statments",tab_count);
        
    }
    else
    {
        error("Expected {, line: ",jt.return_linenum());
    }
    if(jt.return_token_type() == token_type::SYMBOL && jt.return_symbol() == '}')
    {
        xs.enter_tag("symbol","}",tab_count);
        jt.advance();
        return;
    }
    else
    {
        error("Expected }, line: ",jt.return_linenum());
    }
}


void compilation_engine::compile_return()
 {
    if(jt.return_token_type() == token_type::KEYWORD && jt.return_keyword_type() == keyword_type::RETURN)
    {
        xs.enter_tag("keyword","return",tab_count);
        jt.advance();
        if(!(jt.return_token_type() == token_type::SYMBOL && jt.return_symbol() == ';'))
        {
            xs.enter_tag("expression",tab_count);
            increment_tab_count();
            compile_expression();
            decrement_tab_count();
            xs.enter_tag("/expression",tab_count);
        
        }
        if(jt.return_token_type() == token_type::SYMBOL && jt.return_symbol() == ';')
        {
            xs.enter_tag("symbol",";",tab_count);
            jt.advance();
        }
        else
        {
            error("Expected ;, line: ",jt.return_linenum());
        }
    }
 }