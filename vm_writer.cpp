#include "vm_writer.h"
#include <string>



void vm_writer::write_push(segments seg, int num)
{
    vm_file.append(std::string("push ") + segments_string[seg]  +  " " + std::to_string(num) + "\n");
}


void vm_writer::write_pop(segments seg, int num)
{
    vm_file.append(std::string("pop ") + segments_string[seg]  +  " " + std::to_string(num) + "\n");
}


void vm_writer::write_arithmetic(command com)
{
    if(com == command::MUL)
    {
        vm_file.append("call Math.multiply 2\n");
    }
    else if(com == command::DIV)
    {
        vm_file.append("call Math.divide 2\n");
    }
    else
    {
        vm_file.append(std::string(command_string[com]) + "\n");
    }
    
}


void vm_writer::write_label(std::string label)
{
    vm_file.append(std::string("label ") + label + "\n");
}


void vm_writer::write_goto(std::string label)
{
    vm_file.append(std::string("goto ") + label + "\n");
}


void vm_writer::write_if(std::string label)
{
    vm_file.append(std::string("if-goto ") + label + "\n");
}


void vm_writer::write_call(std::string name, int num)
{
    vm_file.append(std::string("call ") + name + " " + std::to_string(num) + "\n");
}

void vm_writer::write_function(std::string name, int num)
{
    vm_file.append(std::string("function ") + name + " " + std::to_string(num) + "\n");
}

void vm_writer::write_return()
{
    vm_file.append(std::string("return") + "\n");
}
