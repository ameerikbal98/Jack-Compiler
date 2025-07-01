#pragma once
#include <iostream>
#include <string>


enum segments
{
    CONST,ARG,STATIC,LOCAL,THIS,THAT,POINTER,TEMP
};

inline const char* segments_string[] {"constant","argument","static","local","this","that","pointer","temp"};

enum command
{
    ADD,SUB,NEG,EQ,GT,LT,AND,OR,NOT,MUL,DIV
};

inline const char* command_string[] {"add","sub","neg","eq","gt","lt","and","or","not","MUL","DIV"};


class vm_writer
{
    std::string vm_file;
public:
    vm_writer() = default;

    std::string return_vm_file() { return vm_file; }


    void write_pop(segments,int);
    void write_push(segments,int);
    void write_arithmetic(command);
    void write_label(std::string);
    void write_goto(std::string);
    void write_if(std::string);
    void write_call(std::string,int);
    void write_function(std::string,int);
    void write_return();

};