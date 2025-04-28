#include "vm_writer.h"



void vm_writer::write_push(segments seg, int num)
{
    filehandle << "push " << segments_string[seg]  <<  " " << std::to_string(num) << "\n";
}


void vm_writer::write_pop(segments seg, int num)
{
    filehandle << "pop " << segments_string[seg]  <<  " " << std::to_string(num) << "\n";
}


void vm_writer::write_arithmetic(command com)
{
    filehandle << command_string[com] << "\n";
}


void vm_writer::write_label(std::string label)
{
    filehandle << "label " << label << "\n";
}


void vm_writer::write_goto(std::string label)
{
    filehandle << "goto " << label << "\n";
}


void vm_writer::write_if(std::string label)
{
    filehandle << "if-goto " << label << "\n";
}


void vm_writer::write_call(std::string name, int num)
{
    filehandle << "call " << name << " " << num << "\n";
}

void vm_writer::write_function(std::string name, int num)
{
    filehandle << "function " << name << " " << num << "\n";
}

void vm_writer::write_return()
{
    filehandle << "return\n";
}