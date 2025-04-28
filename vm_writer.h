#include <iostream>
#include <fstream>


enum segments
{
    CONST,ARG,STATIC,LOCAL,THIS,THAT,POINTER,TEMP
};

const char* segments_string[] {"constant","argment","static","local","this","that","pointer","temp"};

enum command
{
    ADD,SUB,NEG,EQ,GT,LT,AND,OR,NOT
};

const char* command_string[] {"add","sub","neg","eq","gt","lt","and","or","not"};



class vm_writer
{
    std::ofstream filehandle;
public:
    vm_writer(std::string str)
    {
        str = str + ".vm";
        filehandle.open(str);

        if (!filehandle.is_open())
        {
            throw std::runtime_error("Failed to open file: " + str);
        }
    }

    ~vm_writer()
    {
        if(filehandle.is_open())
        {
            filehandle.close();
        }
    }


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