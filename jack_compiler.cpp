//This is part of the nand to tetris course 

#include <iostream>
#include <filesystem>
#include "jack_analyzer.h"
#include "regex_utils.h"
#include "jack_tokenizer.h"
#include "symbol_table.h"
#include "vm_writer.h"


int main(int argc, char *argv[])
{
    //jack_analyzer v ("Space Fighter");
    // if(argc != 2)
    // {
    //     std::cerr << "Usage : ./[name] filename";
    //     return(1);
    // }

    // jack_analyzer v (argv[1]);
    jack_analyzer v("Square");
    v.analyze();
}